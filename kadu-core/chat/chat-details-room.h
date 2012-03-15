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

#ifndef CHAT_DETAILS_ROOM_H
#define CHAT_DETAILS_ROOM_H

#include "buddies/buddy-set.h"
#include "contacts/contact-set.h"
#include "contacts/contact.h"

#include "chat/chat-details.h"

/**
 * @addtogroup Chat
 * @{
 */

/**
 * @class ChatDetailsRoom
 * @author Rafal 'Vogel' Malinowski
 * @short Chat data specyfic to 'Room' chat type.
 *
 * Class contains dynamic set of Contact objects and a room's name. Chat name is set to this room's name.
 */
class ChatDetailsRoom : public ChatDetails
{
	Q_OBJECT

	QString Server;
	QString RoomName;
	QString Password;

	ContactSet Contacts;

protected:
	virtual void load();
	virtual void store();
	virtual bool shouldStore();

public:
	explicit ChatDetailsRoom(ChatShared *chatData);
	virtual ~ChatDetailsRoom();

	virtual ChatType * type() const;
	virtual ContactSet contacts() const { return Contacts; }
	virtual QString name() const;

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Set server for this chat.
	 * @param server new server for this chat
	 */
	void setServer(const QString &server);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Get current server for this chat.
	 * @return current server for this chat
	 */
	QString server() const;

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Set room name for this chat.
	 * @param roomName new room name for this chat
	 */
	void setRoomName(const QString &roomName);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Get current name of room for this chat.
	 * @return current name of room for this chat
	 */
	QString roomName() const;

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Set password for this chat.
	 * @param password new password for this chat
	 */
	void setPassword(const QString &password);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Get current password for this chat.
	 * @return current password for this chat
	 */
	QString password() const;

	void setContacts(const ContactSet &contacts);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Add new contact to this chat.
	 * @param contact contact to add
	 *
	 * Calling this method may result in emiiting of @link contactAdded @endlink signal if contact was not already on chat.
	 */
	void addContact(const Contact &contact);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Remove contact from this chat.
	 * @param contact contact to remove
	 *
	 * Calling this method may result in emiiting of @link contactRemoved @endlink signal if contact was on chat.
	 */
	void removeContact(const Contact &contact);

};

/**
 * @}
 */

#endif // CHAT_DETAILS_ROOM_H
