/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include <QtCore/QObject>
#include <injeqt/injeqt.h>
#include <libgadu.h>

#include "buddies/buddy.h"

#include "protocols/services/search-service.h"

class BuddyStorage;
class GaduConnection;

class GaduSearchService : public SearchService
{
	Q_OBJECT

	QPointer<BuddyStorage> m_buddyStorage;
	QPointer<GaduConnection> Connection;
	BuddySearchCriteria Query;
	unsigned int SearchSeq;
	unsigned int From;
	bool Stopped;

	friend class GaduProtocolSocketNotifiers;
	void handleEventPubdir50SearchReply(struct gg_event *e);

private slots:
	INJEQT_SET void setBuddyStorage(BuddyStorage *buddyStorage);

public:
	explicit GaduSearchService(Account account, QObject *parent = nullptr);
	virtual ~GaduSearchService();

	void setConnection(GaduConnection *connection);

	virtual void searchFirst(BuddySearchCriteria criteria);
	virtual void searchNext();
	virtual void stop();


};
