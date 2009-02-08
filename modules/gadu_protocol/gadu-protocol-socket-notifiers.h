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

	gg_session *Sess;
	int socketEventCalls;

	void connectionFailed(int);

protected:
	virtual void socketEvent();

public:
	GaduProtocolSocketNotifiers(Account *account, QObject *parent = 0);
	virtual ~GaduProtocolSocketNotifiers();
	void setSession(gg_session *sess);
	void checkWrite();

	void setAccount(Account *account) { CurrentAccount = account; }

public slots:
	virtual void dataReceived();
	virtual void dataSent();


signals:
	void ackReceived(int seq, uin_t uin, int status);
	void connected();

	/**
		Otrzymano wiadomo�� CTCP.
		Kto� nas prosi o po��czenie dcc, poniewa�
		jeste�my za NAT-em.
		TODO: zmieni� nazw�.
	**/
	void dccConnectionReceived(Contact contact);

	/**
		Sygna� jest emitowany, gdy serwer przerwa� po��czenie
	**/
	void serverDisconnected();

	void error(GaduProtocol::GaduError);
	void imageReceived(UinType, uint32_t, uint32_t, const QString &filename, const char *data);
	void imageRequestReceived(UinType, uint32_t, uint32_t);
	void messageReceived(int, ContactList, QString &, time_t, QByteArray &);
	void pubdirReplyReceived(gg_pubdir50_t);
	void systemMessageReceived(QString &, QDateTime &, int, void *);
	void userlistReceived(const struct gg_event *);
	void userlistReplyReceived(char, char *);
	void userStatusChanged(const struct gg_event *);

	void dcc7New(struct gg_dcc7 *);
	void dcc7Accepted(struct gg_dcc7 *);
	void dcc7Rejected(struct gg_dcc7 *);

};

#ifndef _MSC_VER
#pragma GCC visibility pop
#endif

#endif // GADU_PROTOCOL_SOCKET_NOTIFIERS_H
