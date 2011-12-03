/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef CHAT_DETAILS_H
#define CHAT_DETAILS_H

#include "chat/chat-shared.h"
#include "storage/details.h"

class BuddySet;
class Chat;
class ChatType;
class ContactSet;

/**
 * @addtogroup Chat
 * @{
 */

/**
 * @class ChatDetails
 * @author Rafal 'Vogel' Malinowski
 * @short Chat data specyfic to given chat type.
 *
 * Objects derivered from this class contains chat data that is specific to
 * given chat type. This class creates common interface for this custom data
 * - it can return list of contacts in that chat.
 */
class ChatDetails : public QObject, public Details<ChatShared>
{
	Q_OBJECT

	friend class ChatShared;

public:
	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @param mainData ChatShared object described by this object
	 * @short Default contructor.
	 *
	 * Contructor must be overridden and called by all subclasses. It assigns
	 * new object ot mainData object.
	 */
	explicit ChatDetails(ChatShared *mainData) : Details<ChatShared>(mainData) {}
	virtual ~ChatDetails() {}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @return chat type of this object
	 * @short Returns chat type of this object.
	 *
	 * Returns chat type of this object.
	 */
	virtual ChatType * type() const = 0;

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @return list of contacts with this chat
	 * @short Returns list of contacts with this chat.
	 *
	 * Returns list of contacts with this chat.
	 */
	virtual ContactSet contacts() const = 0;

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @return name of this chat
	 * @short Returns name of this chat.
	 *
	 * Returns name of this chat. This can be anything - like IRC room name
	 * or list of names of contacts.
	 */
	virtual QString name() const = 0;

};

/**
 * @}
 */

#endif // CHAT_DETAILS_H
