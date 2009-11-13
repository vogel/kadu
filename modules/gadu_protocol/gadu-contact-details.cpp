/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "gadu-contact-details.h"

GaduContactDetails::GaduContactDetails(StoragePoint *storagePoint, Contact parent) :
		ContactDetails(storagePoint, parent),
		MaxImageSize(0), GaduProtocolVersion(0)
{
}

GaduContactDetails::~GaduContactDetails()
{
}

unsigned int GaduContactDetails::uin()
{
	return contact().id().toUInt();
}

bool GaduContactDetails::validateId()
{
	bool ok;
	contact().id().toUInt(&ok);
	return ok;
}

void GaduContactDetails::store()
{
}
