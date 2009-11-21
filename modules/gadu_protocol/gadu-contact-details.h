/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_CONTACT_DETAILS_H
#define GADU_CONTACT_DETAILS_H

#include "contacts/contact.h"
#include "contacts/contact-details.h"

#include "gadu-protocol.h"

class GaduContactDetails : public ContactDetails
{
	PROPERTY_DEC(unsigned long, MaxImageSize)
	PROPERTY_DEC(unsigned int, GaduProtocolVersion)

public:
	explicit GaduContactDetails(ContactShared *contactShared);
	virtual ~GaduContactDetails();

	virtual bool validateId();
	virtual void store();

	GaduProtocol::UinType uin();

	PROPERTY_DEF(unsigned long, maxImageSize, setMaxImageSize, MaxImageSize)
	PROPERTY_DEF(unsigned int, gaduProtocolVersion, setGaduProtocolVersion, GaduProtocolVersion)

};

#endif // GADU_CONTACT_DETAILS_H
