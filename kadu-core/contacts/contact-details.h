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

#include "contacts/contact-shared.h"
#include "storage/details.h"

class ContactDetails : public Details<ContactShared>
{

public:
	explicit ContactDetails(ContactShared *mainData);
	virtual ~ContactDetails();

};

#endif // CONTACT_DETAILS_H
