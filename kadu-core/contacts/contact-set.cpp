/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include "accounts/account-manager.h"
#include "buddies/buddy-set.h"
#include "contacts/contact-manager.h"

#include "contact-set.h"

ContactSet::ContactSet()
{
}

ContactSet::ContactSet(const Contact &contact)
{
	insert(contact);
}

QVector<Contact> ContactSet::toContactVector() const
{
	QVector<Contact> result;
	result.reserve(size());
	foreach (const Contact &contact, *this)
		result.append(contact);

	return result;
}

BuddySet ContactSet::toBuddySet() const
{
	BuddySet buddies;
	foreach (const Contact &contact, *this)
		if (contact.ownerBuddy())
			buddies.insert(contact.ownerBuddy());

	return buddies;
}

Contact ContactSet::toContact() const
{
	if (count() != 1)
		return Contact::null;

	return *constBegin();
}
