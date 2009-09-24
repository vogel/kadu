/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QDateEdit>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QMenu>
#include <QtGui/QPushButton>
#include <QtGui/QSplitter>
#include <QtGui/QStatusBar>
#include <QtGui/QVBoxLayout>

#include "chat/message/message.h"

#include "chat/type/chat-type.h"
#include "chat/filter/chat-name-filter.h"
#include "chat/aggregate-chat.h"
#include "chat/chat-aggregator-builder.h"
#include "contacts/model/contacts-model-base.h"
#include "gui/actions/actions.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/delayed-line-edit.h"
#include "gui/windows/message-box.h"
#include "misc/misc.h"
#include "activate.h"
#include "debug.h"
#include "icons-manager.h"

#include "model/chat-dates-model.h"
#include "model/history-chats-model.h"
#include "model/history-chats-model-proxy.h"
#include "storage/history-storage.h"

#include "history-window.h"
#include <QItemDelegate>

HistoryWindow::HistoryWindow(QWidget *parent) :
		MainWindow(parent)
{
	kdebugf();

	setWindowTitle(tr("History"));
	setWindowIcon(IconsManager::instance()->loadIcon("History"));

	createGui();
	connectGui();

	loadWindowGeometry(this, "History", "HistoryWindowGeometry", 200, 200, 700, 500);
//TODO 0.6.6:
	MainPopupMenu = new QMenu(this);
	MainPopupMenu->addAction(IconsManager::instance()->loadIcon("OpenChat"), tr("&Open chat"), this, SLOT(openChat()));
//	MainPopupMenu->addAction(IconsManager::instance()->loadIcon("LookupUserInfo"), tr("&Search in directory"), this, SLOT(lookupUserInfo()));
//	MainPopupMenu->addAction(IconsManager::instance()->loadIcon("ClearHistory"), tr("&Clear history"), this, SLOT(removeHistoryEntriesPerUser()));
	
	DetailsPopupMenu = new QMenu(this);
//	DetailsPopupMenu->addAction(IconsManager::instance()->loadIcon("ClearHistory"), tr("&Remove entries"), this, SLOT(removeHistoryEntriesPerDate()));

	kdebugf2();
}

HistoryWindow::~HistoryWindow()
{
    	kdebugf();

	saveWindowGeometry(this, "History", "HistoryDialogGeometry");
	
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

	createChatTree(splitter);
	QSplitter *rightSplitter = new QSplitter(Qt::Vertical, splitter);

	QWidget *rightWidget = new QWidget(rightSplitter);
	QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);

	QWidget *filterWidget = new QWidget(rightWidget);
	rightLayout->addWidget(filterWidget);

	createFilterBar(filterWidget);

	DetailsListView = new QTreeView(rightWidget);
	rightLayout->addWidget(DetailsListView);

	DetailsListView->setRootIsDecorated(false);
	DetailsListView->setModel(new ChatDatesModel(0, QList<QDate>(), this));
	DetailsListView->setUniformRowHeights(true);

	ContentBrowser = new ChatMessagesView(0, rightSplitter);
	ContentBrowser->setPruneEnabled(false);

	QList<int> sizes;
	sizes.append(100);
	sizes.append(300);
	splitter->setSizes(sizes);

	setCentralWidget(mainWidget);
}

void HistoryWindow::createChatTree(QWidget *parent)
{
	QWidget *chatsWidget = new QWidget(parent);
	QVBoxLayout *layout = new QVBoxLayout(chatsWidget);

	QLineEdit *filterLineEdit = new QLineEdit(chatsWidget);
	connect(filterLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(filterLineChanged(const QString &)));
	layout->addWidget(filterLineEdit);

	ChatsTree = new QTreeView(parent);
	layout->addWidget(ChatsTree);

	ChatsModel = new HistoryChatsModel(this);

	ChatsModelProxy = new HistoryChatsModelProxy(this);
	ChatsModelProxy->setSourceModel(ChatsModel);

	NameFilter = new ChatNameFilter(this);
	ChatsModelProxy->addFilter(NameFilter);

	ChatsTree->setModel(ChatsModelProxy);
	ChatsModelProxy->sort(1);
	ChatsModelProxy->sort(0); // do the sorting
	ChatsTree->setRootIsDecorated(true);
}

void HistoryWindow::createFilterBar(QWidget *parent)
{
	QHBoxLayout *layout = new QHBoxLayout(parent);

	DelayedLineEdit *searchLineEdit = new DelayedLineEdit(parent);
	layout->addWidget(searchLineEdit);

	QDateEdit *fromDate = new QDateEdit(parent);
	fromDate->setCalendarPopup(true);
	layout->addWidget(fromDate);

	QDateEdit *toDate = new QDateEdit(parent);
	toDate->setCalendarPopup(true);
	layout->addWidget(toDate);

	connect(searchLineEdit, SIGNAL(delayedTextChanged(const QString &)),
			this, SLOT(searchTextChanged(const QString &)));
	connect(fromDate, SIGNAL(dateChanged(const QDate &)),
			this, SLOT(fromDateChanged(const QDate &)));
	connect(toDate, SIGNAL(dateChanged(const QDate &)),
			this, SLOT(toDateChanged(const QDate &)));
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

void HistoryWindow::updateData()
{
	kdebugf();

	ChatsModel->clear();
	QList<Chat *> usedChats;
	QList<Chat *> chatsList = History::instance()->chatsList(Search);
	QList<Chat *> result;

	foreach (Chat *chat, chatsList)
	{
		if (usedChats.contains(chat))
			continue;

		AggregateChat *aggregate = dynamic_cast<AggregateChat *>(ChatAggregatorBuilder::buildAggregateChat(chat->contacts()));
		if (!aggregate)
			continue;
		if (aggregate->chats().size() > 1)
		{
			result.append(aggregate);
			foreach (Chat *usedChat, aggregate->chats())
				usedChats.append(usedChat);
		}
		else
		{
			result.append(chat);
			usedChats.append(chat);
		}
	}

	ChatsModel->addChats(result);
}

void HistoryWindow::selectChat(Chat *chat)
{
	ChatType type = chat->type();
	QModelIndex chatTypeIndex = ChatsModelProxy->chatTypeIndex(type);

	if (!chatTypeIndex.isValid())
	{
		chatActivated(QModelIndex());
		return;
	}

	ChatsTree->collapseAll();
	ChatsTree->expand(chatTypeIndex);

	QModelIndex chatIndex = ChatsModelProxy->chatIndex(chat);
	ChatsTree->selectionModel()->select(chatIndex, QItemSelectionModel::ClearAndSelect);

	chatActivated(chatIndex);
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

	QList<QDate> chatDates = History::instance()->datesForChat(chat, Search);
	model->setChat(chat);
	model->setDates(chatDates);

	int lastRow = model->rowCount(QModelIndex()) - 1;
	QModelIndex last = model->index(lastRow, 0, QModelIndex());
	DetailsListView->selectionModel()->setCurrentIndex(last, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

	dateActivated(last);

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

	QList<Message> messages = History::instance()->messages(chat, date);
	ContentBrowser->setChat(chat);
	ContentBrowser->clearMessages();
	ContentBrowser->appendMessages(messages);

	kdebugf2();
}

void HistoryWindow::filterLineChanged(const QString &filterText)
{
	NameFilter->setName(filterText);
}

void HistoryWindow::searchTextChanged(const QString &searchText)
{
	Search.setQuery(searchText);
	updateData();
}

void HistoryWindow::fromDateChanged(const QDate &date)
{
	Search.setFromDate(date);
	updateData();
}

void HistoryWindow::toDateChanged(const QDate &date)
{
	Search.setToDate(date);
	updateData();
}

void HistoryWindow::showMainPopupMenu(const QPoint &pos)
{
	bool isValid = true;
	Chat *chat = ChatsTree->indexAt(pos).data(ChatRole).value<Chat *>();
	if (!chat)
		isValid = false;

	foreach (QAction *action, MainPopupMenu->actions())
		action->setEnabled(isValid);

	MainPopupMenu->exec(QCursor::pos());
}

void HistoryWindow::showDetailsPopupMenu(const QPoint &pos)
{
	bool isValid = true;
	Chat *chat = DetailsListView->indexAt(pos).data(ChatRole).value<Chat *>();
	QDate date = DetailsListView->indexAt(pos).data(DateRole).value<QDate>();

	if (!chat || !date.isValid())
		isValid = false;

	foreach (QAction *action, DetailsPopupMenu->actions())
		action->setEnabled(isValid);

	DetailsPopupMenu->exec(QCursor::pos());
}

void HistoryWindow::show(Chat *chat)
{
	if (!History::instance()->currentStorage())
	{
		MessageBox::msg(tr("There is no history storage module loaded!"), false, "Warning");
		return;
	}

	updateData();
	selectChat(chat);

	QWidget::show();
	_activateWindow(this);
}

void HistoryWindow::openChat()
{
	kdebugf();
	Chat *chat = ChatsTree->currentIndex().data(ChatRole).value<Chat *>();
	if (!chat)
		return;

	ChatWidgetManager::instance()->openChatWidget(chat, true);

	kdebugf2();
}

bool HistoryWindow::supportsActionType(ActionDescription::ActionType type)
{
	return (type == ActionDescription::TypeGlobal || type == ActionDescription::TypeChat || type == ActionDescription::TypeHistory);
}

void HistoryWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		e->accept();
		hide();
	}
	else
		QWidget::keyPressEvent(e);
}

HistoryWindow *historyDialog = 0;
