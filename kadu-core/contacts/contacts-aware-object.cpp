/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contact-manager.h"

#include "contacts-aware-object.h"

KADU_AWARE_CLASS(ContactsAwareObject)

void ContactsAwareObject::notifyContactAdded(Contact contact)
{
	foreach (ContactsAwareObject *object, Objects)
		object->contactAdded(contact);
}

void ContactsAwareObject::notifyContactRemoved(Contact contact)
{
	foreach (ContactsAwareObject *object, Objects)
		object->contactRemoved(contact);
}

void ContactsAwareObject::triggerAllContactsAdded()
{
	foreach (Contact contact, ContactManager::instance()->contacts())
		contactAdded(contact);
}

void ContactsAwareObject::triggerAllContactsRemoved()
{
	foreach (Contact contact, ContactManager::instance()->contacts())
		contactRemoved(contact);
}
