/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "buddies/buddy.h"

#include "buddy-remove-predicate-object.h"

KADU_PREDICATE_CLASS(BuddyRemovePredicateObject)

bool BuddyRemovePredicateObject::inquireAll(Buddy contact)
{
	foreach (BuddyRemovePredicateObject *object, Objects)
		if (!object->removeContactFromStorage(contact))
			return false;

	return true;
}
