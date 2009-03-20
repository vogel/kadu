/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_PROTOCOL_SOCKET_NOTIFIERS_H
#define GADU_PROTOCOL_SOCKET_NOTIFIERS_H

#include "contacts/contact.h"

#include "gadu-protocol.h"

#include "gadu-socket-notifiers.h"

#ifndef _MSC_VER
#pragma GCC visibility push(hidden)
#endif

class GaduProtocolSocketNotifiers : public GaduSocketNotifiers
{
	Q_OBJECT

	Account *CurrentAccount;
	GaduProtocol *CurrentProtocol;

	gg_session *Sess;

	int Timeout;

	void dumpConnectionState();

	void handleEventMsg(struct gg_event *e);
	void handleEventNotify(struct gg_event *e);
	void handleEventNotify60(struct gg_event *e);
	void handleEventStatus(struct gg_event *e);
	void handleEventConnFailed(struct gg_event *e);
	void handleEventConnSuccess(struct gg_event *e);
	void handleEventDisconnect(struct gg_event *e);

protected:
	virtual bool checkRead();
	virtual bool checkWrite();
	virtual void socketEvent();
	virtual int timeout();
	virtual bool handleSoftTimeout();
	virtual void connectionTimeout();

public:
	GaduProtocolSocketNotifiers(Account *account, GaduProtocol *protocol, QObject *parent = 0);
	void setAccount(Account *account) { CurrentAccount = account; }

	void watchFor(gg_session *sess);

};

#ifndef _MSC_VER
#pragma GCC visibility pop
#endif

#endif // GADU_PROTOCOL_SOCKET_NOTIFIERS_H
