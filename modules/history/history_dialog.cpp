/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QApplication>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qregexp.h>
#include <qsplitter.h>

#include <QCloseEvent>
#include <QGridLayout>
#include <QList>
#include <QKeyEvent>

#include "chat_message.h"
#include "chat_messages_view.h"
#include "config_file.h"
#include "debug.h"
#include "emoticons.h"
#include "gadu_images_manager.h"
#include "kadu.h"
#include "misc.h"

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
		CONST_FOREACH(uin, uins)
		{
			if (userlist->contains("Gadu", QString::number(*uin)))
				name.append(userlist->byID("Gadu", QString::number(*uin)).altNick());
			else
				name.append(QString::number(*uin));
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

DateListViewText::DateListViewText(QTreeWidgetItem *parent, const HistoryDate &date)
	: QTreeWidgetItem(parent, 0), date(date)
{
	setText(0, date.date.toString("yyyy.MM.dd"));
}

const HistoryDate &DateListViewText::getDate() const
{
	return date;
}

HistoryDialog::HistoryDialog(UinsList uins)
	: QWidget(kadu, "HistoryDialog", Qt::WType_TopLevel | Qt::WDestructiveClose), uinslv(0), body(0),
	uins(uins), start(0), findrec(), closeDemand(false), finding(false), dateentries()
{
	kdebugf();
	history->convHist2ekgForm(uins);
	history->buildIndex(uins);

	setCaption(tr("History"));

	QGridLayout *grid = new QGridLayout(this, 2, 5, 5, 5, "grid");

	QSplitter *splitter = new QSplitter(Qt::Horizontal, this, "splitter");

	uinslv = new QTreeWidget(splitter);
	QStringList uinslvLabels;
	uinslvLabels << tr("Uins");
	uinslv->setHeaderLabels(uinslvLabels);
	uinslv->setRootIsDecorated(TRUE);

	QWidget *vbox = new QWidget(splitter);
	QVBoxLayout* vbox_lay = new QVBoxLayout;
	body = new ChatMessagesView;
	body->setPrune(0);
	vbox_lay->addWidget(body);

	QCheckBox *showStatusChanges = new QCheckBox(tr("Show status changes"), this);
	vbox_lay->addWidget(showStatusChanges);
	showStatusChanges->setDisabled(config_file.readBoolEntry("History", "DontSaveStatusChanges"));
	showStatusChanges->setChecked(!config_file.readBoolEntry("History", "DontShowStatusChanges"));
	connect(showStatusChanges, SIGNAL(toggled(bool)), this, SLOT(showStatusChanged(bool)));

	QWidget *btnbox = new QWidget;
	QHBoxLayout* btnbox_lay = new QHBoxLayout;
	btnbox_lay->setSpacing(5);
	QPushButton *searchbtn = new QPushButton(tr("&Find"), this, "searchbtn");
	QPushButton *searchnextbtn = new QPushButton(tr("Find &next"), this, "searcgnextbtn");
	QPushButton *searchprevbtn = new QPushButton(tr("Find &previous"), this, "searchprevbtn");
	btnbox_lay->addWidget(searchbtn);
	btnbox_lay->addWidget(searchnextbtn);
	btnbox_lay->addWidget(searchprevbtn);
	btnbox->setLayout(btnbox_lay);
	QList<int> sizes;
	sizes.append(1);
	sizes.append(3);
	splitter->setSizes(sizes);
	vbox_lay->addWidget(btnbox);
	vbox->setLayout(vbox_lay);
	grid->addMultiCellWidget(splitter, 0, 1, 0, 4);

// 	connect(uinslv, SIGNAL(itemExpanded(QTreeWidgetItem *, int)), this, SLOT(uinsChanged(QTreeWidgetItem *, int)));
	connect(uinslv, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(dateChanged(QTreeWidgetItem *, int)));
	connect(searchbtn, SIGNAL(clicked()), this, SLOT(searchBtnClicked()));
	connect(searchnextbtn, SIGNAL(clicked()), this, SLOT(searchNextBtnClicked()));
	connect(searchprevbtn, SIGNAL(clicked()), this, SLOT(searchPrevBtnClicked()));

	loadGeometry(this, "History", "HistoryGeometry", 0, 30, 500, 400);

	findrec.type = 1;
	findrec.reverse = 0;
	findrec.actualrecord = -1;

	UinsListViewText *uinslvt, *selecteduinslvt = NULL;
	QTreeWidgetItem *datelvt;

	QList<UinsList> uinsentries = history->getUinsLists();

	CONST_FOREACH(uinsentry, uinsentries)
	{
		uinslvt = new UinsListViewText(uinslv, *uinsentry);

			QList<HistoryDate> dateentries;
			dateentries = history->getHistoryDates(*uinsentry);
			if(!dateentries.isEmpty())
			CONST_FOREACH(dateentry, dateentries)
				(new DateListViewText(uinslvt, *dateentry));


		///uinslvt->setExpandable(TRUE);
/// 		if ((*uinsentry).equals(uins) && !uins.isEmpty())
/// 			selecteduinslvt = uinslvt;
	}

	uinslv->sortItems(0, Qt::AscendingOrder);
/**	if (selecteduinslvt)
	{
		selecteduinslvt->setSelected(true);
		datelvt = selecteduinslvt->firstChild();
		if (datelvt)
		{
			while (datelvt->nextSibling())
				datelvt = datelvt->nextSibling();
			uinslv->setCurrentItem(datelvt);
			uinslv->setSelected(datelvt, TRUE);
		}
	}*/
	kdebugf2();
}

void HistoryDialog::showStatusChanged(bool showStatusChanges)
{
	config_file.writeEntry("History", "DontShowStatusChanges", !showStatusChanges);

	if (uinslv->currentItem())
		dateChanged(uinslv->currentItem(), 0);
}

void HistoryDialog::uinsChanged(QTreeWidgetItem *item, int col)
{
	kdebugf();
// 	QList<HistoryDate> dateentries;
// 	uins = ((UinsListViewText *)item)->getUinsList();
// 	if (uins.isEmpty())
// 	{
// 		if (!item->childCount())
// 		{
// 			dateentries = history->getHistoryDates(uins);
// /// 			CONST_FOREACH(dateentry, dateentries)
// /// 				(new DateListViewText(item, *dateentry))->setExpandable(FALSE);
// 		}
// 	}
	kdebugf2();
}

void HistoryDialog::dateChanged(QTreeWidgetItem *item, int col)
{
	kdebugf();
	int count;/*, depth = item->depth();
	switch (depth)
	{
		case 1:
			uinsChanged(item->parent());
			start = ((DateListViewText *)item)->getDate().idx;
			item = item->nextSibling();
			break;
		case 0:
			uinsChanged(item);
			start = 0;
			item = item->firstChild();
			if (item)
				item = item->nextSibling();
			break;
	}

	if (depth < 2)
	{*/

	if (item->childCount())
		uins = ((UinsListViewText *)item)->getUinsList();
	else
	{	
		QTreeWidgetItem *it = item->parent();
		uins = ((UinsListViewText *)it)->getUinsList();
	}

	if(!uins.isEmpty())
	{
		if (item)
			count = ((DateListViewText *)item)->getDate().idx - start;
		else
			count = history->getHistoryEntriesCount(uins) - start;
		showHistoryEntries(start, count);
 	}
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

	UserListElement sender;
	if (isMyMessage)
		return new ChatMessage(kadu->myself(), messageText, TypeSent, entry.date, entry.sdate);
	else
		return new ChatMessage(userlist->byID("Gadu", QString::number(entry.uin)), messageText, TypeReceived, entry.date, entry.sdate);
}

void HistoryDialog::showHistoryEntries(int from, int count)
{
	kdebugf();

	bool noStatus = config_file.readBoolEntry("History", "DontShowStatusChanges");
	QList<HistoryEntry> entries = history->getHistoryEntries(uins, from, count);
	QList<ChatMessage *> chatMessages;

	body->clearMessages();

	QList<HistoryEntry>::const_iterator entry = entries.constBegin();
	QList<HistoryEntry>::const_iterator lastEntry = entries.constEnd();
	for(; entry != lastEntry; ++entry)
		if (!((*entry).type & HISTORYMANAGER_ENTRY_STATUS) || !noStatus)
			chatMessages.append(createChatMessage(*entry));

	body->appendMessages(chatMessages);
}

void HistoryDialog::searchBtnClicked()
{
	kdebugf();

	HistorySearchDialog* hs = new HistorySearchDialog(this, uins);
//	hs->resetBtnClicked();
	hs->setDialogValues(findrec);
	if (hs->exec() == QDialog::Accepted)
	{
		findrec = hs->getDialogValues();
		findrec.actualrecord = -1;
		searchHistory();
	}
	delete hs;
	kdebugf2();
}

void HistoryDialog::searchNextBtnClicked()
{
	kdebugf();
	findrec.reverse = false;
	searchHistory();
	kdebugf2();
}

void HistoryDialog::searchPrevBtnClicked()
{
	kdebugf();
	findrec.reverse = true;
	searchHistory();
	kdebugf2();
}

const QString &HistoryDialog::gaduStatus2symbol(unsigned int status)
{
	static const QString sym[]={QString("avail"), QString("busy"), QString("invisible"), QString("notavail")};
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
	int y;
	QTreeWidgetItem *actlvi;
	actlvi = uinslv->topLevelItem(y);
	for (int i = 1; actlvi && !((UinsListViewText *)actlvi)->getUinsList().equals(uins) && i < actlvi->childCount(); i++)
		actlvi = actlvi->child(i);
	if (actlvi)
	{
		uinslv->expandItem(actlvi);
		actlvi = actlvi->child(1);
		for (int i = 1; actlvi && ((DateListViewText *)actlvi)->getDate().date.date() != datetime.date()&& i < actlvi->childCount(); i++)
			actlvi = actlvi->child(i);
		if (actlvi)
		{
			uinslv->setCurrentItem(actlvi);
// 			body->setSelection(0, 0, 1, 10);
		}
	}
	kdebugf2();
}

void HistoryDialog::searchHistory()
{
	kdebugf();
	int start, end, count, total, len;
	unsigned int i;
	QDateTime fromdate, todate;
	QList<HistoryEntry> entries;
	unsigned int entriesCount;
	QRegExp rxp;

	count = history->getHistoryEntriesCount(uins);
	if (findrec.fromdate.isNull())
		start = 0;
	else
		start = history->getHistoryEntryIndexByDate(uins, findrec.fromdate);
	if (findrec.todate.isNull())
		end = count - 1;
	else
		end = history->getHistoryEntryIndexByDate(uins, findrec.todate, true);
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
	if (findrec.actualrecord == -1)
		findrec.actualrecord = findrec.reverse ? end : start;
	if ((findrec.actualrecord >= end && !findrec.reverse)
		|| (findrec.actualrecord <= start && findrec.reverse))
		return;
	if (findrec.reverse)
		total = findrec.actualrecord - start + 1;
	else
		total = end - findrec.actualrecord + 1;
	kdebugmf(KDEBUG_INFO, "findrec.type = %d\n", findrec.type);
	rxp.setPattern(findrec.data);
	setEnabled(false);
	finding = true;
	if (findrec.reverse)
		do
		{
			len = total > 1000 ? 1000 : total;
			entries = history->getHistoryEntries(uins, findrec.actualrecord - len + 1, len);
			entriesCount = entries.count();
			//ehh, szkoda, ¿e w Qt nie ma reverse iteratorów...
			QList<HistoryEntry>::const_iterator entry = entries.end();
			QList<HistoryEntry>::const_iterator firstEntry = entries.begin();
			bool end;
			i = 0;
			do
			{
				if ((findrec.type == 1 &&
					((*entry).type & HISTORYMANAGER_ENTRY_ALL_MSGS)
					&& (*entry).message.contains(rxp)) ||
					(findrec.type == 2 &&
					((*entry).type & HISTORYMANAGER_ENTRY_STATUS)
					&& findrec.data == gaduStatus2symbol((*entry).status)))
				{
					setDateListViewText((*entry).date);
					//showHistoryEntries(findrec.actualrecord - i,
					//	findrec.actualrecord - i + 99 < count ? 100
					//	: count - findrec.actualrecord + i);
					HistoryDialog::start = findrec.actualrecord - i;
					break;
				}
				end = entry == firstEntry;
				if (!end)
					--entry;
				++i;
			}while (!end);
			findrec.actualrecord -= i + (i < entriesCount);
			total -= i + (i < entriesCount);
			kdebugmf(KDEBUG_INFO, "actualrecord = %d, i = %d, total = %d\n",
				findrec.actualrecord, i, total);
			qApp->processEvents();
		} while (total > 0 && i == entriesCount && !closeDemand);
	else
		do
		{
			len = total > 1000 ? 1000 : total;
			entries = history->getHistoryEntries(uins, findrec.actualrecord, len);
			entriesCount = entries.count();
			i = 0;
			CONST_FOREACH(entry, entries)
			{
				if ((findrec.type == 1 && ((*entry).type & HISTORYMANAGER_ENTRY_ALL_MSGS)
					&& (*entry).message.contains(rxp)) ||
					(findrec.type == 2 &&
					((*entry).type & HISTORYMANAGER_ENTRY_STATUS) &&
					findrec.data == gaduStatus2symbol((*entry).status)))
				{
					setDateListViewText((*entry).date);
					//showHistoryEntries(findrec.actualrecord + i,
					//	findrec.actualrecord + 99 < count ? 100
					//	: count - findrec.actualrecord - i);
					HistoryDialog::start = findrec.actualrecord + i;
					break;
				}
				++i;
			}
			findrec.actualrecord += i + (i < entriesCount);
			total -= i + (i < entriesCount);
			kdebugmf(KDEBUG_INFO, "actualrecord = %d, i = %d, total = %d\n",
				findrec.actualrecord, i, total);
			qApp->processEvents();
		} while (total > 0 && i == entriesCount && !closeDemand);
	if (closeDemand)
	{
		close();
		kdebugf2();
		return;
	}
	if (findrec.actualrecord < 0)
		findrec.actualrecord = 0;
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
	else
		QWidget::keyPressEvent(e);
}

void HistoryDialog::closeEvent(QCloseEvent *e)
{
//	saveGeometry(this, "History", "HistoryGeometry");

	if (finding)
	{
		e->ignore();
		closeDemand = true;
	}
	else
		e->accept();
}
