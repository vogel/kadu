/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_DETAILS_H
#define CONTACT_DETAILS_H

#include <QtCore/QObject>

#include "contacts/contact.h"
#include "storage/storable-object.h"

class ContactShared;

class ContactDetails : public QObject, public StorableObject
{
	Q_OBJECT

	ContactShared *MyContactData;

public:
	explicit ContactDetails(ContactShared *contactData);
	virtual ~ContactDetails();

	ContactShared *contactData() { return MyContactData; }

};

#endif // CONTACT_DETAILS_H
