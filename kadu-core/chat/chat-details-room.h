/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "exports.h"

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
class KADUAPI ChatDetailsRoom : public ChatDetails
{
	Q_OBJECT

	QString Room;
	QString Nick;
	QString Password;

	ContactSet Contacts;

	bool Connected;

private slots:
	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Slot called when a protocol's connection state changes.
	 *
	 * This slot is called when a protocol's connection state changes. If protocol is disconnected then value of Connected property
	 * is set to false and @link disconnected @endlink may be emited.
	 */
	void updateConnected();

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
	 * @short Set value of Connected property.
	 * @param newConnected new value of Connected property
	 *
	 * This method sets value of Connected property only when protocol of this @link Chat @endlink is connected. If not,
	 * the value of Connected property will remain false.
	 *
	 * One of signals @link connected @endlink and @link disconnected @endlink may be emited after calling this method.
	 */
	void setConnected(bool newConnected);
	virtual bool isConnected() const;

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Set room for this chat.
	 * @param room new room for this chat
	 *
	 * Room can have different format depending on protocol. For example, for XMPP MUC it is JIS in form of
	 * room@server.
	 */
	void setRoom(const QString &room);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Get current room for this chat.
	 * @return current room for this chat
	 */
	QString room() const;

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Set nick for this chat.
	 * @param nick new nick for this chat
	 */
	void setNick(const QString &nick);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Get current nick for this chat.
	 * @return current nick for this chat
	 */
	QString nick() const;

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
