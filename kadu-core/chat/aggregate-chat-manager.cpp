/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account-manager.h"
#include "buddies/buddy-set.h"
#include "buddies/buddy-shared.h"
#include "protocols/protocol.h"

#include "chat/aggregate-chat-manager.h"
#include "chat/chat-manager.h"
#include "chat-details-aggregate.h"

AggregateChatManager * AggregateChatManager::Instance = 0;

AggregateChatManager * AggregateChatManager::instance()
{
	if (!Instance)
		Instance = new AggregateChatManager();

	return Instance;
}


AggregateChatManager::AggregateChatManager()
{
	connect(ChatManager::instance(), SIGNAL(chatAdded(Chat)), this, SLOT(chatAdded(Chat)));
	connect(ChatManager::instance(), SIGNAL(chatRemoved(Chat)), this, SLOT(chatRemoved(Chat)));

	foreach (const Chat &chat, ChatManager::instance()->allItems())
		chatAdded(chat);
}

AggregateChatManager::~AggregateChatManager()
{
	disconnect(ChatManager::instance(), SIGNAL(chatAdded(Chat)), this, SLOT(chatAdded(Chat)));
	disconnect(ChatManager::instance(), SIGNAL(chatRemoved(Chat)), this, SLOT(chatRemoved(Chat)));

	foreach (const Chat &chat, ChatManager::instance()->allItems())
		chatRemoved(chat);
}

void AggregateChatManager::chatAdded(const Chat &chat)
{
	BuddySet buddies = chat.contacts().toBuddySet();

	if (!AggregateChats.contains(buddies))
	{
		QList<Chat> chats;
		chats.append(chat);
		AggregateChats.insert(buddies, chats);
	}
	else
		AggregateChats[buddies].append(chat);
}

void AggregateChatManager::chatRemoved(const Chat &chat)
{
	BuddySet buddies = chat.contacts().toBuddySet();

	if (!AggregateChats.contains(buddies))
		return;

	AggregateChats[buddies].removeAll(chat);
	if (AggregateChats.value(buddies).isEmpty())
		AggregateChats.remove(buddies);
}

Chat AggregateChatManager::aggregateChat(const Chat &chat)
{
	return aggregateChat(chat.contacts().toBuddySet());
}

/**
 * @param buddies set of buddies
 * @short Makes chat object that aggregates all chats for given buddy set.
 * @return chat object that aggregates all chats for given buddy set
 *
 * This method will create and return new chat of 'Aggregate' type that
 * contains all chats (for different accounts) for given set of buddies.
 */
Chat AggregateChatManager::aggregateChat(const BuddySet &buddies)
{
	if (!AggregateChats.contains(buddies))
		return Chat::null;

	QList<Chat> chats = AggregateChats.value(buddies);
	if (chats.count() <= 1)
		return Chat::null;

	Chat result = Chat::create();
	result.setType(chats.at(0).type());

	ChatDetailsAggregate *details = new ChatDetailsAggregate(result);
	details->setChats(chats);
	result.setDetails(details);

	return result;
}
