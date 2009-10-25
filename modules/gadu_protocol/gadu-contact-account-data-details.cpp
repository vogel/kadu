/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "gadu-contact-account-data.h"

unsigned int GaduContactAccountData::uin()
{
	return id().toUInt();
}

bool GaduContactAccountData::validateId()
{
	bool ok;
	id().toUInt(&ok);
	return ok;
}
