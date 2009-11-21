/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contacts/contact-shared.h"

#include "gadu-contact-details.h"

GaduContactDetails::GaduContactDetails(ContactShared *contactShared) :
		ContactDetails(contactShared),
		MaxImageSize(0), GaduProtocolVersion(0)
{
}

GaduContactDetails::~GaduContactDetails()
{
}

unsigned int GaduContactDetails::uin()
{
	return contactData()->id().toUInt();
}

bool GaduContactDetails::validateId()
{
	bool ok;
	contactData()->id().toUInt(&ok);
	return ok;
}

void GaduContactDetails::store()
{
}
