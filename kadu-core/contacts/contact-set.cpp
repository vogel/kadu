/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "buddies/buddy-set.h"
#include "buddies/buddy-shared.h"
#include "contacts/contact-manager.h"

#include "contact-set.h"

ContactSet::ContactSet()
{
}

ContactSet::ContactSet(const Contact &contact)
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
	foreach (const Contact &contact, *this)
		buddies.insert(contact.ownerBuddy());

	return buddies;
}

Contact ContactSet::toContact() const
{
	if (count() != 1)
		return Contact::null;

	return (*constBegin());
}
