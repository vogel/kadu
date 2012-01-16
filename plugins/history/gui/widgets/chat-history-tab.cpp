/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QMenu>
#include <QtGui/QSplitter>
#include <QtGui/QVBoxLayout>

#include "buddies/buddy-manager.h"
#include "buddies/model/buddy-list-model.h"
#include "chat/aggregate-chat-manager.h"
#include "chat/chat-details-aggregate.h"
#include "chat/chat-manager.h"
#include "chat/model/chats-list-model.h"
#include "gui/widgets/chat-messages-view.h"
#include "gui/widgets/filtered-tree-view.h"
#include "gui/widgets/talkable-delegate-configuration.h"
#include "gui/widgets/talkable-tree-view.h"
#include "model/merged-proxy-model-factory.h"
#include "model/model-chain.h"
#include "talkable/filter/name-talkable-filter.h"
#include "talkable/model/talkable-proxy-model.h"

#include "gui/widgets/timeline-chat-messages-view.h"
#include "model/dates-model-item.h"
#include "model/history-dates-model.h"
#include "search/history-search-parameters.h"
#include "history.h"

#include "chat-history-tab.h"

ChatHistoryTab::ChatHistoryTab(QWidget *parent) :
		HistoryTab(parent)
{
	MyChatDatesModel = new HistoryDatesModel(true, this);

	ChatDetailsPopupMenu = new QMenu(this);
	ChatDetailsPopupMenu->addAction(KaduIcon("kadu_icons/clear-history").icon(), tr("&Remove entries"),
	                                this, SLOT(removeChatEntriesPerDate()));

	createGui();
}

ChatHistoryTab::~ChatHistoryTab()
{
}

void ChatHistoryTab::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(0);

	QSplitter *splitter = new QSplitter(Qt::Horizontal, this);

	FilteredTreeView *chatsTalkableWidget = new FilteredTreeView(FilteredTreeView::FilterAtTop, splitter);
	chatsTalkableWidget->setFilterAutoVisibility(false);

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

	TimelineChatView = new TimelineChatMessagesView(splitter);

	TimelineChatView->timeline()->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(TimelineChatView->timeline(), SIGNAL(customContextMenuRequested(QPoint)),
	        this, SLOT(showChatDetailsPopupMenu(QPoint)));

	QList<int> sizes;
	sizes.append(150);
	sizes.append(300);
	splitter->setSizes(sizes);

	layout->addWidget(splitter);
}

void ChatHistoryTab::updateData()
{
	QSet<Chat> usedChats;
	QVector<Chat> chatsList = History::instance()->chatsList(HistorySearchParameters());

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

			if (aggregate.contacts().size() > 1)
				conferenceChats.append(aggregate);
			else if (1 == aggregate.contacts().size())
				buddies.append(BuddyManager::instance()->byContact(*aggregate.contacts().begin(), ActionCreateAndAdd));
		}
		else
		{
			usedChats.insert(chat);
			if (chat.contacts().size() > 1)
				conferenceChats.append(chat);
			else if (1 == chat.contacts().size())
				buddies.append(BuddyManager::instance()->byContact(*chat.contacts().begin(), ActionCreateAndAdd));
		}
	}

	ChatsModel->setChats(conferenceChats);
	ChatsBuddiesModel->setBuddyList(buddies);
}

void ChatHistoryTab::chatActivated(const Chat &chat)
{
	QModelIndex selectedIndex = TimelineChatView->timeline()->selectionModel()
	        ? TimelineChatView->timeline()->selectionModel()->currentIndex()
	        : QModelIndex();
	QDate date = selectedIndex.data(DateRole).toDate();

	QVector<DatesModelItem> chatDates = History::instance()->datesForChat(chat, HistorySearchParameters());
	MyChatDatesModel->setDates(chatDates);

	QModelIndex select = MyChatDatesModel->indexForDate(date);
	if (!select.isValid())
	{
		int lastRow = MyChatDatesModel->rowCount(QModelIndex()) - 1;
		if (lastRow >= 0)
			select = MyChatDatesModel->index(lastRow);
	}

	TimelineChatView->timeline()->setModel(MyChatDatesModel);

	connect(TimelineChatView->timeline()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
	        this, SLOT(chatDateCurrentChanged(QModelIndex,QModelIndex)), Qt::UniqueConnection);

	TimelineChatView->timeline()->selectionModel()->setCurrentIndex(select, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}

void ChatHistoryTab::selectChat(const Chat &chat)
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

void ChatHistoryTab::currentChatChanged(const Talkable &talkable)
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

void ChatHistoryTab::removeChatEntriesPerDate()
{
	QDate date = TimelineChatView->timeline()->currentIndex().data(DateRole).value<QDate>();
	if (!date.isValid())
		return;

	const Chat &chat = ChatsTalkableTree->actionContext()->chat();
	if (chat)
	{
		History::instance()->currentStorage()->clearChatHistory(chat, date);
		chatActivated(chat);
	}
}

void ChatHistoryTab::chatDateCurrentChanged(const QModelIndex &current, const QModelIndex &previous)
{
	if (current == previous)
		return;

	QDate date = current.data(DateRole).value<QDate>();

	TimelineChatView->messagesView()->setUpdatesEnabled(false);

	Chat chat = ChatsTalkableTree->actionContext()->chat();
	QVector<Message> messages;
	if (chat && date.isValid())
		messages = History::instance()->messages(chat, date);
	TimelineChatView->messagesView()->setChat(chat);
	TimelineChatView->messagesView()->clearMessages();
	TimelineChatView->messagesView()->appendMessages(messages);

	TimelineChatView->messagesView()->setUpdatesEnabled(true);
}

void ChatHistoryTab::showChatsPopupMenu(const QPoint &pos)
{
	Q_UNUSED(pos)

	QScopedPointer<QMenu> menu;

	menu.reset(TalkableMenuManager::instance()->menu(this, ChatsTalkableTree->actionContext()));
	menu->addSeparator();
	menu->addAction(KaduIcon("kadu_icons/clear-history").icon(),
			tr("&Clear Chat History"), this, SLOT(clearChatHistory()));

	menu->exec(QCursor::pos());
}

void ChatHistoryTab::showChatDetailsPopupMenu(const QPoint &pos)
{
	QDate date = TimelineChatView->timeline()->indexAt(pos).data(DateRole).value<QDate>();
	if (!date.isValid())
		return;

	const Chat &chat = ChatsTalkableTree->actionContext()->chat();
	if (chat)
		ChatDetailsPopupMenu->exec(QCursor::pos());
}

void ChatHistoryTab::clearChatHistory()
{
	if (!ChatsTalkableTree->actionContext())
		return;

	const Chat &chat = ChatsTalkableTree->actionContext()->chat();
	if (!chat)
		return;

	History::instance()->currentStorage()->clearChatHistory(chat);
	updateData();
}

void ChatHistoryTab::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == QKeySequence::Copy && !TimelineChatView->messagesView()->selectedText().isEmpty())
		// Do not use triggerPageAction(), see bug #2345.
		TimelineChatView->messagesView()->pageAction(QWebPage::Copy)->trigger();
	else
		HistoryTab::keyPressEvent(event);
}
