/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddies/buddy.h"
#include "contacts/contact.h"

#include "buddy-name-filter.h"

bool BuddyNameFilter::acceptBuddy(Buddy buddy)
{
	if (Name.isEmpty())
		return true;

	if (buddy.display().contains(Name, Qt::CaseInsensitive))
		return true;
	if (buddy.firstName().contains(Name, Qt::CaseInsensitive))
		return true;
	if (buddy.lastName().contains(Name, Qt::CaseInsensitive))
		return true;
	if (buddy.nickName().contains(Name, Qt::CaseInsensitive))
		return true;
	if (buddy.email().contains(Name, Qt::CaseInsensitive))
		return true;

	foreach (const Contact &contact, buddy.contacts())
		if (contact.id().contains(Name, Qt::CaseInsensitive))
			return true;

	return false;
}

bool BuddyNameFilter::ignoreNextFilters()
{
	return !Name.isEmpty();
}

void BuddyNameFilter::setName(const QString &name)
{
	if (Name != name)
	{
		Name = name;
		emit filterChanged();
	}
}
