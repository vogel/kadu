/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contacts/contact-list.h"

#include "contact-set.h"

ContactSet::ContactSet()
{
}

ContactSet::ContactSet(Contact contact)
{
	insert(contact);
}

ContactList ContactSet::toContactList() const
{
	return toList();
}
