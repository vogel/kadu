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

#include <QtCore/QSet>

#include "buddies/buddy-manager.h"
#include "chat/aggregate-chat-manager.h"
#include "chat/chat.h"
#include "chat/chat-details-aggregate.h"
#include "contacts/contact-set.h"

#include "chats-buddies-splitter.h"

ChatsBuddiesSplitter::ChatsBuddiesSplitter(QVector<Chat> chats)
{
	QSet<Chat> usedChats;

	foreach (const Chat &chat, chats)
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
				Chats.append(aggregate);
			else if (1 == aggregate.contacts().size())
				Buddies.append(BuddyManager::instance()->byContact(*aggregate.contacts().begin(), ActionCreateAndAdd));
		}
		else
		{
			usedChats.insert(chat);
			if (chat.contacts().size() > 1)
				Chats.append(chat);
			else if (1 == chat.contacts().size())
				Buddies.append(BuddyManager::instance()->byContact(*chat.contacts().begin(), ActionCreateAndAdd));
		}
	}
}

QVector<Chat> ChatsBuddiesSplitter::chats() const
{
	return Chats;
}

BuddyList ChatsBuddiesSplitter::buddies() const
{
	return Buddies;
}
