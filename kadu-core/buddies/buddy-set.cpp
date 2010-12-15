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

#include "accounts/account-manager.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "buddies/buddy-list.h"
#include "buddies/buddy-shared.h"

#include "buddy-set.h"

BuddySet::BuddySet()
{
}

BuddySet::BuddySet(Buddy buddy)
{
	insert(buddy);
}

BuddyList BuddySet::toBuddyList() const
{
	return toList();
}

QList<Contact> BuddySet::getAllContacts() const
{
	QList<Contact> allContacts;
	foreach (const Buddy &buddy, toList())
		allContacts.append(buddy.contacts());

	return allContacts;
}

uint qHash(const BuddySet &buddySet)
{
	int hash = 0;
	foreach (const Buddy &buddy, buddySet)
		hash = hash ^ qHash(buddy);

	return hash;
}
