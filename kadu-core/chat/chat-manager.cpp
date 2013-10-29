/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "chat/chat-details-contact-set.h"
#include "chat/chat-details-contact.h"
#include "chat/type/chat-type-manager.h"
#include "core/core.h"
#include "message/unread-message-repository.h"

#include "chat-manager.h"

ChatManager * ChatManager::Instance = 0;

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns singleton instance of ChatManager.
 * @return singleton instance of ChatManager
 */
ChatManager *  ChatManager::instance()
{
	if (0 == Instance)
	{
		Instance = new ChatManager();
		Instance->init();
	}

	return Instance;
}

ChatManager::ChatManager()
{
}

ChatManager::~ChatManager()
{
	disconnect(Core::instance()->unreadMessageRepository(), 0, this, 0);

	foreach (const Message &message, Core::instance()->unreadMessageRepository()->allUnreadMessages())
		unreadMessageRemoved(message);
}

void ChatManager::init()
{
	foreach (const Message &message, Core::instance()->unreadMessageRepository()->allUnreadMessages())
		unreadMessageAdded(message);

	connect(Core::instance()->unreadMessageRepository(), SIGNAL(unreadMessageAdded(Message)),
	        this, SLOT(unreadMessageAdded(Message)));
	connect(Core::instance()->unreadMessageRepository(), SIGNAL(unreadMessageRemoved(Message)),
	        this, SLOT(unreadMessageRemoved(Message)));
}

void ChatManager::itemAboutToBeRegistered(Chat item)
{
	connect(item, SIGNAL(updated()), this, SLOT(chatDataUpdated()));
	connect(item, SIGNAL(opened()), this, SLOT(chatOpened()));
	connect(item, SIGNAL(closed()), this, SLOT(chatClosed()));

	emit chatAboutToBeAdded(item);
}

void ChatManager::itemRegistered(Chat item)
{
	emit chatAdded(item);
}

void ChatManager::itemAboutToBeUnregisterd(Chat item)
{
	disconnect(item, 0, this, 0);

	emit chatAboutToBeRemoved(item);
}

void ChatManager::itemUnregistered(Chat item)
{
	emit chatRemoved(item);
}

QVector<Chat> ChatManager::chats(const Account &account)
{
	QMutexLocker locker(&mutex());

	ensureLoaded();

	QVector<Chat> chats;

	if (account.isNull())
		return chats;

	foreach (const Chat &chat, allItems())
		if (account == chat.chatAccount())
			chats.append(chat);

	return chats;
}

Chat ChatManager::byDisplay(const QString &display)
{
	QMutexLocker locker(&mutex());

	ensureLoaded();

	if (display.isEmpty())
		return Chat::null;

	foreach (const Chat &chat, allItems())
		if (display == chat.display())
			return chat;

	return Chat::null;
}

void ChatManager::chatDataUpdated()
{
	Chat chat(sender());
	if (!chat.isNull())
		emit chatUpdated(chat);
}

void ChatManager::chatOpened()
{
	Chat chat(sender());
	if (!chat.isNull())
		emit chatOpened(chat);
}

void ChatManager::chatClosed()
{
	Chat chat(sender());
	if (!chat.isNull())
		emit chatClosed(chat);
}

void ChatManager::unreadMessageAdded(const Message &message)
{
	const Chat &chat = message.messageChat();
	chat.setUnreadMessagesCount(chat.unreadMessagesCount() + 1);
}

void ChatManager::unreadMessageRemoved(const Message &message)
{
	const Chat &chat = message.messageChat();
	quint16 unreadMessagesCount = chat.unreadMessagesCount();
	if (unreadMessagesCount > 0)
		chat.setUnreadMessagesCount(unreadMessagesCount - 1);
}

#include "moc_chat-manager.cpp"
