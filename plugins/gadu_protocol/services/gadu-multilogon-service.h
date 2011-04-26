/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef GADU_MULTILOGON_SERVICE_H
#define GADU_MULTILOGON_SERVICE_H

#include <libgadu.h>

#include "accounts/account.h"
#include "protocols/services/multilogon-service.h"

class GaduMultilogonService : public MultilogonService
{
	Q_OBJECT

	Account MyAccount;
	QList<MultilogonSession *> Sessions;

	friend class GaduProtocolSocketNotifiers;
	void handleEventMultilogonInfo(struct gg_event *e);

	bool containsSession(const gg_multilogon_session &session);
	bool containsSession(const gg_event_multilogon_info &multilogonInfo, const gg_multilogon_id_t &id);
	void addNewSessions(const gg_event_multilogon_info &multilogonInfo);
	void removeOldSessions(const gg_event_multilogon_info &multilogonInfo);

	friend class GaduProtocol;
	void removeAllSessions();

public:
	GaduMultilogonService(Account account, QObject *parent);
	virtual ~GaduMultilogonService();

	virtual const QList<MultilogonSession *> & sessions() const;
	virtual void killSession(MultilogonSession *session);

};

#endif // GADU_MULTILOGON_SERVICE_H
