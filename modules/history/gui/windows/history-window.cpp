/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QDockWidget>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QMenu>
#include <QtGui/QPushButton>
#include <QtGui/QSplitter>
#include <QtGui/QStatusBar>
#include <QtGui/QVBoxLayout>

#include "contacts/model/contacts-model-base.h"
#include "gui/actions/actions.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/windows/message-box.h"
#include "misc/misc.h"
#include "debug.h"
#include "emoticons.h"
#include "icons-manager.h"

#include "model/chat-dates-model.h"
#include "model/history-chats-model.h"
#include "storage/history-storage.h"

#include "history-window.h"
#include <QItemDelegate>

HistoryWindow::HistoryWindow(QWidget *parent) :
		MainWindow(parent), isSearchInProgress(0), closeDemand(0), inSearchMode(0)
{
	kdebugf();

	setWindowTitle(tr("History"));
	setWindowIcon(IconsManager::instance()->loadIcon("History"));

	createGui();
	connectGui();

	/*
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
	*/

	// 	historyAdvSearchActionDescription = new ActionDescription(
	// 		ActionDescription::TypeHistory, "historyAdvSearchAction",
	// 		window, SLOT(advSearchActivated(QAction *, bool)),
	// 		"Configuration", tr("&Advanced search")
	// 	);

	//	ToolBar::addDefaultAction("SQL History toolbar", "historySearchAction", 1, true);
	//	ToolBar::addDefaultAction("SQL History toolbar", "historyNextResultsAction", 2, true);
	//	ToolBar::addDefaultAction("SQL History toolbar", "historyPrevResultsAction", 3, true);
	// 	ToolBar::addDefaultAction("SQL History toolbar", "historyAdvSearchAction", 4, true);

	// 	loadToolBarsFromConfig("sqlHistoryTopDockArea", Qt::TopToolBarArea);
	// 	loadToolBarsFromConfig("sqlHistoryBottomDockArea", Qt::BottomToolBarArea);
	// 	loadToolBarsFromConfig("sqlHistoryLeftDockArea", Qt::LeftToolBarArea);
	// 	loadToolBarsFromConfig("sqlHistoryRightDockArea", Qt::RightToolBarArea);

	loadWindowGeometry(this, "History", "HistoryWindowGeometry", 200, 200, 700, 500);
	maxLen = 36;

	MainPopupMenu = new QMenu(this);
	MainPopupMenu->addAction(IconsManager::instance()->loadIcon("OpenChat"), tr("&Open chat"), this, SLOT(openChat()));
	MainPopupMenu->addAction(IconsManager::instance()->loadIcon("LookupUserInfo"), tr("&Search in directory"), this, SLOT(lookupUserInfo()));
	MainPopupMenu->addAction(IconsManager::instance()->loadIcon("ClearHistory"), tr("&Clear history"), this, SLOT(removeHistoryEntriesPerUser()));
	
	DetailsPopupMenu = new QMenu(this);
	DetailsPopupMenu->addAction(IconsManager::instance()->loadIcon("ClearHistory"), tr("&Remove entries"), this, SLOT(removeHistoryEntriesPerDate()));

	kdebugf2();
}

HistoryWindow::~HistoryWindow()
{
	// for valgrind
	QStringList searchActions;
	searchActions << "historySearchAction" << "historyNextResultsAction" << "historyPrevResultsAction" << "historyAdvSearchAction";
	foreach(QString act, searchActions)
	{
		ActionDescription *a = KaduActions[act];
		delete a;
	}
	saveWindowGeometry(this, "History", "HistoryDialogGeometry");

	
	kdebugf();
	//writeToolBarsToConfig("history");
	kdebugf2();
}

void HistoryWindow::createGui()
{
	QWidget *mainWidget = new QWidget(this);
	mainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QVBoxLayout *layout = new QVBoxLayout(mainWidget);
	layout->setMargin(5);
	layout->setSpacing(5);

	QSplitter *splitter = new QSplitter(Qt::Horizontal, mainWidget);
	layout->addWidget(splitter);

	ChatsTree = new QTreeView(splitter);
	ChatsModel = new HistoryChatsModel(this);
	ChatsTree->setModel(ChatsModel);
	ChatsTree->setRootIsDecorated(true);

	QSplitter *rightSplitter = new QSplitter(Qt::Vertical, splitter);

	DetailsListView = new QTreeView(rightSplitter);
	DetailsListView->setModel(new ChatDatesModel(0, QList<QDate>(), this));

	ContentBrowser = new ChatMessagesView(0, rightSplitter);
	ContentBrowser->setPruneEnabled(false);
	///	ContentBrowser->setMargin(config_file.readNumEntry("General", "ParagraphSeparator"));

	QList<int> sizes;
	sizes.append(100);
	sizes.append(300);
	splitter->setSizes(sizes);

	setCentralWidget(mainWidget);
}

void HistoryWindow::connectGui()
{
	connect(ChatsTree, SIGNAL(activated(const QModelIndex &)),
			this, SLOT(chatActivated(const QModelIndex &)));
	connect(DetailsListView, SIGNAL(activated(const QModelIndex &)),
			this, SLOT(dateActivated(const QModelIndex &)));

	ChatsTree->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ChatsTree, SIGNAL(customContextMenuRequested(QPoint)),
			this, SLOT(showMainPopupMenu(QPoint)));

	DetailsListView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(DetailsListView, SIGNAL(customContextMenuRequested(QPoint)),
			this, SLOT(showDetailsPopupMenu(QPoint)));
}

void HistoryWindow::globalRefresh()
{
	kdebugf();
/*
	MainListView->clear();
	chatsItem = new QTreeWidgetItem(MainListView, QStringList(tr("Chats")));
	chatsItem->setExpanded(true);
	///QTreeWidgetItem *ft = new QTreeWidgetItem(MainListView, QStringList(tr("File transfers")));
	conferItem = new QTreeWidgetItem(MainListView, QStringList(tr("Conferences")));
	smsItem = new QTreeWidgetItem(MainListView, QStringList(tr("SMS")));
	statusItem = new QTreeWidgetItem(MainListView, QStringList(tr("Status")));
	statusItem->setExpanded(false);
	searchItem = new QTreeWidgetItem(MainListView, QStringList(tr("Search")));*/

// 	chatsItem->setIcon(0, IconsManager::instance()->loadIcon("OpenChat"));
// 	///ft->setIcon(0, icons_manager->loadIcon("SendFile"));
// 	smsItem->setIcon(0, IconsManager::instance()->loadIcon("Mobile"));
// 	conferItem->setIcon(0, IconsManager::instance()->loadIcon("ManageModules"));
// 	statusItem->setIcon(0, IconsManager::instance()->loadIcon("Busy"));
// 	searchItem->setIcon(0, IconsManager::instance()->loadIcon("LookupUserInfo"));

	ChatsModel->clear();

	QList<Chat *> chatsList = History::instance()->chatsList();
	
// 	MainListItem* mainItem;
	foreach (Chat *chat, chatsList)
		ChatsModel->addChat(chat);
// 	{
// 		if (chat->contacts().count() > 1)
// 			mainItem = new MainListItem(conferItem, chat);
// 		else
// 			mainItem = new MainListItem(chatsItem, chat);
// 		mainItem->setIcon(0, IconsManager::instance()->loadIcon("Online"));
// 	}

	kdebugf2();
}

void HistoryWindow::searchBranchRefresh()
{
	kdebugf();
/*
	//dodaje now� szukan� fraz�
	if(!previousSearchResults.isEmpty())
	{
	QTreeWidgetItem* searchSubItem = new QTreeWidgetItem(searchItem);
	searchSubItem->setText(0, previousSearchResults.last().pattern);
	searchSubItem->setIcon(0, QIcon(IconsManager::instance()->loadIcon("SendMessage")));
	}	
// 	searchItem->removeChild();
// 	foreach(HistorySearchResult result, previousSearchResults)
// // 	{
// // 		QTreeWidgetItem* searchSubItem = new QTreeWidgetItem(searchItem);
// // 		searchSubItem->setText(0, result.pattern);
// // 		searchSubItem->setIcon(0, QIcon(IconsManager::instance()->loadIcon("SendMessage")));		
// 	}
*/
	kdebugf2();
}

void HistoryWindow::showMainPopupMenu(const QPoint &pos)
{
	MainPopupMenu->popup(ChatsTree->mapToGlobal(pos));
}

void HistoryWindow::showDetailsPopupMenu(const QPoint &pos)
{
	DetailsPopupMenu->popup(DetailsListView->mapToGlobal(pos));
}

void HistoryWindow::openChat()
{
	kdebugf();
// 	MainListItem* chatItem = dynamic_cast<MainListItem *>(MainListView->currentItem());
// 	if (chatItem)
// 		ChatWidgetManager::instance()->openChatWidget(chatItem->chat(), true);

	kdebugf2();
}

void HistoryWindow::lookupUserInfo()
{
	kdebugf();
// 	MainListItem* uids_item = dynamic_cast<MainListItem*>(MainListView->currentItem());
// 	if (uids_item == NULL)
// 		return;
	//dirty chiaxor, ale na razie to tylko dla gg jest mo�liwe
// 	Contact user = (*uids_item->uidsList().begin());
//   	if (!user.usesProtocol("Gadu"))
// 		return;
// 
// 	SearchDialog *sd = new SearchDialog(kadu, user.ID("Gadu").toUInt());
// 	sd->show();
// 	sd->firstSearch();
	kdebugf2();
}

void HistoryWindow::removeHistoryEntriesPerUser()
{
	kdebugf();

// 	MainListItem* chatItem = dynamic_cast<MainListItem *>(MainListView->currentItem());
// 	if (!chatItem)
// 		return;
// 
// 	if (MessageBox::ask(tr("You want to remove all history entries of selected users.\nAre you sure?\n"), "Warning"))
// 	{
// 		Chat *chat = chatItem->chat();
// 		if (chat)
// 			History::instance()->currentStorage()->clearChatHistory(chat);
// 		MainListView->removeItemWidget((*MainListView->selectedItems().begin()),0);
// 		globalRefresh();
// 		main->getDetailsListView()->clear();
// 		main->getContentBrowser()->clearMessages();
// 	}

	kdebugf2();
}

void HistoryWindow::removeHistoryEntriesPerDate()
{
	kdebugf();
// 	MainListItem* uids_item = dynamic_cast<MainListItem*>(MainListView->currentItem());
// 	if (uids_item == NULL)
// 		return;
// 	DetailsListItem* dateItem = dynamic_cast<DetailsListItem*>(main->getDetailsListView()->currentItem());
// 	if (dateItem == NULL)
// 		return;
// 	if (MessageBox::ask(tr("You want to remove history entries of selected users for selected date.\nAre you sure?\n"), "Warning"))
// 	{
// 		HistoryEntryType typeToRemove;
// 		if(MainListView->currentItem()->parent() == statusItem)
// 			typeToRemove = EntryTypeStatus;
// 		else if(MainListView->currentItem()->parent() == smsItem)
// 			typeToRemove = EntryTypeSms;
// 		else
// 			typeToRemove = EntryTypeMessage;
// 		sql_history->removeHistory(uids_item->uidsList(), dateItem->date(), typeToRemove);
// 		main->getDetailsListView()->removeItemWidget((*main->getDetailsListView()->selectedItems().begin()),0);
// 		globalRefresh();
// 		main->getDetailsListView()->clear();
// 		main->getContentBrowser()->clearMessages();
// 	}
	kdebugf2();
}

void HistoryWindow::chatActivated(const QModelIndex &index)
{
	kdebugf();

	Chat *chat = index.data(ChatRole).value<Chat *>();
	if (!chat)
		return;
	
	ChatDatesModel *model = dynamic_cast<ChatDatesModel *>(DetailsListView->model());
	if (!model)
		return;

	QList<QDate> chatDates = History::instance()->datesForChat(chat);
	model->setChat(chat);
	model->setDates(chatDates);

// 	QTreeWidgetItem* lastItem = main->getDetailsListView()->itemAt(0, main->getDetailsListView()->topLevelItemCount());
// 	main->getDetailsListView()->setCurrentItem(lastItem);
// 	detailsListItemClicked(lastItem, 0);
// 	main->getDetailsListView()->resizeColumnToContents(0); 
// 	main->getDetailsListView()->resizeColumnToContents(1);

	kdebugf2();
}

void HistoryWindow::dateActivated(const QModelIndex &index)
{
	kdebugf();

	Chat *chat = index.data(ChatRole).value<Chat *>();
	if (!chat)
		return;

	QDate date = index.data(DateRole).value<QDate>();
	if (!date.isValid())
		return;

	QList<Message> messages = History::instance()->getMessages(chat, date);
	ContentBrowser->setChat(chat);
	ContentBrowser->clearMessages();
	ContentBrowser->appendMessages(messages);

	kdebugf2();
}

void HistoryWindow::searchActionActivated(QAction* sender, bool toggled)
{
	kdebugf();	
	advSearchWindow = new HistorySearchDialog(this);	
// 	advSearchWindow->show();
	if (advSearchWindow->exec() == QDialog::Accepted)
	{
		searchHistory();
// 		main->getContentBrowser()->findText(searchParameters.pattern);
	}
	delete advSearchWindow;
	kdebugf2();
}

void HistoryWindow::searchNextActActivated(QAction* sender, bool toggled)
{
	kdebugf();
// 	if(!main->getContentBrowser()->findText(searchParameters.pattern))
// 		MessageBox::msg(tr("There ar no more matches"), false, "Warning");
	kdebugf2();
}

void HistoryWindow::searchPrevActActivated(QAction* sender, bool toggled)
{
	kdebugf();
// 	if(!main->getContentBrowser()->findText(searchParameters.pattern, QWebPage::FindBackward))
// 		MessageBox::msg(tr("There ar no more previous matches"), false, "Warning");
	kdebugf2();
}

void HistoryWindow::setSearchParameters(HistorySearchParameters& params)
{
	searchParameters = params;
}

void HistoryWindow::searchHistory()
{
	kdebugf();
/*	MainListItem* uids_item = dynamic_cast<MainListItem*>(MainListView->currentItem());
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
	HistorySearchResult currentResult;// = sql_history->searchHistory(uids_item->uidsList(), searchParameters);
	if(currentResult.detailsItems.isEmpty())
	{
		main->getDetailsListView()->clear();
		MessageBox::msg(tr("Sorry, nothing found."), false, "Warning");
	}
	else
	foreach(HistorySearchDetailsItem dItem, currentResult.detailsItems)
		new DetailsListItem(main->getDetailsListView(), dItem.altNick, dItem.title, dItem.date, QString::number(dItem.length), uids_item->uidsList());	
	previousSearchResults.append(currentResult);
	isSearchInProgress = false;
	inSearchMode = true;
	main->statusBar()->showMessage(tr("Ready"));
	searchBranchRefresh();
	setEnabled(true);

///TODO: detailsChanged odpala� na pierwszym itemie z detailsTreeWidgeta
	//detailsListItemClicked(main->getDetailsListView()->items().last(), 0);
	main->getContentBrowser()->findText(searchParameters.pattern);*/
	kdebugf2();
}

void HistoryWindow::show(ContactSet users)
{
	if (!History::instance()->currentStorage())
	{
		MessageBox::msg(tr("There is no history storage module loaded!"), false, "Warning");
		return;
	}

	selectedUsers = users;
	globalRefresh();
	QWidget::show();
}

void HistoryWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->matches(QKeySequence::Find))
		if (DockWidget->isHidden())
			DockWidget->show();
		else
			DockWidget->hide();
}

void HistoryWindow::closeEvent(QCloseEvent *e)
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

bool HistoryWindow::supportsActionType(ActionDescription::ActionType type)
{
	return (type == ActionDescription::TypeGlobal || type == ActionDescription::TypeChat || type == ActionDescription::TypeHistory);
}

void HistoryWindow::quickSearchPhraseTyped(const QString &text)
{
	kdebugf();
	if (!text.isEmpty())
		if (!ContentBrowser->findText(text))
			if (!ContentBrowser->findText(text, QWebPage::FindBackward))
				MessageBox::msg(tr("Nic ciekawego"), false, "Warning");
			kdebugf2();
}

HistorySearchDetailsItem::HistorySearchDetailsItem(QString altNick, QString title, QDate date, int length) : altNick(altNick), date(date), title(title), length(length)
{
}

HistorySearchResult::HistorySearchResult() : detailsItems(QList<HistorySearchDetailsItem>())
{
}

HistoryWindow *historyDialog = 0;
