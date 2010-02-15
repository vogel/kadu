/*
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "chat/aggregate-chat-builder.h"
#include "chat/chat-manager.h"
#include "chat-details-aggregate.h"

/**
 * @param buddies set of buddies
 * @short Makes chat object that aggregates all chats for given buddy set.
 * @return chat object that aggregates all chats for given buddy set
 *
 * This method will create and return new chat of 'Aggregate' type that
 * contains all chats (for different accounts) for given set of buddies.
 */
Chat AggregateChatBuilder::buildAggregateChat(BuddySet buddies)
{
	QList<Chat> chats;
	foreach (Chat chat, ChatManager::instance()->allItems())
		if (chat.contacts().toBuddySet() == buddies)
			chats.append(chat);

	if (chats.size() <= 1)
		return Chat::null;

	Chat result = Chat::create();
	result.setType(chats[0].type());

	ChatDetailsAggregate *details = new ChatDetailsAggregate(result);
	details->setChats(chats);
	result.setDetails(details);

	return result;
}
