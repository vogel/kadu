/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CHAT_DETAILS_CONTACT_H
#define CHAT_DETAILS_CONTACT_H

#include "contacts/contact.h"

#include "chat/chat-details.h"

/**
 * @addtogroup Chat
 * @{
 */

/**
 * @class ChatDetailsContact
 * @author Rafal 'Vogel' Malinowski
 * @short Chat data specyfic to 'contact' chat type.
 *
 * Class contains one Contact object. Chat name is set to this Contact's
 * Buddy's display name, chat title is that name with current description.
 */
class ChatDetailsContact : public ChatDetails
{
	Q_OBJECT

	Contact CurrentContact;

protected:
	virtual void load();
	virtual void store();
	virtual bool shouldStore();

public:
	explicit ChatDetailsContact(ChatShared *chatData);
	virtual ~ChatDetailsContact();

	virtual ChatType * type() const;
	virtual ContactSet contacts() const;
	virtual QString name() const;

	virtual bool isConnected() const;

	void setContact(const Contact &contact);
	Contact contact();

};

/**
 * @}
 */

#endif // CHAT_DETAILS_CONTACT_H
