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
	int socketEventCalls;

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

public:
	GaduProtocolSocketNotifiers(Account *account, GaduProtocol *protocol, QObject *parent = 0) :
			CurrentAccount(account), CurrentProtocol(protocol),
			GaduSocketNotifiers(parent), Sess(0), socketEventCalls(0) {}

	void watchFor(gg_session *sess);

	void setAccount(Account *account) { CurrentAccount = account; }

signals:
	/**
		Sygna� jest emitowany, gdy serwer przerwa� po��czenie
	**/
	void serverDisconnected();

	void error(GaduProtocol::GaduError);
	void imageReceived(UinType, uint32_t, uint32_t, const QString &filename, const char *data);
	void imageRequestReceived(UinType, uint32_t, uint32_t);
	void pubdirReplyReceived(gg_pubdir50_t);
	void systemMessageReceived(QString &, QDateTime &, int, void *);
	void userlistReceived(const struct gg_event *);
	void userlistReplyReceived(char, char *);

	void dcc7New(struct gg_dcc7 *);
	void dcc7Accepted(struct gg_dcc7 *);
	void dcc7Rejected(struct gg_dcc7 *);
	void dcc7Error(struct gg_dcc7 *);

};

#ifndef _MSC_VER
#pragma GCC visibility pop
#endif

#endif // GADU_PROTOCOL_SOCKET_NOTIFIERS_H
