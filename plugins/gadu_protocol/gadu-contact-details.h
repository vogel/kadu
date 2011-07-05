/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GADU_CONTACT_DETAILS_H
#define GADU_CONTACT_DETAILS_H

#include "contacts/contact.h"
#include "contacts/contact-details.h"
#include "contacts/contact-shared.h"

#include "gadu-protocol.h"

class GaduContactDetails : public ContactDetails
{
	Q_OBJECT

	int GaduFlags;

public:
	explicit GaduContactDetails(ContactShared *contactShared, QObject *parent = 0);
	virtual ~GaduContactDetails();

	virtual void store();

	GaduProtocol::UinType uin();

	void setGaduFlags(int gaduFlags);
	int gaduFlags() const { return GaduFlags; }

};

#endif // GADU_CONTACT_DETAILS_H
