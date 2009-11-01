/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "buddies/buddy-manager.h"

#include "buddies-aware-object.h"

KADU_AWARE_CLASS(BuddiesAwareObject)

void BuddiesAwareObject::notifyBuddyAdded(Buddy contact)
{
	foreach (BuddiesAwareObject *object, Objects)
		object->contactAdded(contact);
}

void BuddiesAwareObject::notifyBuddyRemoved(Buddy contact)
{
	foreach (BuddiesAwareObject *object, Objects)
		object->contactRemoved(contact);
}

void BuddiesAwareObject::triggerAllBuddiesAdded()
{
	foreach (Buddy contact, BuddyManager::instance()->buddies())
		contactAdded(contact);
}

void BuddiesAwareObject::triggerAllBuddiesRemoved()
{
	foreach (Buddy contact, BuddyManager::instance()->buddies())
		contactRemoved(contact);
}
