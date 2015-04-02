/*
 * %kadu copyright begin%
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gadu-socket-notifiers.h"

#include "gadu-protocol.h"

#include "accounts/account.h"

class GaduIMTokenService;
class GaduUserDataService;

class GaduProtocolSocketNotifiers : public GaduSocketNotifiers
{
	Q_OBJECT

public:
	explicit GaduProtocolSocketNotifiers(Account account, GaduProtocol *protocol);
	virtual ~GaduProtocolSocketNotifiers();

	void setGaduIMTokenService(GaduIMTokenService *imTokenService);
	void setGaduUserDataService(GaduUserDataService *userDataService);

	void watchFor(gg_session *sess);

signals:
	void msgEventReceived(struct gg_event *e);
	void multilogonMsgEventReceived(struct gg_event *e);
	void ackEventReceived(struct gg_event *e);

	void typingNotificationEventReceived(struct gg_event *e);

protected:
	virtual bool checkRead();
	virtual bool checkWrite();
	virtual void socketEvent();
	virtual int timeout();
	virtual bool handleSoftTimeout();
	virtual void connectionTimeout();

private:
	Account m_account;
	GaduProtocol *m_protocol;
	gg_session *m_session;
	QPointer<GaduIMTokenService> m_imTokenService;
	QPointer<GaduUserDataService> m_userDataService;

	void dumpConnectionState();
	void handleEventNotify(struct gg_event *e);
	void handleEventNotify60(struct gg_event *e);
	void handleEventStatus(struct gg_event *e);
	void handleEventConnFailed(struct gg_event *e);
	void handleEventConnSuccess(struct gg_event *e);
	void handleEventDisconnect(struct gg_event *e);
	void handleEventMultilogonInfo(struct gg_event *e);

};
