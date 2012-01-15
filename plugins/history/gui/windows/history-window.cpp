/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtCore/QScopedPointer>
#include <QtGui/QDateEdit>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QItemDelegate>
#include <QtGui/QLineEdit>
#include <QtGui/QMenu>
#include <QtGui/QPushButton>
#include <QtGui/QSplitter>
#include <QtGui/QStandardItem>
#include <QtGui/QStandardItemModel>
#include <QtGui/QStatusBar>
#include <QtGui/QVBoxLayout>

#include "accounts/account-manager.h"
#include "buddies/buddy-manager.h"
#include "buddies/model/buddies-model-base.h"
#include "buddies/model/buddy-list-model.h"
#include "chat/aggregate-chat-manager.h"
#include "chat/chat-details-aggregate.h"
#include "chat/chat-manager.h"
#include "chat/model/chats-list-model.h"
#include "chat/type/chat-type-manager.h"
#include "chat/type/chat-type.h"
#include "gui/actions/actions.h"
#include "gui/actions/base-action-context.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/delayed-line-edit.h"
#include "gui/widgets/filter-widget.h"
#include "gui/widgets/filtered-tree-view.h"
#include "gui/widgets/talkable-delegate-configuration.h"
#include "gui/widgets/talkable-menu-manager.h"
#include "gui/widgets/talkable-tree-view.h"
#include "gui/windows/message-dialog.h"
#include "icons/kadu-icon.h"
#include "message/message.h"
#include "misc/misc.h"
#include "model/merged-proxy-model-factory.h"
#include "model/model-chain.h"
#include "model/roles.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol-menu-manager.h"
#include "status/status-type-data.h"
#include "status/status-type-manager.h"
#include "talkable/filter/name-talkable-filter.h"
#include "talkable/model/talkable-proxy-model.h"
#include "activate.h"
#include "debug.h"

#include "model/buddy-status-dates-model.h"
#include "model/chat-dates-model.h"
#include "model/dates-model-item.h"
#include "model/sms-dates-model.h"
#include "search/history-search-parameters.h"
#include "storage/history-storage.h"
#include "history-tree-item.h"
#include "history.h"
#include "timed-status.h"

#include "history-window.h"

HistoryWindow * HistoryWindow::Instance = 0;

void HistoryWindow::show(const Chat &chat)
{
	if (!History::instance()->currentStorage())
	{
		MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), tr("There is no history storage plugin loaded!"));
		return;
	}

	Chat aggregate = AggregateChatManager::instance()->aggregateChat(chat);
	if (!aggregate)
		aggregate = chat;

	if (!Instance)
		Instance = new HistoryWindow();

	Instance->updateData();
	Instance->selectChat(aggregate);

	Instance->setVisible(true);
	_activateWindow(Instance);
}

HistoryWindow::HistoryWindow(QWidget *parent) :
		MainWindow(new BaseActionContext(), "history", parent)
{
	kdebugf();

	setProperty("ownWindowIcon", true);
	setWindowRole("kadu-history");
	setAttribute(Qt::WA_DeleteOnClose);

	setWindowTitle(tr("History"));
	setWindowIcon(KaduIcon("kadu_icons/history").icon());

	createGui();

	loadWindowGeometry(this, "History", "HistoryWindowGeometry", 200, 200, 750, 500);

	DetailsPopupMenu = new QMenu(this);
	DetailsPopupMenu->addAction(KaduIcon("kadu_icons/clear-history").icon(), tr("&Remove entries"), this, SLOT(removeHistoryEntriesPerDate()));

	Context = static_cast<BaseActionContext *>(actionContext());
	updateContext();

	kdebugf2();
}

HistoryWindow::~HistoryWindow()
{
	kdebugf();

	saveWindowGeometry(this, "History", "HistoryDialogGeometry");

	Instance = 0;

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

	createTrees(splitter);
	QSplitter *rightSplitter = new QSplitter(Qt::Vertical, splitter);

	QWidget *rightWidget = new QWidget(rightSplitter);
	QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
	rightLayout->setSpacing(0);
	rightLayout->setMargin(0);

	DetailsListView = new QTreeView(rightWidget);
	rightLayout->addWidget(DetailsListView);

	MyChatDatesModel = new ChatDatesModel(Chat::null, QVector<DatesModelItem>(), this);
	MyBuddyStatusDatesModel = new BuddyStatusDatesModel(Buddy::null, QVector<DatesModelItem>(), this);
	MySmsDatesModel = new SmsDatesModel(QString(), QVector<DatesModelItem>(), this);

	DetailsListView->setAlternatingRowColors(true);
	DetailsListView->setRootIsDecorated(false);
	DetailsListView->setUniformRowHeights(true);

	DetailsListView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(DetailsListView, SIGNAL(customContextMenuRequested(QPoint)),
			this, SLOT(showDetailsPopupMenu(QPoint)));

	ContentBrowser = new ChatMessagesView(Chat::null, false, rightSplitter);
	ContentBrowser->setFocusPolicy(Qt::StrongFocus);
	ContentBrowser->setForcePruneDisabled(true);

	QList<int> sizes;
	sizes.append(150);
	sizes.append(300);
	splitter->setSizes(sizes);

	setCentralWidget(mainWidget);
}

void HistoryWindow::createTrees(QWidget *parent)
{
	QTabWidget *tabWidget = new QTabWidget(parent);

	tabWidget->addTab(createChatTree(tabWidget), tr("Chats"));
	tabWidget->addTab(createStatusTree(tabWidget), tr("Statuses"));
	tabWidget->addTab(createSMSTree(tabWidget), tr("SMS"));
}

QWidget * HistoryWindow::createChatTree(QWidget *parent)
{
	FilteredTreeView *chatsTalkableWidget = new FilteredTreeView(FilteredTreeView::FilterAtTop, parent);

	ChatsTalkableTree = new TalkableTreeView(chatsTalkableWidget);
	ChatsTalkableTree->setSelectionMode(QAbstractItemView::SingleSelection);
	ChatsTalkableTree->setUseConfigurationColors(true);
	ChatsTalkableTree->setContextMenuEnabled(true);
	ChatsTalkableTree->delegateConfiguration().setShowMessagePixmap(false);

	ChatsModel = new ChatsListModel(ChatsTalkableTree);
	ChatsBuddiesModel = new BuddyListModel(ChatsTalkableTree);

	QList<QAbstractItemModel *> models;
	models.append(ChatsModel);
	models.append(ChatsBuddiesModel);

	QAbstractItemModel *mergedModel = MergedProxyModelFactory::createKaduModelInstance(models, ChatsTalkableTree);

	ChatsModelChain = new ModelChain(mergedModel, ChatsTalkableTree);

	TalkableProxyModel *proxyModel = new TalkableProxyModel(ChatsModelChain);
	proxyModel->setSortByStatusAndUnreadMessages(false);

	NameTalkableFilter *nameTalkableFilter = new NameTalkableFilter(NameTalkableFilter::AcceptMatching, proxyModel);
	connect(chatsTalkableWidget, SIGNAL(filterChanged(QString)), nameTalkableFilter, SLOT(setName(QString)));
	proxyModel->addFilter(nameTalkableFilter);

	ChatsModelChain->addProxyModel(proxyModel);

	ChatsTalkableTree->setChain(ChatsModelChain);

	connect(ChatsTalkableTree, SIGNAL(currentChanged(Talkable)), this, SLOT(currentChatChanged(Talkable)));
	connect(ChatsTalkableTree, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showChatsPopupMenu(QPoint)));
	ChatsTalkableTree->setContextMenuPolicy(Qt::CustomContextMenu);

	chatsTalkableWidget->setView(ChatsTalkableTree);

	return chatsTalkableWidget;
}

QWidget * HistoryWindow::createStatusTree(QWidget *parent)
{
	FilteredTreeView *statusesTalkableWidget = new FilteredTreeView(FilteredTreeView::FilterAtTop, parent);

	StatusesTalkableTree = new TalkableTreeView(statusesTalkableWidget);
	StatusesTalkableTree->setUseConfigurationColors(true);
	StatusesTalkableTree->setContextMenuEnabled(true);
	StatusesTalkableTree->delegateConfiguration().setShowMessagePixmap(false);

	StatusBuddiesModel = new BuddyListModel(StatusesTalkableTree);
	StatusesModelChain = new ModelChain(StatusBuddiesModel, StatusesTalkableTree);

	TalkableProxyModel *proxyModel = new TalkableProxyModel(StatusesModelChain);
	proxyModel->setSortByStatusAndUnreadMessages(false);

	NameTalkableFilter *nameTalkableFilter = new NameTalkableFilter(NameTalkableFilter::AcceptMatching, proxyModel);
	connect(statusesTalkableWidget, SIGNAL(filterChanged(QString)), nameTalkableFilter, SLOT(setName(QString)));
	proxyModel->addFilter(nameTalkableFilter);

	StatusesModelChain->addProxyModel(proxyModel);

	StatusesTalkableTree->setChain(StatusesModelChain);

	connect(StatusesTalkableTree, SIGNAL(currentChanged(Talkable)), this, SLOT(currentStatusChanged(Talkable)));
	connect(StatusesTalkableTree, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showStatusesPopupMenu(QPoint)));
	StatusesTalkableTree->setContextMenuPolicy(Qt::CustomContextMenu);

	statusesTalkableWidget->setView(StatusesTalkableTree);

	return statusesTalkableWidget;
}

QWidget * HistoryWindow::createSMSTree(QWidget *parent)
{
	FilteredTreeView *smsListWidget = new FilteredTreeView(FilteredTreeView::FilterAtTop, parent);

	SmsListView = new QListView(smsListWidget);
	SmsListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	SmsListView->setSelectionMode(QAbstractItemView::ExtendedSelection);

	SmsModel = new QStandardItemModel(StatusesTalkableTree);
	QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(SmsModel);
	proxyModel->setSourceModel(SmsModel);

	connect(smsListWidget, SIGNAL(filterChanged(QString)), proxyModel, SLOT(setFilterFixedString(QString)));

	SmsListView->setModel(SmsModel);

	connect(SmsListView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
	        this, SLOT(currentSmsChanged(QModelIndex,QModelIndex)));
	connect(SmsListView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showSmsPopupMenu(QPoint)));
	SmsListView->setContextMenuPolicy(Qt::CustomContextMenu);

	smsListWidget->setView(SmsListView);

	return smsListWidget;
}

void HistoryWindow::updateData()
{
	kdebugf();

	QSet<Chat> usedChats;
	QVector<Chat> chatsList = History::instance()->chatsList(HistorySearchParameters());

	QVector<Chat> result;
	QVector<Chat> conferenceChats;
	BuddyList buddies;

	foreach (const Chat &chat, chatsList)
	{
		if (usedChats.contains(chat))
			continue;
		Chat aggregate = AggregateChatManager::instance()->aggregateChat(chat);
		if (aggregate)
		{
			ChatDetailsAggregate *details = qobject_cast<ChatDetailsAggregate *>(aggregate.details());
			Q_ASSERT(details);
			foreach (const Chat &usedChat, details->chats())
				usedChats.insert(usedChat);

			result.append(aggregate);
			if (aggregate.contacts().size() > 1)
				conferenceChats.append(aggregate);
			else if (1 == aggregate.contacts().size())
				buddies.append(BuddyManager::instance()->byContact(*aggregate.contacts().begin(), ActionCreateAndAdd));
		}
		else
		{
			result.append(chat);
			usedChats.insert(chat);
			if (chat.contacts().size() > 1)
				conferenceChats.append(chat);
			else if (1 == chat.contacts().size())
				buddies.append(BuddyManager::instance()->byContact(*chat.contacts().begin(), ActionCreateAndAdd));
		}
	}

	ChatsModel->setChats(conferenceChats);
	ChatsBuddiesModel->setBuddyList(buddies);

	QVector<Buddy> statusBuddies = History::instance()->statusBuddiesList(HistorySearchParameters());
	StatusBuddiesModel->setBuddyList(statusBuddies.toList());

	QList<QString> smsRecipients = History::instance()->smsRecipientsList(HistorySearchParameters());

	SmsModel->clear();
	foreach (const QString &smsRecipient, smsRecipients)
		SmsModel->appendRow(new QStandardItem(KaduIcon("phone").icon(), smsRecipient));

	chatActivated(Chat::null);
}

void HistoryWindow::selectChat(const Chat &chat)
{
	ChatsTalkableTree->selectionModel()->clearSelection();
	QModelIndexList indexesToSelect;

	if (chat.contacts().size() == 1)
		indexesToSelect = ChatsModelChain->indexListForValue(chat.contacts().begin()->ownerBuddy());
	else if (chat.contacts().size() > 1)
		indexesToSelect = ChatsModelChain->indexListForValue(chat);

	if (1 == indexesToSelect.size())
	{
		ChatsTalkableTree->selectionModel()->select(indexesToSelect.at(0), QItemSelectionModel::Select);
		chatActivated(chat);
	}
	else
		chatActivated(Chat::null);
}

void HistoryWindow::chatActivated(const Chat &chat)
{
	kdebugf();

	QModelIndex selectedIndex = DetailsListView->selectionModel()
			? DetailsListView->selectionModel()->currentIndex()
			: QModelIndex();
	QDate date = selectedIndex.data(DateRole).toDate();

	QVector<DatesModelItem> chatDates = History::instance()->datesForChat(chat, HistorySearchParameters());
	MyChatDatesModel->setChat(chat);
	MyChatDatesModel->setDates(chatDates);

	QModelIndex select = MyChatDatesModel->indexForDate(date);
	if (!select.isValid())
	{
		int lastRow = MyChatDatesModel->rowCount(QModelIndex()) - 1;
		if (lastRow >= 0)
			select = MyChatDatesModel->index(lastRow);
	}

	DetailsListView->setModel(MyChatDatesModel);

	connect(DetailsListView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
			this, SLOT(dateCurrentChanged(QModelIndex,QModelIndex)), Qt::UniqueConnection);

	DetailsListView->selectionModel()->setCurrentIndex(select, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

	kdebugf2();
}

void HistoryWindow::statusBuddyActivated(const Buddy &buddy)
{
	kdebugf();

	QModelIndex selectedIndex = DetailsListView->model()
			? DetailsListView->selectionModel()->currentIndex()
			: QModelIndex();

	QDate date = selectedIndex.data(DateRole).toDate();

	QVector<DatesModelItem> statusDates = History::instance()->datesForStatusBuddy(buddy, HistorySearchParameters());
	MyBuddyStatusDatesModel->setBuddy(buddy);
	MyBuddyStatusDatesModel->setDates(statusDates);

	if (date.isValid())
		selectedIndex = MyBuddyStatusDatesModel->indexForDate(date);
	if (!selectedIndex.isValid())
	{
		int lastRow = MyBuddyStatusDatesModel->rowCount(QModelIndex()) - 1;
		if (lastRow >= 0)
			selectedIndex = MyBuddyStatusDatesModel->index(lastRow);
	}

	DetailsListView->setModel(MyBuddyStatusDatesModel);

	connect(DetailsListView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
			this, SLOT(dateCurrentChanged(QModelIndex,QModelIndex)), Qt::UniqueConnection);

	DetailsListView->selectionModel()->setCurrentIndex(selectedIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

	kdebugf2();
}

void HistoryWindow::smsRecipientActivated(const QString& recipient)
{
	kdebugf();

	QModelIndex selectedIndex = DetailsListView->model()
			? DetailsListView->selectionModel()->currentIndex()
			: QModelIndex();

	QDate date = selectedIndex.data(DateRole).toDate();

	QVector<DatesModelItem> smsDates = History::instance()->datesForSmsRecipient(recipient, HistorySearchParameters());
	MySmsDatesModel->setRecipient(recipient);
	MySmsDatesModel->setDates(smsDates);

	if (date.isValid())
		selectedIndex = MySmsDatesModel->indexForDate(date);
	if (!selectedIndex.isValid())
	{
		int lastRow = MySmsDatesModel->rowCount(QModelIndex()) - 1;
		if (lastRow >= 0)
			selectedIndex = MySmsDatesModel->index(lastRow);
	}

	DetailsListView->setModel(MySmsDatesModel);

	connect(DetailsListView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
			this, SLOT(dateCurrentChanged(QModelIndex,QModelIndex)), Qt::UniqueConnection);

	DetailsListView->selectionModel()->setCurrentIndex(selectedIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

	kdebugf2();
}

void HistoryWindow::dateCurrentChanged(const QModelIndex &current, const QModelIndex &previous)
{
	kdebugf();

	if (current == previous)
		return;

	HistoryTreeItem treeItem = current.data(HistoryItemRole).value<HistoryTreeItem>();
	QDate date = current.data(DateRole).value<QDate>();

	ContentBrowser->setUpdatesEnabled(false);

	switch (treeItem.type())
	{
		case HistoryTypeNone:
			ContentBrowser->setChat(Chat::null);
			ContentBrowser->clearMessages();
			break;

		case HistoryTypeChat:
		{
			Chat chat = treeItem.chat();
			QVector<Message> messages;
			if (chat && date.isValid())
				messages = History::instance()->messages(chat, date);
			ContentBrowser->setChat(chat);
			ContentBrowser->clearMessages();
			ContentBrowser->appendMessages(messages);
			break;
		}

		case HistoryTypeStatus:
		{
			Buddy buddy = treeItem.buddy();
			QList<TimedStatus> statuses;
			if (buddy && date.isValid())
				statuses = History::instance()->statuses(buddy, date);
			if (!buddy.contacts().isEmpty())
				ContentBrowser->setChat(ChatManager::instance()->findChat(ContactSet(buddy.contacts().at(0)), true));
			ContentBrowser->clearMessages();
			ContentBrowser->appendMessages(statusesToMessages(statuses));
			break;
		}

		case HistoryTypeSms:
		{
			QString recipient = treeItem.smsRecipient();
			QVector<Message> sms;
			if (!recipient.isEmpty() && date.isValid())
				sms = History::instance()->sms(recipient, date);
			ContentBrowser->setChat(Chat::null);
			ContentBrowser->clearMessages();
			ContentBrowser->appendMessages(sms);
			break;
		}
	}

	ContentBrowser->setUpdatesEnabled(true);

	kdebugf2();
}

QVector<Message> HistoryWindow::statusesToMessages(const QList<TimedStatus> &statuses)
{
	QVector<Message> messages;

	foreach (const TimedStatus &timedStatus, statuses)
	{
		Message message = Message::create();
		message.setStatus(MessageStatusReceived);
		message.setType(MessageTypeReceived);

		const StatusTypeData &typeData = StatusTypeManager::instance()->statusTypeData(timedStatus.status().type());

		if (timedStatus.status().description().isEmpty())
			message.setContent(typeData.name());
		else
			message.setContent(QString("%1 with description: %2")
					.arg(typeData.name())
					.arg(timedStatus.status().description()));

		message.setReceiveDate(timedStatus.dateTime());
		message.setSendDate(timedStatus.dateTime());

		messages.append(message);
	}

	return messages;
}

void HistoryWindow::showChatsPopupMenu(const QPoint &pos)
{
	Q_UNUSED(pos)

	QScopedPointer<QMenu> menu;

	menu.reset(TalkableMenuManager::instance()->menu(this, ChatsTalkableTree->actionContext()));
	menu->addSeparator();
	menu->addAction(KaduIcon("kadu_icons/clear-history").icon(),
			tr("&Clear Chat History"), this, SLOT(clearChatHistory()));

	menu->exec(QCursor::pos());
}

void HistoryWindow::showStatusesPopupMenu(const QPoint &pos)
{
	Q_UNUSED(pos)

	QScopedPointer<QMenu> menu;

	menu.reset(TalkableMenuManager::instance()->menu(this, StatusesTalkableTree->actionContext()));
	menu->addSeparator();
	menu->addAction(KaduIcon("kadu_icons/clear-history").icon(),
			tr("&Clear Status History"), this, SLOT(clearStatusHistory()));

	menu->exec(QCursor::pos());
}

void HistoryWindow::showSmsPopupMenu(const QPoint &pos)
{
	Q_UNUSED(pos)

	QScopedPointer<QMenu> menu;

	menu.reset(new QMenu(this));
	menu->addSeparator();
	menu->addAction(KaduIcon("kadu_icons/clear-history").icon(),
			tr("&Clear SMS History"), this, SLOT(clearSmsHistory()));

	menu->exec(QCursor::pos());
}

void HistoryWindow::showDetailsPopupMenu(const QPoint &pos)
{
	QDate date = DetailsListView->indexAt(pos).data(DateRole).value<QDate>();
	if (!date.isValid())
		return;

	bool isValid = false;

	HistoryTreeItem treeItem = DetailsListView->indexAt(pos).data(HistoryItemRole).value<HistoryTreeItem>();
	if (treeItem.type() == HistoryTypeChat && treeItem.chat())
		isValid = true;
	else if (treeItem.type() == HistoryTypeStatus && treeItem.buddy() && !treeItem.buddy().contacts().isEmpty())
		isValid = true;
	else if (treeItem.type() == HistoryTypeSms && !treeItem.smsRecipient().isEmpty())
		isValid = true;

	if (isValid)
		DetailsPopupMenu->exec(QCursor::pos());
}

void HistoryWindow::clearChatHistory()
{
	if (!ChatsTalkableTree->actionContext())
		return;

	const Chat &chat = ChatsTalkableTree->actionContext()->chat();
	if (!chat)
		return;

	History::instance()->currentStorage()->clearChatHistory(chat);
	updateData();
}

void HistoryWindow::clearStatusHistory()
{
	if (!StatusesTalkableTree->actionContext())
		return;

	const BuddySet &buddies = StatusesTalkableTree->actionContext()->buddies();
	if (buddies.isEmpty())
		return;

	foreach (const Buddy &buddy, buddies)
		History::instance()->currentStorage()->clearStatusHistory(buddy);

	updateData();
}

void HistoryWindow::clearSmsHistory()
{
	bool removed = false;

	const QModelIndexList &indexes = SmsListView->selectionModel()->selectedIndexes();
	foreach (const QModelIndex &index, indexes)
	{
		QString recipient = index.data(Qt::DisplayRole).toString();
		if (recipient.isEmpty())
			continue;

		removed = true;
		History::instance()->currentStorage()->clearSmsHistory(recipient);
	}

	if (removed)
		updateData();
}

void HistoryWindow::removeHistoryEntriesPerDate()
{
	QDate date = DetailsListView->currentIndex().data(DateRole).value<QDate>();
	if (!date.isValid())
		return;

	HistoryTreeItem treeItem = DetailsListView->currentIndex().data(HistoryItemRole).value<HistoryTreeItem>();
	if (treeItem.type() == HistoryTypeChat && treeItem.chat())
	{
		History::instance()->currentStorage()->clearChatHistory(treeItem.chat(), date);
		chatActivated(treeItem.chat());
	}
	else if (treeItem.type() == HistoryTypeStatus && treeItem.buddy() && !treeItem.buddy().contacts().isEmpty())
	{
		History::instance()->currentStorage()->clearStatusHistory(treeItem.buddy(), date);
		statusBuddyActivated(treeItem.buddy());
	}
	else if (treeItem.type() == HistoryTypeSms && !treeItem.smsRecipient().isEmpty())
	{
		History::instance()->currentStorage()->clearSmsHistory(treeItem.smsRecipient(), date);
		smsRecipientActivated(treeItem.smsRecipient());
	}
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
		close();
	}
	else if (e == QKeySequence::Copy && !ContentBrowser->selectedText().isEmpty())
		// Do not use triggerPageAction(), see bug #2345.
		ContentBrowser->pageAction(QWebPage::Copy)->trigger();
	else
		QWidget::keyPressEvent(e);
}

void HistoryWindow::updateContext()
{
	ContactSet contacts = selectedContacts();

	Context->blockChangedSignal();
	Context->setContacts(contacts);
	Context->setBuddies(contacts.toBuddySet());
	Context->setChat(selectedChat());
	Context->unblockChangedSignal();
}

ContactSet HistoryWindow::selectedContacts() const
{
	return ContactSet();
}

Chat HistoryWindow::selectedChat() const
{
	return Chat::null;
}

void HistoryWindow::currentChatChanged(const Talkable &talkable)
{
	switch (talkable.type())
	{
		case Talkable::ItemChat:
		{
			chatActivated(talkable.toChat());
			break;
		}
		case Talkable::ItemBuddy:
		{
			BuddySet buddies;
			buddies.insert(talkable.toBuddy());
			chatActivated(ChatManager::instance()->findChat(buddies, true));
			break;
		}
		default:
			break;
	}
}

void HistoryWindow::currentStatusChanged(const Talkable &talkable)
{
	statusBuddyActivated(talkable.toBuddy());
}

void HistoryWindow::currentSmsChanged(const QModelIndex &current, const QModelIndex &previous)
{
	Q_UNUSED(previous);

	smsRecipientActivated(current.data().toString());
}
