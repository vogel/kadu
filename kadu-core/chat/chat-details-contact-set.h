/*
 * %kadu copyright begin%
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "buddies/buddy-set.h"
#include "chat/chat-details.h"
#include "contacts/contact-set.h"
#include "contacts/contact.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ChatTypeManager;
class ContactManager;

/**
 * @addtogroup Chat
 * @{
 */

/**
 * @class ChatDetailsContactSet
 * @author Rafal 'Vogel' Malinowski
 * @short Chat data specyfic to 'ContactSet' chat type.
 *
 * Class contains set of Contact objects. Chat name is set to this Contacts'
 * Buddys' display names joined by commas, chat title is list of that names
 * with current descriptions.
 */
class ChatDetailsContactSet : public ChatDetails
{
	Q_OBJECT

public:
	explicit ChatDetailsContactSet(ChatShared *chatData);
	virtual ~ChatDetailsContactSet();

	virtual ChatType * type() const;
	virtual ContactSet contacts() const { return m_contacts; }
	virtual QString name() const;

	virtual bool isConnected() const;

	void setContacts(const ContactSet &contacts);

protected:
	virtual void load();
	virtual void store();
	virtual bool shouldStore();

private:
	QPointer<ChatTypeManager> m_chatTypeManager;
	QPointer<ContactManager> m_contactManager;

	ContactSet m_contacts;

private slots:
	INJEQT_SET void setChatTypeManager(ChatTypeManager *chatTypeManager);
	INJEQT_SET void setContactManager(ContactManager *contactManager);

};

/**
 * @}
 */
