/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include "talkable.h"

Talkable::Talkable() :
		Type(ItemNone)
{
}

Talkable::Talkable(const Buddy &buddy) :
		Type(ItemBuddy), MyBuddy(buddy), MyContact(BuddyPreferredManager::instance()->preferredContact(buddy))
{
}

Talkable::Talkable(const Contact &contact) :
		Type(ItemContact), MyBuddy(contact.ownerBuddy()), MyContact(contact)
{
}

Talkable::Talkable(const Talkable &copyMe)
{
	Type = copyMe.Type;
	MyBuddy = copyMe.MyBuddy;
	MyContact = copyMe.MyContact;
}

Talkable & Talkable::operator = (const Buddy &buddy)
{
	Type = ItemBuddy;
	MyBuddy = buddy;
	MyContact = BuddyPreferredManager::instance()->preferredContact(buddy);

	return *this;
}

Talkable & Talkable::operator = (const Contact &contact)
{
	Type = ItemContact;
	MyBuddy = contact.ownerBuddy();
	MyContact = contact;

	return *this;
}

Talkable & Talkable::operator = (const Talkable &copyMe)
{
	Type = copyMe.Type;
	MyBuddy = copyMe.MyBuddy;
	MyContact = copyMe.MyContact;

	return *this;
}

bool Talkable::operator == (const Talkable &compareTo) const
{
	return (Type == compareTo.Type) && (MyBuddy == compareTo.MyBuddy) && (MyContact == compareTo.MyContact);
}

bool Talkable::operator != (const Talkable &compareTo) const
{
	return !(*this == compareTo);
}
