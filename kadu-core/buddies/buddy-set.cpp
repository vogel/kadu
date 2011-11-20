/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include "accounts/account-manager.h"
#include "buddies/buddy-list.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"

#include "buddy-set.h"

BuddySet::BuddySet()
{
}

BuddySet::BuddySet(Buddy buddy)
{
	insert(buddy);
}

bool BuddySet::isAllAnonymous() const
{
	foreach (const Buddy &buddy, *this)
		if (!buddy.isAnonymous())
			return false;

	return true;
}

BuddyList BuddySet::toBuddyList() const
{
	return toList();
}

Buddy BuddySet::toBuddy() const
{
	if (count() != 1)
		return Buddy::null;

	return *constBegin();
}

QVector<Contact> BuddySet::getAllContacts() const
{
	QVector<Contact> allContacts;
	foreach (const Buddy &buddy, *this)
		foreach (const Contact &contact, buddy.contacts())
			allContacts.append(contact);

	return allContacts;
}

uint qHash(const BuddySet &buddySet)
{
	int hash = 0;
	foreach (const Buddy &buddy, buddySet)
		hash = hash ^ qHash(buddy);

	return hash;
}
