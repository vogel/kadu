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

#include "buddies/buddy-manager.h"
#include "chat/chat.h"
#include "chat/type/chat-type-manager.h"
#include "misc/misc.h"

#include "chat-details-room.h"

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Creates empty ChatDetailsRoom object.
 * @param chatData Chat object that will be decribed by this object
 *
 * Creates empty ChatDetailsRoom object assigned to chatData object.
 */
ChatDetailsRoom::ChatDetailsRoom(ChatShared *chatData) :
		ChatDetails(chatData)
{
}

ChatDetailsRoom::~ChatDetailsRoom()
{
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Loads ChatDetailsRoom object from storage.
 *
 * Loads ChatDetailsRoom object from the same storage assigned Chat object is
 * using. This loads room name of contacts from 'Chat' node
 */
void ChatDetailsRoom::load()
{
	if (!isValidStorage())
		return;

	ChatDetails::load();

	Server = loadValue<QString>("Server");
	RoomName = loadValue<QString>("RoomName");
	Password = pwHash(loadValue<QString>("Password"));
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Stores ChatDetailsRoom object to storage.
 *
 * Stores ChatDetailsRoom object to the same storage assigned Chat object is
 * using. This stores room name into 'Chat' subnode.
 */
void ChatDetailsRoom::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	storeValue("Server", Server);
	storeValue("RoomName", RoomName);
	storeValue("Password", pwHash(Password));
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns true if room name is not empty.
 * @return true if room name is not empty
 *
 * Returns true if assigned room name is not empty is not empty.
 */
bool ChatDetailsRoom::shouldStore()
{
	ensureLoaded();

	return StorableObject::shouldStore() && !RoomName.isEmpty();
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns type of this chat - 'Room'.
 * @return 'Room' ChatType object
 *
 * Returns type of this chat - 'Room'.
 */
ChatType * ChatDetailsRoom::type() const
{
	return ChatTypeManager::instance()->chatType("Room");
}

void ChatDetailsRoom::setServer(const QString &server)
{
	Server = server;
}

QString ChatDetailsRoom::server() const
{
	return Server;
}

void ChatDetailsRoom::setRoomName(const QString &roomName)
{
	RoomName = roomName;
}

QString ChatDetailsRoom::roomName() const
{
	return RoomName;
}

void ChatDetailsRoom::setPassword(const QString &password)
{
	Password = password;
}

QString ChatDetailsRoom::password() const
{
	return Password;
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns name of this chat.
 * @return name of this chat
 *
 * Returns name of this chat (which is name of room).
 */
QString ChatDetailsRoom::name() const
{
	return RoomName;
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Assigns contact set to this chat.
 * @param contacts contact set to assign
 *
 * Assigns contact set to this chat.
 */
void ChatDetailsRoom::setContacts(const ContactSet &contacts)
{
	ensureLoaded();

	Contacts = contacts;
}

void ChatDetailsRoom::addContact(const Contact &contact)
{
	ensureLoaded();

	if (Contacts.contains(contact))
		return;

	Contacts.insert(contact);
	emit contactAdded(contact);
}

void ChatDetailsRoom::removeContact(const Contact &contact)
{
	ensureLoaded();

	if (!Contacts.contains(contact))
		return;

	Contacts.remove(contact);
	emit contactAdded(contact);
}
