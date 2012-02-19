/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "protocols/protocol.h"

#include "chat/buddy-chat-manager.h"
#include "chat/chat-manager.h"
#include "chat-details-buddy.h"

BuddyChatManager * BuddyChatManager::Instance = 0;

BuddyChatManager * BuddyChatManager::instance()
{
	if (!Instance)
	{
		Instance = new BuddyChatManager();
		Instance->init();
	}

	return Instance;
}

BuddyChatManager::BuddyChatManager()
{
}

BuddyChatManager::~BuddyChatManager()
{
	disconnect(ChatManager::instance(), SIGNAL(chatAdded(Chat)), this, SLOT(chatAdded(Chat)));
	disconnect(ChatManager::instance(), SIGNAL(chatRemoved(Chat)), this, SLOT(chatRemoved(Chat)));

	foreach (const Chat &chat, ChatManager::instance()->items())
		chatRemoved(chat);
}

void BuddyChatManager::init()
{
	connect(ChatManager::instance(), SIGNAL(chatAdded(Chat)), this, SLOT(chatAdded(Chat)));
	connect(ChatManager::instance(), SIGNAL(chatRemoved(Chat)), this, SLOT(chatRemoved(Chat)));

	foreach (const Chat &chat, ChatManager::instance()->items())
		chatAdded(chat);
}

void BuddyChatManager::chatAdded(const Chat &chat)
{
	BuddySet buddies = chat.contacts().toBuddySet();

	if (!BuddyChats.contains(buddies))
	{
		QVector<Chat> chats;
		chats.append(chat);
		BuddyChats.insert(buddies, chats);
	}
	else
		BuddyChats[buddies].append(chat);
}

void BuddyChatManager::chatRemoved(const Chat &chat)
{
	BuddySet buddies = chat.contacts().toBuddySet();

	if (!BuddyChats.contains(buddies))
		return;

	BuddyChats[buddies].remove(BuddyChats[buddies].indexOf(chat));
	if (BuddyChats.value(buddies).isEmpty())
		BuddyChats.remove(buddies);
}

Chat BuddyChatManager::buddyChat(const Chat &chat)
{
	return buddyChat(chat.contacts().toBuddySet());
}

/**
 * @param buddies set of buddies
 * @short Makes chat object that aggregates all chats for given buddy set.
 * @return chat object that aggregates all chats for given buddy set
 *
 * This method will create and return new chat of 'Buddy' type that
 * contains all chats (for different accounts) for given set of buddies.
 */
Chat BuddyChatManager::buddyChat(const BuddySet &buddies)
{
	if (!BuddyChats.contains(buddies))
		return Chat::null;

	QVector<Chat> chats = BuddyChats.value(buddies);
	if (chats.count() <= 1)
		return Chat::null;

	Chat result = Chat::create();
	result.setType("Buddy");

	ChatDetailsBuddy *details = qobject_cast<ChatDetailsBuddy *>(result.details());
	details->setChats(chats);

	return result;
}
