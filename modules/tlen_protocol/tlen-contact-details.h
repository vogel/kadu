/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TLEN_CONTACT_DETAILS
#define TLEN_CONTACT_DETAILS

#include "contacts/contact.h"
#include "contacts/contact-details.h"

class Contact;

class TlenContactDetails : public ContactDetails
{
	PROPERTY_DEC(unsigned long, MaxImageSize)

public:
	explicit TlenContactDetails(StoragePoint *storagePoint, Contact parent = Contact::null);
	virtual ~TlenContactDetails();

	virtual bool validateId();
	virtual void store();

	PROPERTY_DEF(unsigned long, maxImageSize, setMaxImageSize, MaxImageSize)

};

#endif // TLEN_CONTACT_DETAILS
