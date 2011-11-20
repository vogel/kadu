/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef CHAT_DETAILS_CONFERENCE_H
#define CHAT_DETAILS_CONFERENCE_H

#include "buddies/buddy-set.h"
#include "contacts/contact-set.h"
#include "contacts/contact.h"

#include "chat/chat-details.h"

/**
 * @addtogroup Chat
 * @{
 */

/**
 * @class ChatDetailsConference
 * @author Rafal 'Vogel' Malinowski
 * @short Chat data specyfic to 'conference' chat type.
 *
 * Class contains set of Contact objects. Chat name is set to this Contacts'
 * Buddys' display names joined by commas, chat title is list of that names
 * with current descriptions.
 */
class ChatDetailsConference : public ChatDetails
{
	Q_OBJECT

	ContactSet Contacts;

protected:
	virtual void load();
	virtual void store();
	virtual bool shouldStore();

public:
	explicit ChatDetailsConference(ChatShared *chatData);
	virtual ~ChatDetailsConference();

	virtual ChatType * type() const;
	virtual ContactSet contacts() const { return Contacts; }
	virtual QString name() const;

	void setContacts(const ContactSet &contacts);

};

/**
 * @}
 */

#endif // CHAT_DETAILS_CONFERENCE_H
