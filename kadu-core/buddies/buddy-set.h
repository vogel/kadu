/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
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

#ifndef BUDDY_SET_H
#define BUDDY_SET_H

#include <QtCore/QList>
#include <QtCore/QSet>

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

	bool isAllAnonymous() const;
	BuddyList toBuddyList() const;
	Buddy toBuddy() const;

	QVector<Contact> getAllContacts() const;

};

uint qHash(const BuddySet &buddySet);

#endif // BUDDY_SET_H
