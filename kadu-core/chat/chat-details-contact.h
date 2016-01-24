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

#include "chat/chat-details.h"
#include "contacts/contact.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class BuddyManager;
class ChatTypeManager;
class ContactManager;

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

public:
	explicit ChatDetailsContact(ChatShared *chatData);
	virtual ~ChatDetailsContact();

	// TODO: should probably return just string
	virtual ChatType * type() const;
	virtual ContactSet contacts() const;
	virtual QString name() const;

	virtual bool isConnected() const;

	void setContact(const Contact &contact);
	Contact contact();

protected:
	virtual void load();
	virtual void store();
	virtual bool shouldStore();

private:
	QPointer<BuddyManager> m_buddyManager;
	QPointer<ChatTypeManager> m_chatTypeManager;
	QPointer<ContactManager> m_contactManager;

	Contact CurrentContact;

private slots:
	INJEQT_SET void setBuddyManager(BuddyManager *buddyManager);
	INJEQT_SET void setChatTypeManager(ChatTypeManager *chatTypeManager);
	INJEQT_SET void setContactManager(ContactManager *contactManager);

};

/**
 * @}
 */
