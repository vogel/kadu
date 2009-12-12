/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account-manager.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "buddies/buddy-list.h"

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
