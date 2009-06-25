/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contact.h"

#include "contact-remove-predicate-object.h"

KADU_PREDICATE_CLASS(ContactRemovePredicateObject)

bool ContactRemovePredicateObject::inquireAll(Contact contact)
{
	foreach (ContactRemovePredicateObject *object, Objects)
		if (!object->removeContactFromStorage(contact))
			return false;

	return true;
}
