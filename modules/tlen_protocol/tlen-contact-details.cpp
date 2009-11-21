/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contacts/contact-shared.h"

#include "tlen-contact-details.h"

class ContactShared;

TlenContactDetails::TlenContactDetails(ContactShared *contactShared) :
		ContactDetails(contactShared), MaxImageSize(0)
{
}

TlenContactDetails::~TlenContactDetails()
{
}

bool TlenContactDetails::validateId()
{
	return true;
}

void TlenContactDetails::store()
{
}
