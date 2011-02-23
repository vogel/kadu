/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
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

#ifndef BUDDY_OR_CONTACT_H
#define BUDDY_OR_CONTACT_H

#include "buddies/buddy.h"
#include "contacts/contact.h"
#include "exports.h"

class KADUAPI BuddyOrContact
{
public:
	enum ItemType
	{
		ItemNone,
		ItemBuddy,
		ItemContact
	};

private:
	ItemType Type;
	Buddy MyBuddy;
	Contact MyContact;

public:
	BuddyOrContact();
	BuddyOrContact(const Buddy &buddy);
	BuddyOrContact(const Contact &contact);
	BuddyOrContact(const BuddyOrContact& copyMe);

	BuddyOrContact & operator = (const Buddy &buddy);
	BuddyOrContact & operator = (const Contact &contact);
	BuddyOrContact & operator = (const BuddyOrContact &copyMe);

	bool operator == (const BuddyOrContact &compareTo) const;
	bool operator != (const BuddyOrContact &compareTo) const;

	ItemType type() { return Type; }
	Buddy buddy() { return MyBuddy; }
	Contact contact() { return MyContact; }

};

#endif // BUDDY_OR_CONTACT_H
