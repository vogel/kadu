/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef JABBER_CONTACT_DETAILS_H
#define JABBER_CONTACT_DETAILS_H

#include "contacts/contact.h"
#include "contacts/contact-details.h"

class JabberContactDetails : public ContactDetails
{
	PROPERTY_DEC(unsigned long, MaxImageSize)

public:
	explicit JabberContactDetails(StoragePoint *storagePoint, Contact parent = Contact::null);
	virtual ~JabberContactDetails();

	virtual bool validateId();
	virtual void store();

	PROPERTY_DEF(unsigned long, maxImageSize, setMaxImageSize, MaxImageSize)

};

#endif // JABBER_CONTACT_DETAILS_H
