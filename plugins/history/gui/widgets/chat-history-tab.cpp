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
#include "gui/widgets/talkable-menu-manager.h"
#include "gui/widgets/talkable-tree-view.h"
#include "icons/kadu-icon.h"
#include "model/merged-proxy-model-factory.h"
#include "model/model-chain.h"
#include "talkable/filter/name-talkable-filter.h"
#include "talkable/model/talkable-proxy-model.h"

#include "gui/widgets/timeline-chat-messages-view.h"
#include "model/dates-model-item.h"
#include "search/history-search-parameters.h"
#include "storage/history-storage.h"
#include "chats-buddies-splitter.h"

#include "chat-history-tab.h"

ChatHistoryTab::ChatHistoryTab(QWidget *parent) :
		HistoryTab(true, parent)
{
	setUpGui();
}

ChatHistoryTab::~ChatHistoryTab()
{
}

void ChatHistoryTab::setUpGui()
{
	connect(talkableTree(), SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showChatsPopupMenu()));
	talkableTree()->setContextMenuPolicy(Qt::CustomContextMenu);
}

void ChatHistoryTab::displayChat(const Chat &chat, bool force)
{
	if (!force && CurrentChat == chat)
		return;

	timelineView()->messagesView()->setChat(chat);

	CurrentChat = chat;

	if (historyStorage())
		setFutureDates(historyStorage()->chatDates(CurrentChat));
	else
		setDates(QVector<DatesModelItem>());
}

void ChatHistoryTab::displayAggregateChat(const Chat &chat, bool force)
{
	const Chat &agrregate = AggregateChatManager::instance()->aggregateChat(chat);

	displayChat(agrregate ? agrregate : chat, force);
}

void ChatHistoryTab::showChatsPopupMenu()
{
	QScopedPointer<QMenu> menu;

	menu.reset(TalkableMenuManager::instance()->menu(this, talkableTree()->actionContext()));
	menu->addSeparator();
	menu->addAction(KaduIcon("kadu_icons/clear-history").icon(),
			tr("&Clear Chat History"), this, SLOT(clearChatHistory()));

	menu->exec(QCursor::pos());
}

void ChatHistoryTab::clearChatHistory()
{
	if (!talkableTree()->actionContext())
		return;

	const Chat &chat = talkableTree()->actionContext()->chat();
	if (!chat)
		return;

	if (historyStorage())
	{
		historyStorage()->clearChatHistory(chat);
		updateData();
	}

	displayChat(Chat::null, false);
}

void ChatHistoryTab::currentTalkableChanged(const Talkable &talkable)
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

void ChatHistoryTab::talkablesAvailable()
{
	if (!ChatToSelect)
		return;

	QModelIndexList indexesToSelect;

	if (ChatToSelect.contacts().size() == 1)
		indexesToSelect = modelChain()->indexListForValue(ChatToSelect.contacts().begin()->ownerBuddy());
	else if (ChatToSelect.contacts().size() > 1)
		indexesToSelect = modelChain()->indexListForValue(ChatToSelect);

	if (1 == indexesToSelect.size())
	{
		talkableTree()->selectionModel()->select(indexesToSelect.at(0), QItemSelectionModel::ClearAndSelect);
		talkableTree()->scrollTo(indexesToSelect.at(0), QAbstractItemView::EnsureVisible);
		displayChat(ChatToSelect, false);
	}
	else
		talkableTree()->selectionModel()->select(QModelIndex(), QItemSelectionModel::ClearAndSelect);

	ChatToSelect = Chat::null;
}

void ChatHistoryTab::displayForDate(const QDate &date)
{
	if (historyStorage())
		setFutureMessages(historyStorage()->messages(CurrentChat, date));
	else
		setMessages(QVector<Message>());
}

void ChatHistoryTab::removeEntriesPerDate(const QDate &date)
{
	if (CurrentChat && historyStorage())
	{
		historyStorage()->clearChatHistory(CurrentChat, date);
		displayChat(CurrentChat, true);
	}
}

void ChatHistoryTab::updateData()
{
	setMessages(QVector<Message>());

	if (!historyStorage())
	{
		setTalkables(QVector<Talkable>());
		displayChat(Chat::null, false);
		return;
	}

	setFutureTalkables(historyStorage()->chats());
}

void ChatHistoryTab::selectChat(const Chat &chat)
{
	ChatToSelect = chat;
}
