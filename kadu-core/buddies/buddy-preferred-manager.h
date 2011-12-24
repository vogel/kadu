/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
class Contact;

class KADUAPI BuddyPreferredManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(BuddyPreferredManager)

	static BuddyPreferredManager *Instance;

	//QMap<Buddy, Contact> Preferreds;

	BuddyPreferredManager();
	~BuddyPreferredManager();

public:
	static BuddyPreferredManager *instance();

	Contact preferredContact(const Buddy &buddy, const Account &account, bool includechats = true);
	Contact preferredContact(const Buddy &buddy, bool includechats = true);
	Account preferredAccount(const Buddy &buddy, bool includechats = true);

	Contact preferredContactByPriority(const Buddy &buddy, const Account &account = Account::null);
	Contact preferredContactByUnreadMessages(const Buddy &buddy, const Account &account = Account::null);
	Contact preferredContactByChatWidgets(const Buddy &buddy, const Account &account = Account::null);
	Contact preferredContactByStatus(const Buddy &buddy, const Account &account = Account::null);
	Contact morePreferredContactByStatus(const Contact & c1, const Contact & c2, const Account &account = Account::null);

	//void updatePreferred(Buddy buddy);

signals:
	void buddyUpdated(Buddy &buddy);

};

#endif // BUDDY_PREFERRED_MANAGER_H
