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
#include "chat/buddy-chat-manager.h"
#include "chat/chat.h"
#include "chat/chat-details-buddy.h"
#include "chat/type/chat-type-manager.h"
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

	Chat buddyChat = BuddyChatManager::instance()->buddyChat(chat);
	if (!buddyChat)
	{
		UsedChats.insert(chat);
		assignChat(chat);
		return;
	}

	ChatDetailsBuddy *details = qobject_cast<ChatDetailsBuddy *>(buddyChat.details());
	Q_ASSERT(details);

	foreach (const Chat &usedChat, details->chats())
		UsedChats.insert(usedChat);
	assignChat(buddyChat);
}

void ChatsBuddiesSplitter::assignChat(const Chat &chat)
{
	ChatType *chatType = ChatTypeManager::instance()->chatType(chat.type());
	if (chatType && (chatType->name() == "Contact" || chatType->name() == "Buddy"))
		Buddies.insert(BuddyManager::instance()->byContact(*chat.contacts().begin(), ActionCreateAndAdd));
	else
		Chats.insert(chat);
}

QSet<Chat> ChatsBuddiesSplitter::chats() const
{
	return Chats;
}

QSet<Buddy> ChatsBuddiesSplitter::buddies() const
{
	return Buddies;
}
