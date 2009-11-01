/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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
	virtual void updatePersonalInfo(Buddy contact);

};

#endif // GADU_PERSONAL_INFO_SERVICE_H

