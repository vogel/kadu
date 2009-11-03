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
	foreach (Buddy buddy, BuddyManager::instance()->buddies())
		contactAdded(buddy);
}

void BuddiesAwareObject::triggerAllBuddiesRemoved()
{
	foreach (Buddy buddy, BuddyManager::instance()->buddies())
		contactRemoved(buddy);
}
