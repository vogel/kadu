/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddies/buddy-preferred-manager.h"

#include "buddy-or-contact.h"

BuddyOrContact::BuddyOrContact() :
		Type(ItemNone)
{
}

BuddyOrContact::BuddyOrContact(Buddy buddy) :
		Type(ItemBuddy), MyBuddy(buddy), MyContact(BuddyPreferredManager::instance()->preferredContact(buddy))
{
}

BuddyOrContact::BuddyOrContact(Contact contact) :
		Type(ItemContact), MyBuddy(contact.ownerBuddy()), MyContact(contact)
{
}

BuddyOrContact::BuddyOrContact(const BuddyOrContact &copyMe)
{
	Type = copyMe.Type;
	MyBuddy = copyMe.MyBuddy;
	MyContact = copyMe.MyContact;
}

BuddyOrContact & BuddyOrContact::operator = (Buddy buddy)
{
	Type = ItemBuddy;
	MyBuddy = buddy;
	MyContact = BuddyPreferredManager::instance()->preferredContact(buddy);

	return *this;
}

BuddyOrContact & BuddyOrContact::operator = (Contact contact)
{
	Type = ItemContact;
	MyBuddy = contact.ownerBuddy();
	MyContact = contact;

	return *this;
}

BuddyOrContact & BuddyOrContact::operator = (const BuddyOrContact &copyMe)
{
	Type = copyMe.Type;
	MyBuddy = copyMe.MyBuddy;
	MyContact = copyMe.MyContact;

	return *this;
}

bool BuddyOrContact::operator == (const BuddyOrContact &compareTo) const
{
	return (Type == compareTo.Type) && (MyBuddy == compareTo.MyBuddy) && (MyContact == compareTo.MyContact);
}

bool BuddyOrContact::operator != (const BuddyOrContact &compareTo) const
{
	return !(*this == compareTo);
}
