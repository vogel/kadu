/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#ifndef GADU_CONTACT_PERSONAL_INFO_SERVICE_H
#define GADU_CONTACT_PERSONAL_INFO_SERVICE_H

#include <libgadu.h>

#include "protocols/services/contact-personal-info-service.h"

class GaduProtocol;

class GaduContactPersonalInfoService : public ContactPersonalInfoService
{
	Q_OBJECT

	GaduProtocol *Protocol;
	unsigned int FetchSeq;
	QString Id;

	friend class GaduProtocolSocketNotifiers;
	void handleEventPubdir50Read(struct gg_event *e);

public:
	GaduContactPersonalInfoService(GaduProtocol *protocol);

	virtual void fetchPersonalInfo(Contact contact);

};

#endif // GADU_CONTACT_PERSONAL_INFO_SERVICE_H

