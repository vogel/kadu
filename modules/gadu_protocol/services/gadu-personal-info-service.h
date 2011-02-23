/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef GADU_PERSONAL_INFO_SERVICE_H
#define GADU_PERSONAL_INFO_SERVICE_H

#include <libgadu.h>

#include "protocols/services/personal-info-service.h"

class GaduProtocol;

class GaduPersonalInfoService : public PersonalInfoService
{
	Q_OBJECT

	GaduProtocol *Protocol;
	unsigned int FetchSeq;
	unsigned int UpdateSeq;

	void fetchReplyReceived(gg_pubdir50_t res);
	void updateReplyReceived(gg_pubdir50_t res);

	friend class GaduProtocolSocketNotifiers;
	void handleEventPubdir50Read(struct gg_event *e);
	void handleEventPubdir50Write(struct gg_event *e);

public:
	GaduPersonalInfoService(GaduProtocol *protocol);

	virtual void fetchPersonalInfo();
	virtual void updatePersonalInfo(Buddy buddy);

};

#endif // GADU_PERSONAL_INFO_SERVICE_H

