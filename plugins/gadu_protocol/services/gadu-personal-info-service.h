/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef GADU_PERSONAL_INFO_SERVICE_H
#define GADU_PERSONAL_INFO_SERVICE_H

#include <libgadu.h>

#include "protocols/services/personal-info-service.h"

class GaduConnection;

class GaduPersonalInfoService : public PersonalInfoService
{
	Q_OBJECT

	QPointer<GaduConnection> Connection;

	unsigned int FetchSeq;
	unsigned int UpdateSeq;

	void fetchReplyReceived(gg_pubdir50_t res);
	void updateReplyReceived(gg_pubdir50_t res);

	friend class GaduProtocolSocketNotifiers;
	void handleEventPubdir50Read(struct gg_event *e);
	void handleEventPubdir50Write(struct gg_event *e);

public:
	explicit GaduPersonalInfoService(Account account, QObject *parent = 0);
	virtual ~GaduPersonalInfoService();

	void setConnection(GaduConnection *connection);

	virtual void fetchPersonalInfo(const QString &id);
	virtual void updatePersonalInfo(const QString &id, Buddy buddy);

};

#endif // GADU_PERSONAL_INFO_SERVICE_H

