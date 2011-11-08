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

#ifndef TALKABLE_H
#define TALKABLE_H

#include "buddies/buddy.h"
#include "contacts/contact.h"
#include "exports.h"

class KADUAPI Talkable
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
	Talkable();
	Talkable(const Buddy &buddy);
	Talkable(const Contact &contact);
	Talkable(const Talkable &copyMe);

	Talkable & operator = (const Talkable &copyMe);

	bool operator == (const Talkable &compareTo) const;
	bool operator != (const Talkable &compareTo) const;

	ItemType type() const { return Type; }

	Buddy buddy() const;
	Contact contact() const;

	bool isEmpty() const;

	Avatar avatar() const;
	bool isBlocked() const;
	bool isBlocking() const;
	Account account() const;
	Status currentStatus() const;

};

#endif // TALKABLE_H
