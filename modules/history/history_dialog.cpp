/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QApplication>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QSplitter>
#include <QtGui/QVBoxLayout>
#include <QtGui/QMenu>

#include "chat_message.h"
#include "chat_messages_view.h"
#include "config_file.h"
#include "debug.h"
#include "emoticons.h"
#include "gadu_images_manager.h"
#include "kadu.h"
#include "misc.h"
#include "icons_manager.h"

#include "history_dialog.h"

UinsListViewText::UinsListViewText(QTreeWidget *parent, const UinsList &uins)
	: QTreeWidgetItem(parent), uins(uins)
{
//	kdebugf();
	QString name;

	if (uins.isEmpty())
		setText(0, "SMS");
	else
	{
		uint i = 0, uinsCount = uins.count();
		bool found;
		QList<UserListElement> users = userlist->toList();
		foreach(const UinType &uin, uins)
		{
			found = false;
			/* Dorr: it's faster than using find() function */
			foreach (const UserListElement &user, users)
			{
				if (user.usesProtocol("Gadu") && user.ID("Gadu") == QString::number(uin))
				{
					name.append(user.altNick());
					found = true;
					break;
				}
			}
			if (!found)
				name.append(QString::number(uin));

			if (i++ < uinsCount - 1)
				name.append(",");
		}
		setText(0, name);
	}
//	kdebugf2();
}

const UinsList &UinsListViewText::getUinsList() const
{
	return uins;
}

DateListViewText::DateListViewText(QTreeWidgetItem *parent, UinsList uins, const HistoryDate &date, const QList<QDate> &messageDates)
	: QTreeWidgetItem(parent, 0), date(date)
{
	setText(0, date.date.toString("yyyy.MM.dd"));
	containsMessages = messageDates.contains(date.date.date());
	if (!containsMessages)
	{
		kdebugmf(KDEBUG_INFO, "Date '%s' not found in messageDates. Probably contains only status changes\n", qPrintable(date.date.toString("yyyy.MM.dd")));
	}
}

void DateListViewText::showStatusChanges(bool showStatus)
{
	setHidden(!showStatus && !containsMessages);
}

const HistoryDate &DateListViewText::getDate() const
{
	return date;
}

HistoryDialog::HistoryDialog(UinsList uins)
	: QWidget(kadu, Qt::Window),
	 uinsTreeWidget(0), body(0), uins(uins), start(0), findRec(), closeDemand(false), finding(false), dateEntries()
{
	kdebugf();

	setWindowTitle(tr("History"));
	setAttribute(Qt::WA_DeleteOnClose);

	QGridLayout *grid = new QGridLayout(this);
	grid->setMargin(0);
	grid->setSpacing(0);

	QSplitter *splitter = new QSplitter(Qt::Horizontal, this);

	uinsTreeWidget = new QTreeWidget(splitter);
	QFontMetrics fm(uinsTreeWidget->font());
	uinsTreeWidget->setMinimumWidth(fm.width("W") * 20);
	uinsTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(uinsTreeWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showPopupMenu(const QPoint &)));

	QStringList uinsLabels;
	uinsLabels << tr("Uins");
	uinsTreeWidget->setHeaderLabels(uinsLabels);

	QAction *clearAction = new QAction(icons_manager->loadIcon("ClearHistory"), tr("Clear history"), this);
	connect(clearAction, SIGNAL(triggered(bool)), this, SLOT(clearHistory(bool)));
	QAction *rebuildAction = new QAction(icons_manager->loadIcon("History"), tr("Rebuild index"), this);
	connect(rebuildAction, SIGNAL(triggered(bool)), this, SLOT(rebuildIndex(bool)));

	popupMenu = new QMenu(uinsTreeWidget);
	popupMenu->addAction(clearAction);
	popupMenu->addAction(rebuildAction);

	QWidget *rightWidget = new QWidget(splitter);
	QVBoxLayout* rightLatout = new QVBoxLayout(rightWidget);
	body = new ChatMessagesView(rightWidget);
	body->setPrune(0);
	body->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	connect(body, SIGNAL(loadFinished(bool)), this, SLOT(pageLoaded(bool)));

	QCheckBox *showStatusChanges = new QCheckBox(tr("Show status changes"), rightWidget);
	ShowStatus = config_file.readBoolEntry("History", "ShowStatusChanges");
	showStatusChanges->setDisabled(!config_file.readBoolEntry("History", "SaveStatusChanges"));
	showStatusChanges->setChecked(ShowStatus);

	connect(showStatusChanges, SIGNAL(toggled(bool)), this, SLOT(showStatusChangesSlot(bool)));
	connect(showStatusChanges, SIGNAL(toggled(bool)), this, SIGNAL(showStatusChanges(bool)));

	QWidget *buttonWidget = new QWidget(rightWidget);
	QHBoxLayout* buttonLatout = new QHBoxLayout(buttonWidget);
	buttonLatout->setSpacing(5);
	QPushButton *searchButton = new QPushButton(tr("&Find"), rightWidget);
	QPushButton *searchNextButton = new QPushButton(tr("Find &next"), rightWidget);
	QPushButton *searchPrevButton = new QPushButton(tr("Find &previous"), rightWidget);

	buttonLatout->addWidget(searchButton);
	buttonLatout->addWidget(searchNextButton);
	buttonLatout->addWidget(searchPrevButton);

	rightLatout->addWidget(body);
	rightLatout->addWidget(showStatusChanges);
	rightLatout->addWidget(buttonWidget);

	QList<int> sizes;
	sizes.append(1);
	sizes.append(3);
	splitter->setSizes(sizes);
	grid->addMultiCellWidget(splitter, 0, 1, 0, 4);

	connect(uinsTreeWidget, SIGNAL(itemExpanded(QTreeWidgetItem *)), this, SLOT(uinsChanged(QTreeWidgetItem *)));
	connect(uinsTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(dateChanged(QTreeWidgetItem *)));
	connect(searchButton, SIGNAL(clicked()), this, SLOT(searchButtonClicked()));
	connect(searchNextButton, SIGNAL(clicked()), this, SLOT(searchNextButtonClicked()));
	connect(searchPrevButton, SIGNAL(clicked()), this, SLOT(searchPrevButtonClicked()));

	loadWindowGeometry(this, "History", "HistoryGeometry", 0, 50, 500, 400);

	findRec.type = 1;
	findRec.reverse = 0;
	findRec.actualrecord = -1;

	UinsListViewText *uinsItem, *selectedUinsItem = 0;

	QList<UinsList> uinsEntries = history->getUinsLists();

	foreach(const UinsList &uinsEntry, uinsEntries)
	{
		uinsItem = new UinsListViewText(uinsTreeWidget, uinsEntry);
		uinsItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
		if (uinsEntry.equals(uins) && !uins.isEmpty())
			selectedUinsItem = uinsItem;
	}
	uinsTreeWidget->sortItems(0, Qt::AscendingOrder);

	if (selectedUinsItem)
	{
		uinsChanged(selectedUinsItem);
		if (selectedUinsItem->childCount())
		{
			QTreeWidgetItem *dateItem;
			selectedUinsItem->setExpanded(true);
			dateItem = selectedUinsItem->child(selectedUinsItem->childCount() - 1);
			uinsTreeWidget->setCurrentItem(dateItem);
			dateItem->setSelected(true);
			dateChanged(dateItem);
		}
	}

	kdebugf2();
}

void HistoryDialog::clearHistory(bool)
{
	QTreeWidgetItem *item = uinsTreeWidget->currentItem();
	UinsList uins = dynamic_cast<UinsListViewText *>(item)->getUinsList();
	history->removeHistory(uins);
}

void HistoryDialog::rebuildIndex(bool)
{
	QTreeWidgetItem *item = uinsTreeWidget->currentItem();
	UinsList uins = dynamic_cast<UinsListViewText *>(item)->getUinsList();
	history->buildIndex(uins);
	// remove message dates too
	QFile messageDatesFile(ggPath("history/") + history->getFileNameByUinsList(uins) + ".message_dates");
	messageDatesFile.remove();
}

void HistoryDialog::showPopupMenu(const QPoint &pos)
{
	QTreeWidgetItem *item = uinsTreeWidget->itemAt(pos);
	if (item->parent() == NULL) /* only for top level items */
		popupMenu->exec(uinsTreeWidget->mapToGlobal(pos));
}

void HistoryDialog::showStatusChangesSlot(bool showStatusChanges)
{
	config_file.writeEntry("History", "ShowStatusChanges", showStatusChanges);
	ShowStatus = showStatusChanges;

	if (uinsTreeWidget->currentItem())
		dateChanged(uinsTreeWidget->currentItem());
}

void HistoryDialog::uinsChanged(QTreeWidgetItem *item)
{
	kdebugf();

	uins = dynamic_cast<UinsListViewText *>(item)->getUinsList();

	if (!item->childCount())
	{
		QList<HistoryDate> dateEntries = history->getHistoryDates(uins);
		QList<QDate> messageDates = history->getMessageDates(uins);

		foreach(const HistoryDate &dateEntry, dateEntries)
		{
			DateListViewText *dlvt = new DateListViewText(item, uins, dateEntry, messageDates);
			connect(this, SIGNAL(showStatusChanges(bool)), dlvt, SLOT(showStatusChanges(bool)));
			dlvt->showStatusChanges(ShowStatus);
		}
	}
	kdebugf2();
}

void HistoryDialog::dateChanged(QTreeWidgetItem *item)
{
	kdebugf();
	quint64 count, idx;
	if (!item)
	{
		body->clearMessages();
		return;
	}	
	if (dynamic_cast<UinsListViewText *>(item))
	{
		uinsChanged(item);
		start = 0;
		if (item->child(0))
			item = item->child(1);
		else
			item = item->child(0);
	}
	else
	{
		uinsChanged(item->parent());
		start = ((DateListViewText *)item)->getDate().idx;
		int index = item->parent()->indexOfChild(item);
		if (index != item->parent()->childCount() - 1)
			item = item->parent()->child(index + 1);
		else
			item = 0; 
	}
	if (item)
		count = ((DateListViewText *)item)->getDate().idx - start;
	else
		count = history->getHistoryEntriesCount(uins) - start;

	showHistoryEntries(start, count);

	kdebugf2();
}

ChatMessage * HistoryDialog::createChatMessage(const HistoryEntry &entry)
{
	QString messageText = "hmm";
	if (entry.type == HISTORYMANAGER_ENTRY_SMSSEND)
		messageText = entry.mobile + " SMS :: " + entry.message;
	else if (entry.type & HISTORYMANAGER_ENTRY_STATUS)
	{
		switch (entry.status)
		{
			case GG_STATUS_AVAIL:
			case GG_STATUS_AVAIL_DESCR:
				messageText = tr("Online");
				break;
			case GG_STATUS_BUSY:
			case GG_STATUS_BUSY_DESCR:
				messageText = tr("Busy");
				break;
			case GG_STATUS_INVISIBLE:
			case GG_STATUS_INVISIBLE_DESCR:
				messageText = tr("Invisible");
				break;
			case GG_STATUS_NOT_AVAIL:
			case GG_STATUS_NOT_AVAIL_DESCR:
				messageText = tr("Offline");
				break;
			default:
				messageText = tr("Unknown");
		}
		if (!entry.description.isEmpty())
			messageText.append(QString(" (") + entry.description + ")");
		messageText.append(QString(" ip=") + entry.ip);
	}
	else
		messageText = entry.message;

	bool isMyMessage = entry.type & (HISTORYMANAGER_ENTRY_CHATSEND | HISTORYMANAGER_ENTRY_MSGSEND | HISTORYMANAGER_ENTRY_SMSSEND);

	UserListElement sender = userlist->byID("Gadu", QString::number(entry.uin));
	if (isMyMessage)
		return new ChatMessage(kadu->myself(), UserListElements(sender), messageText, TypeSent, entry.date, entry.sdate);
	else
		return new ChatMessage(userlist->byID("Gadu", QString::number(entry.uin)), UserListElements(kadu->myself()),messageText, TypeReceived, entry.date, entry.sdate);
}

void HistoryDialog::showHistoryEntries(int from, int count)
{
	kdebugf();

	bool noStatus = !config_file.readBoolEntry("History", "ShowStatusChanges");
	QList<HistoryEntry> entries = history->getHistoryEntries(uins, from, count);
	QList<ChatMessage *> chatMessages;

	body->removeMessages();

	QList<HistoryEntry>::const_iterator entry = entries.constBegin();
	QList<HistoryEntry>::const_iterator lastEntry = entries.constEnd();
	for(; entry != lastEntry; ++entry)
		if (((*entry).type != HISTORYMANAGER_ENTRY_STATUS) || !noStatus)
			chatMessages.append(createChatMessage(*entry));

	body->appendMessages(chatMessages);
}

void HistoryDialog::pageLoaded(bool b)
{
	/* Dorr: highlight and navigate to the word found */
	static QWebPage::FindFlags flag = 0;
	if (showResults)
	{
		if (findRec.reverse) 
			flag = QWebPage::FindBackward;

		if (body->findText(findRec.data, flag))
			showResults = false;
	}
}

void HistoryDialog::searchButtonClicked()
{
	kdebugf();

	HistorySearchDialog* hs = new HistorySearchDialog(this, uins);
//	hs->resetBtnClicked();
	hs->setDialogValues(findRec);
	if (hs->exec() == QDialog::Accepted)
	{
		findRec = hs->getDialogValues();
		findRec.actualrecord = -1;
		searchHistory();
	}
	delete hs;
	kdebugf2();
}

void HistoryDialog::searchNextButtonClicked()
{
	kdebugf();
	findRec.reverse = false;
	searchHistory();
	kdebugf2();
}

void HistoryDialog::searchPrevButtonClicked()
{
	kdebugf();
	findRec.reverse = true;
	searchHistory();
	kdebugf2();
}

const QString &HistoryDialog::gaduStatus2symbol(unsigned int status)
{
	static const QString sym[] = {QString("avail"), QString("busy"), QString("invisible"), QString("notavail")};
	switch (status)
	{
		case GG_STATUS_AVAIL:
		case GG_STATUS_AVAIL_DESCR:
			return sym[0];
		case GG_STATUS_BUSY:
		case GG_STATUS_BUSY_DESCR:
			return sym[1];
		case GG_STATUS_INVISIBLE:
		case GG_STATUS_INVISIBLE_DESCR:
			return sym[2];
		default:
			return sym[3];
	}
}

void HistoryDialog::setDateListViewText(const QDateTime &datetime)
{
	kdebugf();
	QTreeWidgetItem *actlvi;

	for (int i = 0; i < uinsTreeWidget->topLevelItemCount(); i++)
	{
		actlvi = uinsTreeWidget->topLevelItem(i);
		if (dynamic_cast<UinsListViewText *>(actlvi)->getUinsList().equals(uins))
			break;
	}
 	if (actlvi)
 	{
		QTreeWidgetItem *uinItem = actlvi;

		uinsTreeWidget->expandItem(uinItem);

		for (int i = 0; i < uinItem->childCount(); i++)
		{
			actlvi = uinItem->child(i);
			if (dynamic_cast<DateListViewText *>(actlvi)->getDate().date.date() == datetime.date())
				break;
		}
		if (actlvi)
		{
			uinsTreeWidget->setCurrentItem(actlvi);
			actlvi->setSelected(true);
		}
	}
	kdebugf2();
}

void HistoryDialog::searchHistory()
{
	kdebugf();
	int start, end, count, total, len, force = 0;
	unsigned int i;
	QDateTime fromdate, todate;
	QList<HistoryEntry> entries;
	unsigned int entriesCount;
	QRegExp rxp;

	/* Dorr: if we're in the middle of searching */
	if (findRec.actualrecord > 0)
	{
		QWebPage::FindFlags flag = 0;
		if (findRec.reverse)
			flag = QWebPage::FindBackward;
	
		/* try to find the text on the current page */
		if (body->findText(findRec.data, flag))
		{
			return;
		}
		/* if not found force searching on the next date */
		else
		{
			QTreeWidgetItem *actlvi = uinsTreeWidget->currentItem();
			if (actlvi && actlvi->parent())
			{
				int index = actlvi->parent()->indexOfChild(actlvi);
				if (findRec.reverse)
					--index;
				else
					++index;
				if ((index > 0) && (index < actlvi->parent()->childCount()))
				{
					actlvi = actlvi->parent()->child(index);
					force = history->getHistoryEntryIndexByDate(uins, dynamic_cast<DateListViewText *>(actlvi)->getDate().date);
				}
			}
		}
	}
	else
	{
		/* open the first date */
		QTreeWidgetItem *actlvi = uinsTreeWidget->currentItem();
		if (actlvi && !actlvi->parent())
			if (actlvi->childCount() > 0)
				uinsTreeWidget->setCurrentItem(actlvi->child(0));
	}

	count = history->getHistoryEntriesCount(uins);
	if (findRec.fromdate.isNull())
		start = 0;
	else
		start = history->getHistoryEntryIndexByDate(uins, findRec.fromdate);
	if (findRec.todate.isNull())
		end = count - 1;
	else
		end = history->getHistoryEntryIndexByDate(uins, findRec.todate, true);
	kdebugmf(KDEBUG_INFO, "start = %d, end = %d\n", start, end);
	if (start > end || (start == end && (start == -1 || start == count)))
		return;
	if (start == -1)
		start = 0;
	if (end == count)
		--end;
	entries = history->getHistoryEntries(uins, start, 1);
	fromdate = entries[0].date;
	entries = history->getHistoryEntries(uins, end, 1);
	todate = entries[0].date;
	kdebugmf(KDEBUG_INFO, "start = %s, end = %s\n",
		fromdate.toString("dd.MM.yyyy hh:mm:ss").latin1(),
		todate.toString("dd.MM.yyyy hh:mm:ss").latin1());
	if (force)
		findRec.actualrecord = force;
	else if (findRec.actualrecord == -1)
		findRec.actualrecord = findRec.reverse ? end : start;
	if ((findRec.actualrecord >= end && !findRec.reverse)
		|| (findRec.actualrecord <= start && findRec.reverse))
		return;
	if (findRec.reverse)
		total = findRec.actualrecord - start + 1;
	else
		total = end - findRec.actualrecord + 1;
	kdebugmf(KDEBUG_INFO, "findRec.type = %d\n", findRec.type);
	rxp.setPattern(findRec.data);
	setEnabled(false);
	showResults = true;
	finding = true;
	if (findRec.reverse)
		do
		{
			len = total > 1000 ? 1000 : total;
			entries = history->getHistoryEntries(uins, findRec.actualrecord - len + 1, len);

			entriesCount = entries.count();
			//ehh, szkoda, �e w Qt nie ma reverse iterator�w...
			QList<HistoryEntry>::const_iterator entry = entries.end() - 1;
			QList<HistoryEntry>::const_iterator firstEntry = entries.begin();
			bool end;
			i = 0;
			do
			{
				if ((findRec.type == 1 &&
					((*entry).type & HISTORYMANAGER_ENTRY_ALL_MSGS)
					&& (*entry).message.contains(rxp)) ||
					(findRec.type == 2 &&
					((*entry).type & HISTORYMANAGER_ENTRY_STATUS)
					&& findRec.data == gaduStatus2symbol((*entry).status)))
				{
					setDateListViewText((*entry).date);
					//showHistoryEntries(findRec.actualrecord - i,
					//	findRec.actualrecord - i + 99 < count ? 100
					//	: count - findRec.actualrecord + i);
					HistoryDialog::start = findRec.actualrecord - i;
					break;
				}
				end = entry == firstEntry;
				if (!end)
					--entry;
				++i;
			} while (!end);
			findRec.actualrecord -= i + (i < entriesCount);
			total -= i + (i < entriesCount);
			kdebugmf(KDEBUG_INFO, "actualrecord = %d, i = %d, total = %d\n",
				findRec.actualrecord, i, total);
			qApp->processEvents();
		} while (total > 0 && i == entriesCount && !closeDemand);
	else
		do
		{
			len = total > 1000 ? 1000 : total;
			entries = history->getHistoryEntries(uins, findRec.actualrecord, len);
			entriesCount = entries.count();
			i = 0;
			foreach(const HistoryEntry &entry, entries)
			{
				if ((findRec.type == 1 && (entry.type & HISTORYMANAGER_ENTRY_ALL_MSGS)
					&& entry.message.contains(rxp)) ||
					(findRec.type == 2 &&
					(entry.type & HISTORYMANAGER_ENTRY_STATUS) &&
					findRec.data == gaduStatus2symbol(entry.status)))
				{
					setDateListViewText(entry.date);
					//showHistoryEntries(findRec.actualrecord + i,
					//	findRec.actualrecord + 99 < count ? 100
					//	: count - findRec.actualrecord - i);
					HistoryDialog::start = findRec.actualrecord + i;
					break;
				}
				++i;
			}
			findRec.actualrecord += i + (i < entriesCount);
			total -= i + (i < entriesCount);
			kdebugmf(KDEBUG_INFO, "actualrecord = %d, i = %d, total = %d\n",
				findRec.actualrecord, i, total);
			qApp->processEvents();
		} while (total > 0 && i == entriesCount && !closeDemand);
	if (closeDemand)
	{
		close();
		kdebugf2();
		return;
	}
	if (findRec.actualrecord < 0)
		findRec.actualrecord = 0;
	setEnabled(true);
	finding = false;
	kdebugf2();
}

void HistoryDialog::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		e->accept();
		close();
	}
	else if (e == QKeySequence::Copy)
	{
		body->pageAction(QWebPage::Copy)->trigger();
	}
	else
		QWidget::keyPressEvent(e);
}

void HistoryDialog::closeEvent(QCloseEvent *e)
{
	saveWindowGeometry(this, "History", "HistoryGeometry");

	if (finding)
	{
		e->ignore();
		closeDemand = true;
	}
	else
		e->accept();
}
