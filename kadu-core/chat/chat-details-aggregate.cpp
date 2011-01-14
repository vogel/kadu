/*
 * %kadu copyright begin%
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

#include "chat/chat.h"
#include "chat/type/chat-type-manager.h"
#include "contacts/contact-set.h"

#include "chat-details-aggregate.h"

/**
 * @short Creates empty ChatDetailsAggregate object.
 * @param chatData Chat object that will be decribed by this object
 *
 * Creates empty ChatDetailsAggregate object assigned to chatData object.
 */
ChatDetailsAggregate::ChatDetailsAggregate(ChatShared *chatData) :
		ChatDetails(chatData)
{
}

ChatDetailsAggregate::~ChatDetailsAggregate()
{
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Return false, this class should never be stored.
 * @return false, this class should never be stored
 *
 * Return false, this class should never be stored.
 */
bool ChatDetailsAggregate::shouldStore()
{
	return false;
}

/**
 * @short Returns type of this chat - 'Aggregate'.
 * @return 'Aggregate' ChatType object
 *
 * Returns type of this chat - 'Aggregate'.
 */
ChatType * ChatDetailsAggregate::type() const
{
	return ChatTypeManager::instance()->chatType("Aggregate");
}

/**
 * @short Returns set of contracts of first chat in this aggregate chat or empty set.
 * @return set of contracts of first chat in this aggregate chat or empty set
 *
 * Returns set of contracts of first chat in this aggregate chat or empty set
 * no chats are aggreagated.
 */
ContactSet ChatDetailsAggregate::contacts() const
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
QString ChatDetailsAggregate::name() const
{
	return Chats.isEmpty() ? QString() : Chats.at(0).name();
}

/**
 * @short Assigns aggregate chats to this chat.
 * @param chats chats to be assigned
 *
 * Assigns aggregate chats to this chat.
 */
void ChatDetailsAggregate::setChats(QList<Chat> chats)
{
	Chats = chats;
}

/**
 * @short Returns assigned aggregate chats to this chat.
 * @return assigned aggregate chats to this chat
 *
 * Returns assigned aggregate chats to this chat.
 */
QList<Chat> ChatDetailsAggregate::chats()
{
	return Chats;
}
