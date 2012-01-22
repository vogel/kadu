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
#include <qdbuspendingreply.h>

#include "buddies/buddy-manager.h"
#include "chat/aggregate-chat-manager.h"
#include "chat/chat.h"
#include "chat/chat-details-aggregate.h"
#include "contacts/contact-set.h"
#include "talkable/talkable.h"

#include "chats-buddies-splitter.h"

ChatsBuddiesSplitter::ChatsBuddiesSplitter(QVector<Talkable> talkables)
{
	// we ignore contacts
	foreach (const Talkable &talkable, talkables)
		if (talkable.isValidChat())
			processChat(talkable.toChat());
		else if (talkable.isValidBuddy())
			Buddies.insert(talkable.toBuddy());
}

void ChatsBuddiesSplitter::processChat(const Chat &chat)
{
	if (UsedChats.contains(chat))
		return;

	Chat aggregate = AggregateChatManager::instance()->aggregateChat(chat);
	if (!aggregate)
	{
		UsedChats.insert(chat);
		assignChat(chat);
		return;
	}

	ChatDetailsAggregate *details = qobject_cast<ChatDetailsAggregate *>(aggregate.details());
	Q_ASSERT(details);

	foreach (const Chat &usedChat, details->chats())
		UsedChats.insert(usedChat);
	assignChat(aggregate);
}

void ChatsBuddiesSplitter::assignChat(const Chat &chat)
{
	if (chat.contacts().size() > 1)
		Chats.insert(chat);
	else if (1 == chat.contacts().size())
		Buddies.insert(BuddyManager::instance()->byContact(*chat.contacts().begin(), ActionCreateAndAdd));
}

QSet<Chat> ChatsBuddiesSplitter::chats() const
{
	return Chats;
}

QSet<Buddy> ChatsBuddiesSplitter::buddies() const
{
	return Buddies;
}
