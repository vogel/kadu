/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>
#include <libgadu.h>

#include "protocols/services/personal-info-service.h"

class BuddyStorage;
class GaduConnection;

class GaduPersonalInfoService : public PersonalInfoService
{
	Q_OBJECT

	QPointer<BuddyStorage> m_buddyStorage;
	QPointer<GaduConnection> Connection;

	unsigned int FetchSeq;
	unsigned int UpdateSeq;

	void fetchReplyReceived(gg_pubdir50_t res);
	void updateReplyReceived(gg_pubdir50_t res);

	friend class GaduProtocolSocketNotifiers;
	void handleEventPubdir50Read(struct gg_event *e);
	void handleEventPubdir50Write(struct gg_event *e);

private slots:
	INJEQT_SET void setBuddyStorage(BuddyStorage *buddyStorage);

public:
	explicit GaduPersonalInfoService(Account account, QObject *parent = nullptr);
	virtual ~GaduPersonalInfoService();

	void setConnection(GaduConnection *connection);

	virtual void fetchPersonalInfo(const QString &id);
	virtual void updatePersonalInfo(const QString &id, Buddy buddy);

};
