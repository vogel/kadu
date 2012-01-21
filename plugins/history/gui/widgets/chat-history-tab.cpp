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
#include "gui/widgets/filter-widget.h"
#include "gui/widgets/filtered-tree-view.h"
#include "gui/widgets/talkable-delegate-configuration.h"
#include "gui/widgets/talkable-tree-view.h"
#include "model/merged-proxy-model-factory.h"
#include "model/model-chain.h"
#include "talkable/filter/name-talkable-filter.h"
#include "talkable/model/talkable-proxy-model.h"

#include "gui/widgets/timeline-chat-messages-view.h"
#include "model/dates-model-item.h"
#include "search/history-search-parameters.h"
#include "chats-buddies-splitter.h"
#include "history.h"

#include "chat-history-tab.h"

ChatHistoryTab::ChatHistoryTab(QWidget *parent) :
		HistoryTab(true, parent), ChatsFutureWatcher(0)
{
	createGui();
}

ChatHistoryTab::~ChatHistoryTab()
{
}

void ChatHistoryTab::createTreeView(QWidget *parent)
{
	FilteredTreeView *chatsTalkableWidget = new FilteredTreeView(FilteredTreeView::FilterAtTop, parent);

	chatsTalkableWidget->filterWidget()->setAutoVisibility(false);
	chatsTalkableWidget->filterWidget()->setLabel(tr("Filter") + ":");

	ChatsTalkableTree = new TalkableTreeView(chatsTalkableWidget);
	ChatsTalkableTree->setAlternatingRowColors(true);
	ChatsTalkableTree->setContextMenuEnabled(true);
	ChatsTalkableTree->setSelectionMode(QAbstractItemView::SingleSelection);
	ChatsTalkableTree->setUseConfigurationColors(true);
	ChatsTalkableTree->delegateConfiguration().setShowMessagePixmap(false);

	QString style;
	style.append("QTreeView::branch:has-siblings:!adjoins-item { border-image: none; image: none }");
	style.append("QTreeView::branch:has-siblings:adjoins-item { border-image: none; image: none }");
	style.append("QTreeView::branch:has-childres:!has-siblings:adjoins-item { border-image: none; image: none }");
	ChatsTalkableTree->setStyleSheet(style);
	ChatsTalkableTree->viewport()->setStyleSheet(style);

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
	connect(ChatsTalkableTree, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showChatsPopupMenu()));
	ChatsTalkableTree->setContextMenuPolicy(Qt::CustomContextMenu);

	chatsTalkableWidget->setView(ChatsTalkableTree);
}

void ChatHistoryTab::displayChat(const Chat &chat, bool force)
{
	if (!force && CurrentChat == chat)
		return;

	timelineView()->messagesView()->setChat(chat);

	CurrentChat = chat;
	setFutureDates(History::instance()->datesForChat(CurrentChat));
}

void ChatHistoryTab::displayAggregateChat(const Chat &chat, bool force)
{
	const Chat &agrregate = AggregateChatManager::instance()->aggregateChat(chat);

	displayChat(agrregate ? agrregate : chat, force);
}

void ChatHistoryTab::showChatsPopupMenu()
{
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
	displayChat(Chat::null, false);
}

void ChatHistoryTab::currentChatChanged(const Talkable &talkable)
{
	switch (talkable.type())
	{
		case Talkable::ItemChat:
		{
			displayAggregateChat(talkable.toChat(), false);
			break;
		}
		case Talkable::ItemBuddy:
		{
			BuddySet buddies;
			buddies.insert(talkable.toBuddy());
			displayAggregateChat(ChatManager::instance()->findChat(buddies, true), false);
			break;
		}
		case Talkable::ItemContact:
		{
			ContactSet contacts;
			contacts.insert(talkable.toContact());
			displayChat(ChatManager::instance()->findChat(contacts, true), false);
			break;
		}
		default:
			displayChat(Chat::null, false);
			break;
	}
}

void ChatHistoryTab::displayForDate(const QDate &date)
{
	setFutureMessages(History::instance()->messages(CurrentChat, date));
}

void ChatHistoryTab::removeEntriesPerDate(const QDate &date)
{
	if (CurrentChat)
	{
		History::instance()->currentStorage()->clearChatHistory(CurrentChat, date);
		displayChat(CurrentChat, true);
	}
}

void ChatHistoryTab::futureChatsAvailable()
{
	hideTabWaitOverlay();

	if (!ChatsFutureWatcher)
		return;

	ChatsBuddiesSplitter chatsBuddies(ChatsFutureWatcher->result());

	ChatsModel->setChats(chatsBuddies.chats());
	ChatsBuddiesModel->setBuddyList(chatsBuddies.buddies());

	ChatsFutureWatcher->deleteLater();
	ChatsFutureWatcher = 0;

	doSelectChat();
}

void ChatHistoryTab::futureChatsCanceled()
{
	hideTabWaitOverlay();

	if (!ChatsFutureWatcher)
		return;

	ChatsFutureWatcher->deleteLater();
	ChatsFutureWatcher = 0;
}

void ChatHistoryTab::updateData()
{
	if (ChatsFutureWatcher)
		delete ChatsFutureWatcher;

	QFuture<QVector<Chat> > futureChats = History::instance()->chatsList();
	ChatsFutureWatcher = new QFutureWatcher<QVector<Chat> >();
	connect(ChatsFutureWatcher, SIGNAL(finished()), this, SLOT(futureChatsAvailable()));
	connect(ChatsFutureWatcher, SIGNAL(canceled()), this, SLOT(futureChatsCanceled()));

	ChatsFutureWatcher->setFuture(futureChats);

	showTabWaitOverlay();
}

void ChatHistoryTab::doSelectChat()
{
	if (!ChatToSelect)
		return;

	QModelIndexList indexesToSelect;

	if (ChatToSelect.contacts().size() == 1)
		indexesToSelect = ChatsModelChain->indexListForValue(ChatToSelect.contacts().begin()->ownerBuddy());
	else if (ChatToSelect.contacts().size() > 1)
		indexesToSelect = ChatsModelChain->indexListForValue(ChatToSelect);

	if (1 == indexesToSelect.size())
	{
		ChatsTalkableTree->selectionModel()->select(indexesToSelect.at(0), QItemSelectionModel::ClearAndSelect);
		ChatsTalkableTree->scrollTo(indexesToSelect.at(0), QAbstractItemView::EnsureVisible);
		displayChat(ChatToSelect, false);
	}
	else
		ChatsTalkableTree->selectionModel()->select(QModelIndex(), QItemSelectionModel::ClearAndSelect);

	ChatToSelect = Chat::null;
}

void ChatHistoryTab::selectChat(const Chat &chat)
{
	ChatToSelect = chat;

	if (!ChatsFutureWatcher)
		doSelectChat();
}
