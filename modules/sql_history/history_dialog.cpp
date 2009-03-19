/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QLayout>
#include <QtGui/QTreeWidget>
#include <QtGui/QPushButton>
#include <QtGui/qsplitter.h>
#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlquery.h>
#include <QtGui/qstatusbar.h>
#include <QtGui/QCloseEvent>
#include <QtGui/QDockWidget>
#include <QtGui/QKeyEvent>
#include <QtGui/QKeySequence>
#include <QtGui/QGridLayout>
#include <QtCore/QList>
#include <QtGui/QMenu>
#include <QtGui/QProgressBar>
#include <QtGui/QRadioButton>

#include "action.h"
#include "toolbar.h"
#include "config_file.h"
#include "chat_manager.h"
#include "debug.h"
#include "emoticons.h"
#include "message_box.h"
#include "sql_history.h"
#include "icons_manager.h"
#include "kadu.h"
#include "misc/misc.h"
#include "search.h"

#include "history_dialog.h"

MainListViewText::MainListViewText(QTreeWidget* parent, const UserListElements& uids)
	: QTreeWidgetItem(parent), Uids(uids)
{
	prepareText();	
}

MainListViewText::MainListViewText(QTreeWidgetItem* parent, const UserListElements& uids)
	: QTreeWidgetItem(parent), Uids(uids)
{
	prepareText();
}

void MainListViewText::prepareText()
{
	QString name;
	unsigned int i = 0;
	unsigned int count = Uids.count();
	foreach(UserListElement uid, Uids)
	{
		QString proto = (*uid.protocolList().begin());
		if (userlist->contains(proto, uid.ID(proto)))
			name.append(userlist->byID(proto, uid.ID(proto)).altNick());
		else
			name.append(uid.ID(proto));
		if (i++ < count - 1)
			name.append(", ");
	}
	setText(0, name);
}
 
DetailsListViewItem::DetailsListViewItem(QTreeWidget* parent, QString contact, QString title, QDate date, QString length, const UserListElements& uids)
	: QTreeWidgetItem(parent), Date(date), Uids(uids)
{
	setText(0, contact);
	setText(1, title);
	setText(2, date.toString("dd.MM.yyyy"));
	setText(3, length);
	setIcon(0, QIcon(icons_manager->loadIcon("WriteEmail")));
}

QDate DetailsListViewItem::date() const
{
	return Date;
}

HistoryMainWidget::HistoryMainWidget(QWidget *parent, QWidget *window)
	: KaduMainWindow(parent)
{
	kdebugf();
	QWidget *mvbox = new QWidget;
	QSplitter* right = new QSplitter(Qt::Vertical);
	QVBoxLayout *mvbox_lay = new QVBoxLayout;
	DetailsListView = new QTreeWidget(right);
	QStringList detailsLabels;
	detailsLabels << tr("Contact") << tr("Title") << tr("Date") << tr("Length");
	DetailsListView->setHeaderLabels(detailsLabels);
	DetailsListView->setRootIsDecorated(TRUE);
	ContentBrowser = new ChatMessagesView(right);
	ContentBrowser->setPrune(0);
	ContentBrowser->setMargin(config_file.readNumEntry("General", "ParagraphSeparator"));

	dock = new QDockWidget(tr("Quick search"), this);
	QWidget *dockWidget = new QWidget;
	QHBoxLayout* dockLayout = new QHBoxLayout;
	dockLayout->setMargin(3);
	dockLayout->setSpacing(3);

	quickSearchPhraseEdit = new QLineEdit(dock);
	connect(quickSearchPhraseEdit, SIGNAL(textChanged(const QString &)), this, SLOT(quickSearchPhraseTyped(const QString &)));
	dockLayout->addWidget(quickSearchPhraseEdit);

	QPushButton *nextButton = new QPushButton(tr("Next"));
	dockLayout->addWidget(nextButton);

	QPushButton *prevButton = new QPushButton(tr("Previous"));
	dockLayout->addWidget(prevButton);

	dockWidget->setLayout(dockLayout);
	dock->setWidget(dockWidget);
	addDockWidget(Qt::BottomDockWidgetArea, dock);
	dock->hide();
	
	historySearchActionDescription = new ActionDescription(
		ActionDescription::TypeHistory, "historySearchAction",
		window, SLOT(searchActionActivated(QAction *, bool)),
		"LookupUserInfo", tr("&Search")
	);

	historyNextResultsActionDescription = new ActionDescription(
		ActionDescription::TypeHistory, "historyNextResultsAction",
		window, SLOT(searchNextActActivated(QAction *, bool)),
		"NextPageHistory", tr("&Next")
	);
	historyPrevResultsActionDescription = new ActionDescription(
		ActionDescription::TypeHistory, "historyPrevResultsAction",
		window, SLOT(searchPrevActActivated(QAction *, bool)),
		"PreviousPageHistory", tr("&Previous")
	);
// 	historyAdvSearchActionDescription = new ActionDescription(
// 		ActionDescription::TypeHistory, "historyAdvSearchAction",
// 		window, SLOT(advSearchActivated(QAction *, bool)),
// 		"Configuration", tr("&Advanced search")
// 	);

///	ToolBar::addDefaultAction("SQL History toolbar", "historySearchAction", 1, true);
///	ToolBar::addDefaultAction("SQL History toolbar", "historyNextResultsAction", 2, true);
///	ToolBar::addDefaultAction("SQL History toolbar", "historyPrevResultsAction", 3, true);
// 	ToolBar::addDefaultAction("SQL History toolbar", "historyAdvSearchAction", 4, true);

// 	loadToolBarsFromConfig("sqlHistoryTopDockArea", Qt::TopToolBarArea);
// 	loadToolBarsFromConfig("sqlHistoryBottomDockArea", Qt::BottomToolBarArea);
// 	loadToolBarsFromConfig("sqlHistoryLeftDockArea", Qt::LeftToolBarArea);
// 	loadToolBarsFromConfig("sqlHistoryRightDockArea", Qt::RightToolBarArea);
	
	mvbox_lay->addWidget(right);
	mvbox_lay->setMargin(0);
	mvbox_lay->setSpacing(0);
	mvbox->setLayout(mvbox_lay);
	setCentralWidget(mvbox);
	loadToolBarsFromConfig("history");
	statusBar()->showMessage(tr("Ready"));
	kdebugf2();
}

HistoryMainWidget::~HistoryMainWidget()
{
	kdebugf();
	writeToolBarsToConfig("history");
	kdebugf2();
}


bool HistoryMainWidget::supportsActionType(ActionDescription::ActionType type)
{
	return (type == ActionDescription::TypeGlobal || type == ActionDescription::TypeChat || type == ActionDescription::TypeHistory);
}

void HistoryMainWidget::quickSearchPhraseTyped(const QString &text)
{
	kdebugf();
	if(!text.isEmpty())
		if(!ContentBrowser->findText(text))
			if(!ContentBrowser->findText(text, QWebPage::FindBackward))
				//zmieniaæ kolor t³a edita? jak?
				MessageBox::msg(tr("Nic ciekawego"), false, "Warning");
	kdebugf2();
}

HistoryDlg::HistoryDlg() : QWidget(NULL), isSearchInProgress(0), closeDemand(0), inSearchMode(0), chatsItem(0)
{
	kdebugf();
	setCaption(tr("History"));
	setIcon(icons_manager->loadIcon("History").pixmap());
	QGridLayout* grid = new QGridLayout(this, 2, 5, 3, 3);
	grid->setSpacing(0);
	QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
	QSplitter* left_splitter = new QSplitter(Qt::Vertical, splitter);
	MainListView = new QTreeWidget(left_splitter);
	QStringList mainLabels;
	mainLabels << "";
	MainListView->setHeaderLabels(mainLabels);
	MainListView->setRootIsDecorated(TRUE);

	QWidget *vbox = new QWidget(splitter);
	QVBoxLayout *vbox_lay = new QVBoxLayout();

	main = new HistoryMainWidget(splitter, this);
	vbox_lay->setMargin(0);
	vbox_lay->setSpacing(0);
	vbox_lay->addWidget(main);

	QList<int> sizes;
	sizes.append(100);
	sizes.append(300);
	splitter->setSizes(sizes);
	vbox->setLayout(vbox_lay);
	grid->addMultiCellWidget(splitter, 0, 1, 0, 4);
	connect(MainListView, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(mainItemChanged(QTreeWidgetItem*, int)));

	MainListView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(MainListView, SIGNAL(customContextMenuRequested(QPoint)),
		this, SLOT(showMainPopupMenu(QPoint)));
	main->getDetailsListView()->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(main->getDetailsListView(), SIGNAL(customContextMenuRequested(QPoint)),
		this, SLOT(showDetailsPopupMenu(QPoint)));
	
	connect(main->getDetailsListView(), SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(detailsItemChanged(QTreeWidgetItem*, int)));
	loadWindowGeometry(this, "History", "HistoryWindowGeometry", 200, 200, 700, 500);
	maxLen = 36;//~	

	MainPopupMenu = new QMenu;
	MainPopupMenu->addAction(icons_manager->loadIcon("OpenChat"), tr("&Open chat"), this, SLOT(openChat()));
	MainPopupMenu->addAction(icons_manager->loadIcon("LookupUserInfo"), tr("&Search in directory"), this, SLOT(lookupUserInfo()));
	MainPopupMenu->addAction(icons_manager->loadIcon("ClearHistory"), tr("&Clear history"), this, SLOT(removeHistoryEntriesPerUser()));
	
	DetailsPopupMenu = new QMenu;
	DetailsPopupMenu->addAction(icons_manager->loadIcon("ClearHistory"), tr("&Remove entries"), this, SLOT(removeHistoryEntriesPerDate()));
	kdebugf2();
}

HistoryDlg::~HistoryDlg()
{
	// for valgrind
	QStringList searchActions;
	searchActions << "historySearchAction" << "historyNextResultsAction" << "historyPrevResultsAction" << "historyAdvSearchAction";
	foreach(QString act, searchActions)
	{
		ActionDescription *a = KaduActions[act];
		delete a;
	}
}

void HistoryDlg::globalRefresh()
{
	kdebugf();
	int anonymousCount = 0;
	bool chatsExpanded, statExpanded, smsExpanded, searchExpanded, conferExpanded, anonChatsExpanded = false;
	if(chatsItem)
	{
		if(chatsItem->isExpanded())
			chatsExpanded = true;
		if(anonChatsItem->isExpanded())
			anonChatsExpanded = true;
		if(statusItem->isExpanded())
			statExpanded = true;
		if(smsItem->isExpanded())
			smsExpanded = true;
		if(conferItem->isExpanded())
			conferExpanded = true;
		if(searchItem->isExpanded())
			searchExpanded = true;
	}
	MainListView->clear();
	chatsItem = new QTreeWidgetItem(MainListView, QStringList(tr("Chats")));
	chatsItem->setExpanded(false);
	///QTreeWidgetItem *ft = new QTreeWidgetItem(MainListView, QStringList(tr("File transfers")));
	conferItem = new QTreeWidgetItem(MainListView, QStringList(tr("Conferences")));
	smsItem = new QTreeWidgetItem(MainListView, QStringList(tr("SMS")));
	statusItem = new QTreeWidgetItem(MainListView, QStringList(tr("Status")));
	statusItem->setExpanded(false);
	searchItem = new QTreeWidgetItem(MainListView, QStringList(tr("Search")));

	chatsItem->setIcon(0, QIcon(icons_manager->loadIcon("OpenChat")));
	///ft->setIcon(0, QIcon(icons_manager->loadIcon("SendFile")));
	smsItem->setIcon(0, QIcon(icons_manager->loadIcon("Mobile")));
	conferItem->setIcon(0, QIcon(icons_manager->loadIcon("ManageModules")));
	statusItem->setIcon(0, QIcon(icons_manager->loadIcon("Busy")));
	searchItem->setIcon(0, QIcon(icons_manager->loadIcon("LookupUserInfo")));

	anonChatsItem = new QTreeWidgetItem(chatsItem, QStringList(tr("Anonymous")));
	anonChatsItem->setIcon(0, QIcon(icons_manager->loadIcon("PersonalInfo")));
	//mo¿e byæ status od anonima??
	//anonStatusItem = new QTreeWidgetItem(statusItem, QStringList(tr("Anonymous")));
	//anonStatusItem->setIcon(0, QIcon(icons_manager->loadIcon("PersonalInfo")));

	QList<UserListElements> chatUidGroups = sql_history->getChatUidGroups();
	foreach(UserListElements uid_group, chatUidGroups)
	{
		if (uid_group.count() == 1) //zwyk³y chat
		{
			MainListViewText* mainItem;
			if(!(*uid_group.begin()).isAnonymous())
				mainItem = new MainListViewText(chatsItem, uid_group);
			else
			{
				mainItem = new MainListViewText(anonChatsItem, uid_group);
				++anonymousCount;
			}
			//je¶li w userboksie zaznaczono kontakty, rozwin±æ ich pozycjê i wy¶wietliæ historiê
			if(selectedUsers.count() && uid_group.contains((*(*selectedUsers.begin()).protocolList().begin()), (*selectedUsers.begin()).ID((*(*selectedUsers.begin()).protocolList().begin()))))
			{
				MainListView->expandItem(chatsItem);
				MainListView->setCurrentItem(mainItem);
				mainItemChanged(mainItem, 0);
			}
			mainItem->setIcon(0, icons_manager->loadIcon("Online") );
		}
		else if (uid_group.count() > 1) //konferencja
		{
			MainListViewText *conferenceItem = new MainListViewText(conferItem, uid_group);
			conferenceItem->setIcon(0, QIcon(icons_manager->loadIcon("Profiles")));
		}
	}

	QList<UserListElements> statusUidGroups = sql_history->getStatusUidGroups();
	foreach(UserListElements uid_group, statusUidGroups)
	{
		MainListViewText* statItem = new MainListViewText(statusItem, uid_group);
		statItem->setIcon(0, icons_manager->loadIcon("Online"));
	}

	QList<UserListElements> smsUidGroups = sql_history->getSmsUidGroups();
	foreach(UserListElements uid_group, smsUidGroups)
	{
		MainListViewText* sItem = new MainListViewText(smsItem, uid_group);
		sItem->setIcon(0, icons_manager->loadIcon("Mobile"));
	}
	if(!anonymousCount)
		anonChatsItem->setHidden(true);

	MainListView->sortItems(0, Qt::AscendingOrder);

	if(chatsExpanded)
		chatsItem->setExpanded(true);
	if(anonChatsExpanded)
		anonChatsItem->setExpanded(true);
	if(conferExpanded)
		conferItem->setExpanded(true);
	if(searchExpanded)
		searchItem->setExpanded(true);
	if(smsExpanded)
		smsItem->setExpanded(true);
	if(statExpanded)
		statusItem->setExpanded(true);
	kdebugf2();
}

void HistoryDlg::searchBranchRefresh()
{
	kdebugf();
	//dodaje now± szukan± frazê
	if(!previousSearchResults.isEmpty())
	{
	QTreeWidgetItem* searchSubItem = new QTreeWidgetItem(searchItem);
	searchSubItem->setText(0, previousSearchResults.last().pattern);
	searchSubItem->setIcon(0, QIcon(icons_manager->loadIcon("SendMessage")));
	}	
// 	searchItem->removeChild();
// 	foreach(HistorySearchResult result, previousSearchResults)
// // 	{
// // 		QTreeWidgetItem* searchSubItem = new QTreeWidgetItem(searchItem);
// // 		searchSubItem->setText(0, result.pattern);
// // 		searchSubItem->setIcon(0, QIcon(icons_manager->loadIcon("SendMessage")));		
// 	}
	kdebugf2();
}

void HistoryDlg::showMainPopupMenu(const QPoint &pos)
{
	MainPopupMenu->popup(MainListView->mapToGlobal(pos));
}

void HistoryDlg::showDetailsPopupMenu(const QPoint &pos)
{
	DetailsPopupMenu->popup(main->getDetailsListView()->mapToGlobal(pos));
}

void HistoryDlg::openChat()
{
	kdebugf();
	MainListViewText* uids_item = dynamic_cast<MainListViewText*>(MainListView->currentItem());
	if (uids_item == NULL)
		return;
  	chat_manager->openPendingMsgs(uids_item->uidsList());
	kdebugf2();
}

void HistoryDlg::lookupUserInfo()
{
	kdebugf();
	MainListViewText* uids_item = dynamic_cast<MainListViewText*>(MainListView->currentItem());
	if (uids_item == NULL)
		return;
	//dirty chiaxor, ale na razie to tylko dla gg jest mo¿liwe
	UserListElement user = (*uids_item->uidsList().begin());
  	if (!user.usesProtocol("Gadu"))
		return;

	SearchDialog *sd = new SearchDialog(kadu, user.ID("Gadu").toUInt());
	sd->show();
	sd->firstSearch();
	kdebugf2();
}

void HistoryDlg::removeHistoryEntriesPerUser()
{
	kdebugf();
	MainListViewText* uids_item = dynamic_cast<MainListViewText*>(MainListView->currentItem());
	if (uids_item == NULL)
		return;
	if (MessageBox::ask(tr("You want to remove all history entries of selected users.\nAre you sure?\n"), "Warning"))
	{
		const UserListElements& uids = uids_item->uidsList();
		sql_history->removeHistory(uids);
		MainListView->removeItemWidget((*MainListView->selectedItems().begin()),0);
		globalRefresh();
		main->getDetailsListView()->clear();
		main->getContentBrowser()->clearMessages();
	}
	kdebugf2();
}

void HistoryDlg::removeHistoryEntriesPerDate()
{
	kdebugf();
	MainListViewText* uids_item = dynamic_cast<MainListViewText*>(MainListView->currentItem());
	if (uids_item == NULL)
		return;
	DetailsListViewItem* dateItem = dynamic_cast<DetailsListViewItem*>(main->getDetailsListView()->currentItem());
	if (dateItem == NULL)
		return;
	if (MessageBox::ask(tr("You want to remove history entries of selected users for selected date.\nAre you sure?\n"), "Warning"))
	{
		HistoryEntryType typeToRemove;
		if(MainListView->currentItem()->parent() == statusItem)
			typeToRemove = EntryTypeStatus;
		else if(MainListView->currentItem()->parent() == smsItem)
			typeToRemove = EntryTypeSms;
		else
			typeToRemove = EntryTypeMessage;
		sql_history->removeHistory(uids_item->uidsList(), dateItem->date(), typeToRemove);
		main->getDetailsListView()->removeItemWidget((*main->getDetailsListView()->selectedItems().begin()),0);
		globalRefresh();
		main->getDetailsListView()->clear();
		main->getContentBrowser()->clearMessages();
	}
	kdebugf2();
}

void HistoryDlg::mainItemChanged(QTreeWidgetItem* item, int column)
{
	kdebugf();
		//TODO: optymalizacja ¿e hej, muli
	main->getDetailsListView()->clear();
	inSearchMode = false;
	if (item->parent() == searchItem)
	{
		inSearchMode = true;
		foreach(HistorySearchResult result, previousSearchResults)
		{
// 				kdebug("itemLabel: %s, wzorzec: %s a text: %s\n", result.itemLabel.local8Bit().data(), result.pattern.local8Bit().data(), item->text(0).local8Bit().data() );
			if(result.pattern == item->text(0))
			{	
				//TODO: zaznaczanie tego tu u³atwia³oby chyba sprawê
				///MainListView->setItemSelected((*MainListView->findItems(result.itemLabel, Qt::MatchExactly).begin()), true);
				foreach(HistorySearchDetailsItem dItem, result.detailsItems)
				{
					new DetailsListViewItem(main->getDetailsListView(), dItem.altNick, dItem.title, dItem.date, QString::number(dItem.length), result.users);
				}	
				break;
			}
		}
	}
	else
	{
		MainListViewText* uids_item = dynamic_cast<MainListViewText*>(item);
	if (uids_item == NULL || item == NULL)
		return;
	const UserListElements& uids = uids_item->uidsList();
	QString uid_str = sql_history->findUidGroup(uids);
	QString query_str, title, length;
	QSqlQuery query(QSqlDatabase::database("kadu-history"));
	QList<QDate> dates;
	if (uids_item->parent() == smsItem)
		dates = sql_history->historySmsDates(uids);
	else if (uids_item->parent() == statusItem)
		dates = sql_history->historyStatusDates(uids);
	else
		dates = sql_history->historyDates(uids);

	foreach(QDate date, dates)
	{
		if (uids_item->parent() == chatsItem || uids_item->parent() == anonChatsItem)
		{
			query_str = "SELECT COUNT(content) FROM kadu_messages WHERE uid_group_id = '%1' AND date(receive_time) = '%2';";
			query_str = query_str.arg(uid_str).arg(date.toString(Qt::ISODate));
			if (!query.exec(query_str))
			{
				MessageBox::msg(query.lastError().text(), false, "Warning");
				kdebugf2();
				break;
			}
			while (query.next())
				length = query.value(0).asString();
			query_str = "SELECT content FROM kadu_messages WHERE uid_group_id = '%1' AND date(receive_time) = '%2' LIMIT 1;";
			query_str = query_str.arg(uid_str).arg(date.toString(Qt::ISODate));
		}
		else if (uids_item->parent() == conferItem)
		{		
			query_str = "SELECT COUNT(content) FROM kadu_messages WHERE uid_group_id = '%1' AND date(receive_time) = '%2';";
			query_str = query_str.arg(uid_str).arg(date.toString(Qt::ISODate));
			if (!query.exec(query_str))
			{
				MessageBox::msg(query.lastError().text(), false, "Warning");
				kdebugf2();
				break;
			}
			while (query.next())
				length = query.value(0).asString();
			query_str = "SELECT content FROM kadu_messages WHERE uid_group_id = '%1' AND date(receive_time) = '%2' LIMIT 1;";
			query_str = query_str.arg(uid_str).arg(date.toString(Qt::ISODate));
		}
		else if (uids_item->parent() == smsItem)
		{
			query_str = "SELECT COUNT(content) FROM kadu_sms WHERE uid_group_id = '%1' AND date(send_time) = '%2';";
			query_str = query_str.arg(uid_str).arg(date.toString(Qt::ISODate));
			if (!query.exec(query_str))
			{
				MessageBox::msg(query.lastError().text(), false, "Warning");
				kdebugf2();
				break;
			}
			while (query.next())
				length = query.value(0).asString();
			query_str = "SELECT content FROM kadu_sms WHERE uid_group_id = '%1' AND date(send_time) = '%2' LIMIT 1;";
			query_str = query_str.arg(uid_str).arg(date.toString(Qt::ISODate));
		
		}
		else if (uids_item->parent() == statusItem)
		{
			query_str = "SELECT COUNT(status) FROM kadu_status WHERE uid_group_id = '%1' AND date(time) = '%2';";
			query_str = query_str.arg(uid_str).arg(date.toString(Qt::ISODate));
			if (!query.exec(query_str))
			{
				MessageBox::msg(query.lastError().text(), false, "Warning");
				kdebugf2();
				break;
			}
			while (query.next())
				length = query.value(0).asString();
			query_str = "SELECT description, status FROM kadu_status WHERE uid_group_id = '%1' AND date(time) = '%2' LIMIT 1;";
			query_str = query_str.arg(uid_str).arg(date.toString(Qt::ISODate));
		}
		
		if (!query.exec(query_str))
		{
			MessageBox::msg(query.lastError().text(), false, "Warning");
			kdebugf2();
			break;
		}
		while (query.next())
		{
			title = QString::fromUtf8(query.value(0).asString());
			HtmlDocument::escapeText(title);
			int l = title.length();
				title.truncate(maxLen);
			if (l > maxLen)
				title += " ...";
		}
		
		
		new DetailsListViewItem(main->getDetailsListView(), (*uids.begin()).altNick(), title, date, length);
	}

	}

	//itemAt -> ?co¶ innego
	QTreeWidgetItem* lastItem = main->getDetailsListView()->itemAt(main->getDetailsListView()->topLevelItemCount() - 1, 0);
	main->getDetailsListView()->setCurrentItem(lastItem);
	detailsItemChanged(lastItem, 0);
	main->getDetailsListView()->resizeColumnToContents(0); 
	main->getDetailsListView()->resizeColumnToContents(1); 
	kdebugf2();
}

void HistoryDlg::detailsItemChanged(QTreeWidgetItem* item, int column)
{
	kdebugf();
	//TODO: optymalizacja ¿e hej, przeca to muli, chamszczyzna
	if (inSearchMode)
	{
		DetailsListViewItem* date_item = ((DetailsListViewItem*)item);
		QList<ChatMessage*> chat_messages;
		if(previousSearchResults.last().currentType == EntryTypeStatus)
			chat_messages = sql_history->getStatusEntries(date_item->uidsList(), date_item->date());
		else if(previousSearchResults.last().currentType == EntryTypeSms)
			chat_messages = sql_history->getSmsEntries(date_item->uidsList(), date_item->date());
		else
			chat_messages = sql_history->historyMessages(date_item->uidsList(), date_item->date());



		QString pattern = previousSearchResults.last().pattern;
			foreach(ChatMessage* message, chat_messages)
			{	
				if(message->unformattedMessage.contains(pattern))
					message->unformattedMessage.replace(pattern, QString("<a style=\"color: #fffc46; font-weight: bold; background: #ff3e2c\">%2</a>").arg(pattern));
			}
			main->getContentBrowser()->clearMessages();
			main->getContentBrowser()->appendMessages(chat_messages);

	}

	MainListViewText* uids_item =
		dynamic_cast<MainListViewText*>(MainListView->selectedItems().first());
	if (uids_item == NULL || item == NULL)
		return;
	if (uids_item->parent() == chatsItem || uids_item->parent() == conferItem|| uids_item->parent() == anonChatsItem)
	{
		DetailsListViewItem* date_item = ((DetailsListViewItem*)item);
		QList<ChatMessage*> chat_messages = sql_history->historyMessages(uids_item->uidsList(), date_item->date());
		if (inSearchMode)
		{
// // // 			anchors.clear();
// // // 			int i = 0;
			QString pattern = previousSearchResults.last().pattern;
			foreach(ChatMessage* message, chat_messages)
			{	
				if(message->unformattedMessage.contains(pattern))
					message->unformattedMessage.replace(pattern, QString("<a style=\"color: #fffc46; font-weight: bold; background: #ff3e2c\">%2</a>").arg(pattern));
			}
			main->getContentBrowser()->clearMessages();
			//idxIt = anchors.begin();
			main->getContentBrowser()->appendMessages(chat_messages);
// 		// 	ContentBrowser->setCursorPosition((*idxIt),0);
		}
		else
		{
			main->getContentBrowser()->clearMessages();
			main->getContentBrowser()->appendMessages(chat_messages);
		// 	ContentBrowser->setCursorPosition(0,0);
		}
	}
	else if (uids_item->parent() == smsItem)
	{
		DetailsListViewItem* date_item = ((DetailsListViewItem*)item);
		QList<ChatMessage*> sms_messages = sql_history->getSmsEntries(uids_item->uidsList(), date_item->date());
		if (inSearchMode/*!searchParameters.pattern.isEmpty()*/)
		{
// // // 			anchors.clear();
// // // 			int i = 0;
			QString pattern = previousSearchResults.last().pattern;
			foreach(ChatMessage* message, sms_messages)
			{	
				if(message->unformattedMessage.contains(pattern))
					message->unformattedMessage.replace(pattern, QString("<a style=\"color: #fffc46; font-weight: bold; background: #ff3e2c\">%2</a>").arg(pattern));
			}
			main->getContentBrowser()->clearMessages();
			if(!sms_messages.isEmpty())
				main->getContentBrowser()->appendMessages(sms_messages);
		}
		else
		{
			main->getContentBrowser()->clearMessages();
			if(!sms_messages.isEmpty())
				main->getContentBrowser()->appendMessages(sms_messages);
		}
		
	}
	else if (uids_item->parent() == statusItem)
	{
		DetailsListViewItem* date_item = ((DetailsListViewItem*)item);
		QList<ChatMessage*> status_messages = sql_history->getStatusEntries(uids_item->uidsList(), date_item->date());
		if (inSearchMode/*!searchParameters.pattern.isEmpty()*/)
		{
// // // 			anchors.clear();
// // // 			int i = 0;
			QString pattern = previousSearchResults.last().pattern;
			foreach(ChatMessage* message, status_messages)
			{	
				if(message->unformattedMessage.contains(pattern))
					message->unformattedMessage.replace(pattern, QString("<a style=\"color: #fffc46; font-weight: bold; background: #ff3e2c\">%2</a>").arg(pattern));
			}
			main->getContentBrowser()->clearMessages();
			if(!status_messages.isEmpty())
				main->getContentBrowser()->appendMessages(status_messages);
		}
		else
		{
			main->getContentBrowser()->clearMessages();
			if(!status_messages.isEmpty())
				main->getContentBrowser()->appendMessages(status_messages);
		}
		
	}
	kdebugf2();
}

void HistoryDlg::selectedUsersNeeded(UserGroup*& users)
{
	kdebugf();
	//foreach(UserListElements u, uid_groups)
	//	users->addUsers(new UserGroup(u));
	kdebugf2();

}

void HistoryDlg::searchActionActivated(QAction* sender, bool toggled)
{
	kdebugf();	
	advSearchWindow = new HistoryAdvSearchWindow(this);	
// 	advSearchWindow->show();
	if (advSearchWindow->exec() == QDialog::Accepted)
	{
		searchHistory();
// 		main->getContentBrowser()->findText(searchParameters.pattern);
	}
	delete advSearchWindow;
	kdebugf2();
}

void HistoryDlg::searchNextActActivated(QAction* sender, bool toggled)
{
	kdebugf();
	if(!main->getContentBrowser()->findText(searchParameters.pattern))
		MessageBox::msg(tr("There ar no more matches"), false, "Warning");
	kdebugf2();
}

void HistoryDlg::searchPrevActActivated(QAction* sender, bool toggled)
{
	kdebugf();
	if(!main->getContentBrowser()->findText(searchParameters.pattern, QWebPage::FindBackward))
		MessageBox::msg(tr("There ar no more previous matches"), false, "Warning");
	kdebugf2();
}

void HistoryDlg::setSearchParameters(HistorySearchParameters& params)
{
	searchParameters = params;
}

void HistoryDlg::searchHistory()
{
	kdebugf();
	MainListViewText* uids_item = dynamic_cast<MainListViewText*>(MainListView->currentItem());
	if (uids_item == NULL)
		return;
	if(MainListView->currentItem()->parent() == statusItem)
		searchParameters.currentType = EntryTypeStatus;
	else if(MainListView->currentItem()->parent() == smsItem)
		searchParameters.currentType = EntryTypeSms;
	else
		searchParameters.currentType = EntryTypeMessage;
	main->statusBar()->showMessage(tr("Searching history. Please wait..."));
	main->getDetailsListView()->clear();
	isSearchInProgress = true;
	setEnabled(false);
	HistorySearchResult currentResult = sql_history->searchHistory(uids_item->uidsList(), searchParameters);
	if(currentResult.detailsItems.isEmpty())
	{
		main->getDetailsListView()->clear();
		MessageBox::msg(tr("Sorry, nothing found."), false, "Warning");
	}
	else
	foreach(HistorySearchDetailsItem dItem, currentResult.detailsItems)
		new DetailsListViewItem(main->getDetailsListView(), dItem.altNick, dItem.title, dItem.date, QString::number(dItem.length), uids_item->uidsList());	
	previousSearchResults.append(currentResult);
	isSearchInProgress = false;
	inSearchMode = true;
	main->statusBar()->showMessage(tr("Ready"));
	searchBranchRefresh();
	setEnabled(true);

///TODO: detailsChanged odpalaæ na pierwszym itemie z detailsTreeWidgeta
	//detailsItemChanged(main->getDetailsListView()->items().last(), 0);
	main->getContentBrowser()->findText(searchParameters.pattern);
	kdebugf2();
}

void HistoryDlg::showDialog(UserListElements users)
{
	selectedUsers = users;
	//TODO: zamieniæ na tylko zaznaczanie wybranego usera albo co tam...ale globalRefresh() i tak musi byæ gdzie indziej... ino gdzie?
	globalRefresh();
	show();
}

void HistoryDlg::keyPressEvent(QKeyEvent *e)
{
	if(e->matches(QKeySequence::Find))
		if(main->getDockWidget()->isHidden())
			main->getDockWidget()->show();
		else
			main->getDockWidget()->hide();	
}

void HistoryDlg::closeEvent(QCloseEvent *e)
{
	saveWindowGeometry(this, "History", "HistoryWindowGeometry");
	if (isSearchInProgress)
	{
		e->ignore();
		closeDemand = true;
	}
	else
		e->accept();
}

HistorySearchDetailsItem::HistorySearchDetailsItem(QString altNick, QString title, QDate date, int length) : altNick(altNick), date(date), title(title), length(length)
{
}

HistorySearchResult::HistorySearchResult() : detailsItems(QList<HistorySearchDetailsItem>())
{
}


HistoryDlg* historyDialog = NULL;
