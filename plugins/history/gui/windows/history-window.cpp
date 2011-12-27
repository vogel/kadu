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
#include <QtGui/QStatusBar>
#include <QtGui/QVBoxLayout>

#include "accounts/account-manager.h"
#include "buddies/model/buddies-model-base.h"
#include "chat/aggregate-chat-manager.h"
#include "chat/chat-details-aggregate.h"
#include "chat/chat-manager.h"
#include "chat/type/chat-type-manager.h"
#include "chat/type/chat-type.h"
#include "gui/actions/actions.h"
#include "gui/actions/base-action-context.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/delayed-line-edit.h"
#include "gui/widgets/filter-widget.h"
#include "gui/widgets/talkable-menu-manager.h"
#include "gui/windows/message-dialog.h"
#include "icons/kadu-icon.h"
#include "message/message.h"
#include "misc/misc.h"
#include "model/roles.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol-menu-manager.h"
#include "status/status-type-data.h"
#include "status/status-type-manager.h"
#include "talkable/filter/name-talkable-filter.h"
#include "activate.h"
#include "debug.h"

#include "model/buddy-status-dates-model.h"
#include "model/chat-dates-model.h"
#include "model/dates-model-item.h"
#include "model/history-chats-model-proxy.h"
#include "model/history-chats-model.h"
#include "model/sms-dates-model.h"
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

	setWindowRole("kadu-history");
	setAttribute(Qt::WA_DeleteOnClose);

	setWindowTitle(tr("History"));
	setWindowIcon(KaduIcon("kadu_icons/history").icon());

	createGui();
	connectGui();

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

	createChatTree(splitter);
	QSplitter *rightSplitter = new QSplitter(Qt::Vertical, splitter);

	QWidget *rightWidget = new QWidget(rightSplitter);
	QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
	rightLayout->setSpacing(0);
	rightLayout->setMargin(0);

	QWidget *filterWidget = new QWidget(rightWidget);
	rightLayout->addWidget(filterWidget);

	createFilterBar(filterWidget);

	DetailsListView = new QTreeView(rightWidget);
	rightLayout->addWidget(DetailsListView);

	MyChatDatesModel = new ChatDatesModel(Chat::null, QVector<DatesModelItem>(), this);
	MyBuddyStatusDatesModel = new BuddyStatusDatesModel(Buddy::null, QVector<DatesModelItem>(), this);
	MySmsDatesModel = new SmsDatesModel(QString(), QVector<DatesModelItem>(), this);

	DetailsListView->setAlternatingRowColors(true);
	DetailsListView->setRootIsDecorated(false);
	DetailsListView->setUniformRowHeights(true);

	ContentBrowser = new ChatMessagesView(Chat::null, false, rightSplitter);
	ContentBrowser->setFocusPolicy(Qt::StrongFocus);
	ContentBrowser->setForcePruneDisabled(true);

	QList<int> sizes;
	sizes.append(150);
	sizes.append(300);
	splitter->setSizes(sizes);

	setCentralWidget(mainWidget);
}

void HistoryWindow::createChatTree(QWidget *parent)
{
	QWidget *chatsWidget = new QWidget(parent);
	chatsWidget->setMinimumWidth(150);
	QVBoxLayout *layout = new QVBoxLayout(chatsWidget);

	FilterWidget *filterWidget = new FilterWidget(chatsWidget);
	layout->addWidget(filterWidget);

	ChatsTree = new QTreeView(parent);
	ChatsTree->header()->hide();
	layout->addWidget(ChatsTree);

	ChatsModel = new HistoryChatsModel(this);

	ChatsModelProxy = new HistoryChatsModelProxy(this);
	ChatsModelProxy->setSourceModel(ChatsModel);

	StatusBuddyNameFilter = new NameTalkableFilter(NameTalkableFilter::UndecidedMatching, this);
	connect(filterWidget, SIGNAL(textChanged(const QString &)), StatusBuddyNameFilter, SLOT(setName(const QString &)));
	ChatsModelProxy->addTalkableFilter(StatusBuddyNameFilter);

	ChatsTree->setAlternatingRowColors(true);
	ChatsTree->setModel(ChatsModelProxy);
	ChatsTree->setRootIsDecorated(true);
}

void HistoryWindow::createFilterBar(QWidget *parent)
{
	QGridLayout *layout = new QGridLayout(parent);
	layout->setSpacing(0);
	layout->setMargin(0);

	QLabel *filterLabel = new QLabel(tr("Search") + ": ", parent);
	layout->addWidget(filterLabel, 0, 0, 1, 1);

	DelayedLineEdit *searchLineEdit = new DelayedLineEdit(parent);
	layout->addWidget(searchLineEdit, 0, 1, 1, 4);

	QCheckBox *filterByDate = new QCheckBox(tr("by date"), parent);
	filterByDate->setChecked(false);
	layout->addWidget(filterByDate, 1, 0, 1, 1);

	FromDateLabel = new QLabel(tr("From") + ": ", parent);
	FromDateLabel->setEnabled(false);
	layout->addWidget(FromDateLabel, 1, 1, 1, 1, Qt::AlignRight);

	FromDate = new QDateEdit(parent);
	FromDate->setEnabled(false);
	FromDate->setCalendarPopup(true);
	FromDate->setDate(QDateTime::currentDateTime().addDays(-7).date());
	layout->addWidget(FromDate, 1, 2, 1, 1);

	ToDateLabel = new QLabel(tr("To") + ": ", parent);
	ToDateLabel->setEnabled(false);
	layout->addWidget(ToDateLabel, 1, 3, 1, 1, Qt::AlignRight);

	ToDate = new QDateEdit(parent);
	ToDate->setEnabled(false);
	ToDate->setCalendarPopup(true);
	ToDate->setDate(QDateTime::currentDateTime().date());
	layout->addWidget(ToDate, 1, 4, 1, 1);

	connect(filterByDate, SIGNAL(stateChanged(int)),
			this, SLOT(dateFilteringEnabled(int)));

	connect(searchLineEdit, SIGNAL(delayedTextChanged(const QString &)),
			this, SLOT(searchTextChanged(const QString &)));
	connect(FromDate, SIGNAL(dateChanged(const QDate &)),
			this, SLOT(fromDateChanged(const QDate &)));
	connect(ToDate, SIGNAL(dateChanged(const QDate &)),
			this, SLOT(toDateChanged(const QDate &)));
}

void HistoryWindow::connectGui()
{
	connect(ChatsTree->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
			this, SLOT(treeCurrentChanged(QModelIndex,QModelIndex)));
	connect(ChatsTree->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
			this, SLOT(updateContext()));

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

	QModelIndex index = ChatsTree->selectionModel()->currentIndex();
	HistoryTreeItem treeItem = index.data(HistoryItemRole).value<HistoryTreeItem>();

	QSet<Chat> usedChats;
	QVector<Chat> chatsList = History::instance()->chatsList(Search);
	QVector<Chat> result;

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
		}
		else
		{
			result.append(chat);
			usedChats.insert(chat);
		}
	}

	ChatsModel->setChats(result);

	selectHistoryItem(treeItem);

	ChatsModel->setStatusBuddies(History::instance()->statusBuddiesList(Search));
	ChatsModel->setSmsRecipients(History::instance()->smsRecipientsList(Search));
}

void HistoryWindow::selectChat(const Chat &chat)
{
	ChatDetailsAggregate *aggregateDetails = qobject_cast<ChatDetailsAggregate *>(chat.details());
	Q_ASSERT(!aggregateDetails || !aggregateDetails->chats().isEmpty());
	QString typeName = aggregateDetails ? aggregateDetails->chats().at(0).type() : chat.type();
	ChatType *type = ChatTypeManager::instance()->chatType(typeName);

	if (!type)
	{
		treeItemActivated(HistoryTreeItem());
		return;
	}

	QModelIndex chatTypeIndex = ChatsModelProxy->chatTypeIndex(type);
	if (!chatTypeIndex.isValid())
	{
		treeItemActivated(HistoryTreeItem());
		return;
	}

	ChatsTree->collapseAll();
	ChatsTree->expand(chatTypeIndex);

	QModelIndex chatIndex = ChatsModelProxy->chatIndex(chat);
	ChatsTree->selectionModel()->select(chatIndex, QItemSelectionModel::ClearAndSelect);

	chatActivated(chat);
}

void HistoryWindow::selectStatusBuddy(const Buddy &buddy)
{
	QModelIndex statusIndex = ChatsModelProxy->statusIndex();
	if (!statusIndex.isValid())
	{
		treeItemActivated(HistoryTreeItem());
		return;
	}

	ChatsTree->collapseAll();
	ChatsTree->expand(statusIndex);

	QModelIndex statusBuddyIndex = ChatsModelProxy->statusBuddyIndex(buddy);
	ChatsTree->selectionModel()->select(statusBuddyIndex, QItemSelectionModel::ClearAndSelect);

	statusBuddyActivated(buddy);
}

void HistoryWindow::selectSmsRecipient(const QString& recipient)
{
	QModelIndex smsIndex = ChatsModelProxy->smsIndex();
	if (!smsIndex.isValid())
	{
		treeItemActivated(HistoryTreeItem());
		return;
	}

	ChatsTree->collapseAll();
	ChatsTree->expand(smsIndex);

	QModelIndex smsRecipientIndex = ChatsModelProxy->smsRecipientIndex(recipient);
	ChatsTree->selectionModel()->select(smsRecipientIndex, QItemSelectionModel::ClearAndSelect);

	smsRecipientActivated(recipient);
}

void HistoryWindow::selectHistoryItem(const HistoryTreeItem &item)
{
	switch (item.type())
	{
		case HistoryTypeNone:
			// do nothing
			break;

		case HistoryTypeChat:
			selectChat(item.chat());
			break;

		case HistoryTypeStatus:
			selectStatusBuddy(item.buddy());
			break;

		case HistoryTypeSms:
			selectSmsRecipient(item.smsRecipient());
			break;
	}
}

void HistoryWindow::chatActivated(const Chat &chat)
{
	kdebugf();

	QModelIndex selectedIndex = DetailsListView->selectionModel()
			? DetailsListView->selectionModel()->currentIndex()
			: QModelIndex();
	QDate date = selectedIndex.data(DateRole).toDate();

	QVector<DatesModelItem> chatDates = History::instance()->datesForChat(chat, Search);
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
			this, SLOT(dateCurrentChanged(QModelIndex,QModelIndex)));

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

	QVector<DatesModelItem> statusDates = History::instance()->datesForStatusBuddy(buddy, Search);
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
			this, SLOT(dateCurrentChanged(QModelIndex,QModelIndex)));

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

	QVector<DatesModelItem> smsDates = History::instance()->datesForSmsRecipient(recipient, Search);
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
			this, SLOT(dateCurrentChanged(QModelIndex,QModelIndex)));

	DetailsListView->selectionModel()->setCurrentIndex(selectedIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

	kdebugf2();
}

void HistoryWindow::treeItemActivated(const HistoryTreeItem &item)
{
	switch (item.type())
	{
		case HistoryTypeNone:
			// do nothing
			break;

		case HistoryTypeChat:
			chatActivated(item.chat());
			break;

		case HistoryTypeStatus:
			statusBuddyActivated(item.buddy());
			break;

		case HistoryTypeSms:
			smsRecipientActivated(item.smsRecipient());
			break;
	}
}

void HistoryWindow::treeCurrentChanged(const QModelIndex &current, const QModelIndex &previous)
{
	if (current == previous)
		return;

	kdebugf();

	treeItemActivated(current.data(HistoryItemRole).value<HistoryTreeItem>());

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

	if (!Search.query().isEmpty())
		QTimer::singleShot(500, this, SLOT(selectQueryText()));


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

void HistoryWindow::searchTextChanged(const QString &searchText)
{
	Search.setQuery(searchText);
	updateData();
}

void HistoryWindow::fromDateChanged(const QDate &date)
{
	Search.setFromDate(date);
	if (ToDate->date() < date)
		ToDate->setDate(date);
	else
		updateData();
}

void HistoryWindow::toDateChanged(const QDate &date)
{
	Search.setToDate(date);
	if (FromDate->date() > date)
		FromDate->setDate(date);
	else
		updateData();
}

void HistoryWindow::showMainPopupMenu(const QPoint &pos)
{
	QScopedPointer<QMenu> menu;

	HistoryTreeItem treeItem = ChatsTree->indexAt(pos).data(HistoryItemRole).value<HistoryTreeItem>();
	switch (treeItem.type())
	{
		case HistoryTypeChat:
		{
			Chat chat = treeItem.chat();
			if (!chat)
				return;

			menu.reset(TalkableMenuManager::instance()->menu(this, actionContext()));
			menu->addSeparator();
			menu->addAction(KaduIcon("kadu_icons/clear-history").icon(),
					tr("&Clear Chat History"), this, SLOT(clearChatHistory()));

			break;
		}

		case HistoryTypeStatus:
		{
			Buddy buddy = treeItem.buddy();
			if (!buddy || buddy.contacts().isEmpty())
				return;

			menu.reset(TalkableMenuManager::instance()->menu(this, actionContext()));
			menu->addSeparator();
			menu->addAction(KaduIcon("kadu_icons/clear-history").icon(),
					tr("&Clear Status History"), this, SLOT(clearStatusHistory()));

			break;
		}

		case HistoryTypeSms:
		{
			QString recipient = treeItem.smsRecipient();
			if (recipient.isEmpty())
				return;

			menu.reset(new QMenu(this));
			menu->addAction(KaduIcon("kadu_icons/clear-history").icon(),
					tr("&Clear SMS History"), this, SLOT(clearSmsHistory()));
			break;
		}

		default:
			return;
	}

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

void HistoryWindow::openChat()
{
	const Chat &chat = ChatsTree->currentIndex().data(ChatRole).value<Chat>();
	ChatWidget * const chatWidget = ChatWidgetManager::instance()->byChat(chat, true);
	if (chatWidget)
		chatWidget->activate();
}

void HistoryWindow::clearChatHistory()
{
	kdebugf();
	Chat chat = ChatsTree->currentIndex().data(ChatRole).value<Chat>();
	if (!chat)
		return;

	History::instance()->currentStorage()->clearChatHistory(chat);
	updateData();
	kdebugf2();
}

void HistoryWindow::clearStatusHistory()
{
	kdebugf();
	Buddy buddy = ChatsTree->currentIndex().data(BuddyRole).value<Buddy>();
	if (!buddy)
		return;

	History::instance()->currentStorage()->clearStatusHistory(buddy);
	updateData();
	kdebugf2();
}

void HistoryWindow::clearSmsHistory()
{
	kdebugf();
	QString recipient = ChatsTree->currentIndex().data(Qt::DisplayRole).toString();
	if (recipient.isEmpty())
		return;

	History::instance()->currentStorage()->clearSmsHistory(recipient);
	updateData();
	kdebugf2();
}

void HistoryWindow::removeHistoryEntriesPerDate()
{
	QDate date = DetailsListView->currentIndex().data(DateRole).value<QDate>();
	if (!date.isValid())
		return;

	HistoryTreeItem treeItem = DetailsListView->currentIndex().data(HistoryItemRole).value<HistoryTreeItem>();
	if (treeItem.type() == HistoryTypeChat && treeItem.chat())
		History::instance()->currentStorage()->clearChatHistory(treeItem.chat(), date);
	else if (treeItem.type() == HistoryTypeStatus && treeItem.buddy() && !treeItem.buddy().contacts().isEmpty())
		History::instance()->currentStorage()->clearStatusHistory(treeItem.buddy(), date);
	else if (treeItem.type() == HistoryTypeSms && !treeItem.smsRecipient().isEmpty())
		History::instance()->currentStorage()->clearSmsHistory(treeItem.smsRecipient(), date);

	selectHistoryItem(ChatsTree->currentIndex().data(HistoryItemRole).value<HistoryTreeItem>());
}

void HistoryWindow::selectQueryText()
{
	ContentBrowser->findText(QString()); // clear old selection
	ContentBrowser->findText(Search.query(), QWebPage::HighlightAllOccurrences);
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
	Chat chat = ChatsTree->currentIndex().data(ChatRole).value<Chat>();
	if (chat)
		return chat.contacts();

	ContactSet contacts = ContactSet();
	Buddy buddy = ChatsTree->currentIndex().data(BuddyRole).value<Buddy>();
	if (buddy)
	{
		foreach (const Contact &contact, buddy.contacts())
			contacts += contact;
	}
	return contacts;
}

Chat HistoryWindow::selectedChat() const
{
	Chat chat = ChatsTree->currentIndex().data(ChatRole).value<Chat>();
	ChatDetails *details = chat.details();
	ChatDetailsAggregate *aggregate = qobject_cast<ChatDetailsAggregate *>(details);
	if (!aggregate)
		return chat;

	QMap<Account, Chat> map;
	foreach (const Chat &chat, aggregate->chats())
		map.insert(chat.chatAccount(), chat);

	Account bestAccount = AccountManager::bestAccount(map.keys());
	return map.value(bestAccount);
}

void HistoryWindow::dateFilteringEnabled(int state)
{
	bool enabled = state == 2;
	FromDateLabel->setEnabled(enabled);
	FromDate->setEnabled(enabled);
	ToDateLabel->setEnabled(enabled);
	ToDate->setEnabled(enabled);

	if (enabled)
	{
		Search.setFromDate(FromDate->date());
		Search.setToDate(ToDate->date());
		updateData();
	}
	else
	{
		Search.setFromDate(QDate());
		Search.setToDate(QDate());
		updateData();
	}
}
