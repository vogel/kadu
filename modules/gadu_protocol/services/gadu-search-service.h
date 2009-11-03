/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_SEARCH_SERVICE_H
#define GADU_SEARCH_SERVICE_H

#include <QtCore/QObject>

#include <libgadu.h>

#include "buddies/buddy.h"

#include "protocols/services/search-service.h"

class GaduProtocol;

class GaduSearchService : public SearchService
{
	Q_OBJECT

	GaduProtocol *Protocol;
	BuddySearchCriteria Query;
	unsigned int SearchSeq;
	unsigned int From;
	bool Stopped;

	friend class GaduProtocolSocketNotifiers;
	void handleEventPubdir50SearchReply(struct gg_event *e);

public:
	GaduSearchService(GaduProtocol *protocol);
	virtual void searchFirst(BuddySearchCriteria criteria);
	virtual void searchNext();
	virtual void stop();

};

#endif // GADU_SEARCH_SERVICE_H
