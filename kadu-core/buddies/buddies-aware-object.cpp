/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include "buddies/buddy-manager.h"

#include "buddies-aware-object.h"

KADU_AWARE_CLASS(BuddiesAwareObject)

void BuddiesAwareObject::notifyBuddyAdded(Buddy buddy)
{
	foreach (BuddiesAwareObject *object, Objects)
		object->contactAdded(buddy);
}

void BuddiesAwareObject::notifyBuddyRemoved(Buddy buddy)
{
	foreach (BuddiesAwareObject *object, Objects)
		object->contactRemoved(buddy);
}

void BuddiesAwareObject::triggerAllBuddiesAdded()
{
	foreach (const Buddy &buddy, BuddyManager::instance()->items())
		contactAdded(buddy);
}

void BuddiesAwareObject::triggerAllBuddiesRemoved()
{
	foreach (const Buddy &buddy, BuddyManager::instance()->items())
		contactRemoved(buddy);
}
