/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "tlen-contact-details.h"

TlenContactDetails::TlenContactDetails(StoragePoint *storagePoint, Contact parent) :
		ContactDetails(storagePoint, parent), MaxImageSize(0)
{
}

bool TlenContactDetails::validateId()
{
	return true;
}
