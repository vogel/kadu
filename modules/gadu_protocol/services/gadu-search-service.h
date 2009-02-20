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

#include "contacts/contact.h"

#include "protocols/services/search-service.h"

class GaduProtocol;

class GaduSearchService : public SearchService
{
	Q_OBJECT

	GaduProtocol *Protocol;
	Contact Query;
	unsigned int SearchSeq;
	unsigned int From;
	bool Stopped;

private slots:
	void pubdirReplyReceived(gg_pubdir50_t res);

public:
	GaduSearchService(GaduProtocol *protocol);

	// TODO 0.6.6: restore SearchRecord or QMap<QString, QString> or sth?
	virtual void searchFirst(Contact contact);
	virtual void searchNext();
	virtual void stop();

};

#endif // GADU_SEARCH_SERVICE_H
