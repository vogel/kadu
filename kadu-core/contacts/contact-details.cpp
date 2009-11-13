/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contacts/contact.h"
#include "contacts/contact-shared.h"

#include "contact-details.h"

ContactDetails::ContactDetails(StoragePoint *storagePoint, Contact parent) :
		QObject(parent.data()), StorableObject(storagePoint), MyContactData(parent.data())
{
}

ContactDetails::~ContactDetails()
{
}
