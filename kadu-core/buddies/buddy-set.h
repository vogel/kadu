/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef BUDDY_SET_H
#define BUDDY_SET_H

#include <QtCore/QSet>
#include <QtCore/QList>

#include "accounts/account.h"
#include "buddies/buddy.h"
#include "contacts/contact-set.h"

#include "exports.h"

class Account;
class BuddyList;
class Contact;
class ContactSet;

class KADUAPI BuddySet : public QSet<Buddy>
{

public:
	BuddySet();
	explicit BuddySet(Buddy buddy);

	BuddyList toBuddyList() const;
	QList<Contact> getAllContacts() const;

	bool operator < (const BuddySet &other) const;

};

#endif // BUDDY_SET_H
