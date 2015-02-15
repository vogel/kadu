/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef BUDDY_PREFERRED_MANAGER_H
#define BUDDY_PREFERRED_MANAGER_H

#include <QtCore/QMap>
#include <QtCore/QObject>

#include "accounts/account.h"
#include "buddies/buddy.h"
#include "contacts/contact.h"
#include "message/message.h"

class Account;
class Buddy;
class BuddySet;
class ChatWidgetRepository;
class Contact;
class ContactSet;

class KADUAPI BuddyPreferredManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(BuddyPreferredManager)

public:
	static BuddyPreferredManager *instance();

	Contact preferredContact(const Buddy &buddy, const Account &account, bool includechats = true);
	Contact preferredContact(const Buddy &buddy, bool includechats = true);

	Contact preferredContact2(const Buddy &buddy);
	ContactSet preferredContacts(const BuddySet &buddies);

	Account preferredAccount(const Buddy &buddy, bool includechats = true);

	Contact preferredContactByPriority(const Buddy &buddy, const Account &account = Account::null);
	Contact preferredContactByUnreadMessages(const Buddy &buddy, const Account &account = Account::null);
	Contact preferredContactByChatWidgets(const Buddy &buddy, const Account &account = Account::null);
	Contact preferredContactByStatus(const Buddy &buddy, const Account &account = Account::null);
	Contact morePreferredContactByStatus(const Contact & c1, const Contact & c2, const Account &account = Account::null);

	//void updatePreferred(Buddy buddy);

signals:
	void buddyUpdated(const Buddy &buddy);

private:
	static BuddyPreferredManager *m_instance;

	QPointer<ChatWidgetRepository> m_chatWidgetRepository;

	BuddyPreferredManager();
	~BuddyPreferredManager();

	void setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository);

	bool isAccountCommon(const Account &account, const BuddySet &buddies);
	Account getCommonAccount(const BuddySet &buddies);

};

#endif // BUDDY_PREFERRED_MANAGER_H
