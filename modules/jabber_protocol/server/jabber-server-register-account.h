/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef JABBER_SERVER_REGISTER_ACCOUNT_H
#define JABBER_SERVER_REGISTER_ACCOUNT_H

#include "protocols/protocol.h"

#include <QtCore/QObject>
#include "im.h"
#include "xmpp_tasks.h"

#include "client/mini-client.h"

class JabberServerRegisterAccount : public QObject
{
	Q_OBJECT

	bool Result;

	QString Server;
	QString Username;
	QString Password;

	QString Jid;
	MiniClient *Client;

	// Account settings
	XMPP::Jid jid_, server_;
	//UserAccount::SSLFlag ssl_;
	bool opt_host_, legacy_ssl_probe_;
	bool isOld_;
	QString host_;
	int port_;
	QString pass_;
	QString proxy_;

	XMPP::XData fields;

	XMPP::Form convertFromXData(const XMPP::XData& xdata);
	XMPP::XData convertToXData(const XMPP::Form& form);

public:
	JabberServerRegisterAccount(const QString &server, const QString &username, const QString &password, bool legacySSLProbe, bool legacySSL, bool forceSSL, const QString &host, int port);

	void performAction();

	QString jid() { return Jid; }

	bool result() { return Result; }
	MiniClient * client() { return Client; };

private slots:

	void clientHandshaken();
	void clientError();
	void actionFinished();
	void sendRegistrationData();

signals:
	void finished(JabberServerRegisterAccount *);
};

#endif // JABBER_SERVER_REGISTER_ACCOUNT_H
