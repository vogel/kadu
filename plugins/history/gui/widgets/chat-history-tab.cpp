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
#include "chats-buddies-splitter.h"
#include "history.h"

#include "chat-history-tab.h"

ChatHistoryTab::ChatHistoryTab(QWidget *parent) :
		HistoryTab(parent)
{
	MyChatDatesModel = new HistoryDatesModel(true, this);

	createGui();
}

ChatHistoryTab::~ChatHistoryTab()
{
}

void ChatHistoryTab::createTreeView (QWidget *parent)
{
	FilteredTreeView *chatsTalkableWidget = new FilteredTreeView(FilteredTreeView::FilterAtTop, parent);
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
}

void ChatHistoryTab::updateData()
{
	ChatsBuddiesSplitter chatsBuddies(History::instance()->chatsList(HistorySearchParameters()));

	ChatsModel->setChats(chatsBuddies.chats());
	ChatsBuddiesModel->setBuddyList(chatsBuddies.buddies());
}

void ChatHistoryTab::chatActivated(const Chat &chat)
{
	QVector<DatesModelItem> chatDates = History::instance()->datesForChat(chat, HistorySearchParameters());
	MyChatDatesModel->setDates(chatDates);

	QDate date = timelineView()->currentDate();
	QModelIndex select = MyChatDatesModel->indexForDate(date);
	if (!select.isValid())
	{
		int lastRow = MyChatDatesModel->rowCount(QModelIndex()) - 1;
		if (lastRow >= 0)
			select = MyChatDatesModel->index(lastRow);
	}

	timelineView()->timeline()->setModel(MyChatDatesModel);

	connect(timelineView()->timeline()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
	        this, SLOT(chatDateCurrentChanged(QModelIndex,QModelIndex)), Qt::UniqueConnection);

	if (select.isValid())
		timelineView()->timeline()->selectionModel()->setCurrentIndex(select, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
	else
		timelineView()->messagesView()->clearMessages();
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

void ChatHistoryTab::removeEntriesPerDate(const QDate &date)
{
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

	timelineView()->messagesView()->setUpdatesEnabled(false);

	Chat chat = ChatsTalkableTree->actionContext()->chat();
	QVector<Message> messages;
	if (chat && date.isValid())
		messages = History::instance()->messages(chat, date);
	timelineView()->messagesView()->setChat(chat);
	timelineView()->messagesView()->clearMessages();
	timelineView()->messagesView()->appendMessages(messages);

	timelineView()->messagesView()->setUpdatesEnabled(true);
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

void ChatHistoryTab::clearChatHistory()
{
	if (!ChatsTalkableTree->actionContext())
		return;

	const Chat &chat = ChatsTalkableTree->actionContext()->chat();
	if (!chat)
		return;

	History::instance()->currentStorage()->clearChatHistory(chat);
	updateData();
	chatActivated(Chat::null);
}
