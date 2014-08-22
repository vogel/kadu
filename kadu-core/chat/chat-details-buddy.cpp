/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "chat/chat.h"
#include "chat/type/chat-type-manager.h"
#include "contacts/contact-set.h"

#include "chat-details-buddy.h"

/**
 * @short Creates empty ChatDetailsBuddy object.
 * @param chatData Chat object that will be decribed by this object
 *
 * Creates empty ChatDetailsBuddy object assigned to chatData object.
 */
ChatDetailsBuddy::ChatDetailsBuddy(ChatShared *chatData) :
		ChatDetails(chatData)
{
}

ChatDetailsBuddy::~ChatDetailsBuddy()
{
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Return false, this class should never be stored.
 * @return false, this class should never be stored
 *
 * Return false, this class should never be stored.
 */
bool ChatDetailsBuddy::shouldStore()
{
	return false;
}

/**
 * @short Returns type of this chat - 'Buddy'.
 * @return 'Buddy' ChatType object
 *
 * Returns type of this chat - 'Buddy'.
 */
ChatType * ChatDetailsBuddy::type() const
{
	return ChatTypeManager::instance()->chatType("Buddy");
}

/**
 * @short Returns set of contracts of first chat in this aggregate chat or empty set.
 * @return set of contracts of first chat in this aggregate chat or empty set
 *
 * Returns set of contracts of first chat in this aggregate chat or empty set
 * no chats are aggreagated.
 */
ContactSet ChatDetailsBuddy::contacts() const
{
	return Chats.isEmpty() ? ContactSet() : Chats.at(0).contacts();
}

/**
 * @short Returns name of first chat in this aggregate chat or empty string.
 * @return name of first chat in this aggregate chat or empty string
 *
 * Returns name of first chat in this aggregate chat or empty string if
 * no chats are aggreagated.
 */
QString ChatDetailsBuddy::name() const
{
	return Chats.isEmpty() ? MyBuddy.isEmpty() ? QString() : MyBuddy.display() : Chats.at(0).name();
}

bool ChatDetailsBuddy::isConnected() const
{
	return false;
}

void ChatDetailsBuddy::setBuddy(const Buddy &buddy)
{
	MyBuddy = buddy;
}

Buddy ChatDetailsBuddy::buddy() const
{
	return MyBuddy;
}

/**
 * @short Assigns aggregate chats to this chat.
 * @param chats chats to be assigned
 *
 * Assigns aggregate chats to this chat.
 */
void ChatDetailsBuddy::setChats(const QVector<Chat> &chats)
{
	Chats = chats;
}

/**
 * @short Returns assigned aggregate chats to this chat.
 * @return assigned aggregate chats to this chat
 *
 * Returns assigned aggregate chats to this chat.
 */
const QVector<Chat> & ChatDetailsBuddy::chats() const
{
	return Chats;
}

void ChatDetailsBuddy::addChat(const Chat &chat)
{
	if (!Chats.contains(chat))
		Chats.append(chat);
}

void ChatDetailsBuddy::removeChat(const Chat &chat)
{
	int indexOf = Chats.indexOf(chat);
	if (indexOf >= 0)
		Chats.remove(indexOf);
}

#include "moc_chat-details-buddy.cpp"
