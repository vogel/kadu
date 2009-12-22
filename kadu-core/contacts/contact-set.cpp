/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account-manager.h"
#include "buddies/buddy-set.h"
#include "buddies/buddy-shared.h"
#include "contacts/contact-manager.h"

#include "contact-set.h"

ContactSet::ContactSet()
{
}

ContactSet::ContactSet(Contact contact)
{
	insert(contact);
}

QList<Contact> ContactSet::toContactList() const
{
	return toList();
}

BuddySet ContactSet::toBuddySet() const
{
	BuddySet buddies;
	foreach (const Contact &contact, toList())
		buddies.insert(contact.ownerBuddy());

	return buddies;
}

Contact ContactSet::toContact() const
{
	if (count() != 1)
		return Contact::null;

	return (*begin());
}
