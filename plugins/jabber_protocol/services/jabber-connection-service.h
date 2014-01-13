/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * %kadu copyright end%
 *
 * Copyright (C) 2006 Remko Troncon
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

#ifndef JABBER_CONNECTION_SERVICE_H
#define JABBER_CONNECTION_SERVICE_H

#include <QtCore/QPointer>

#include <jid.h>
#include <xmpp.h>

#include "jabber-account-details.h"

class QTimer;

class JabberProtocol;
class NetworkProxy;

namespace XMPP
{

class Client;

class JabberConnectionService : public QObject
{
	Q_OBJECT

	QTimer *CleanUpTimer;
	JabberProtocol *ParentProtocol;
	QPointer<XMPP::Client> XmppClient;

	QPointer<XMPP::AdvancedConnector> Connector;
	QPointer<XMPP::QCATLSHandler> TLSHandler;
	QPointer<XMPP::ClientStream> Stream;

	XMPP::Jid MyJid;
	QString Password;
	QString LocalAddress;

	bool forceTLS() const;
	bool useSSL() const;

	XMPP::AdvancedConnector::Proxy createProxyConfiguration(NetworkProxy proxy) const;
	XMPP::AdvancedConnector * createConnector();
	XMPP::QCATLSHandler * createTLSHandler();

	static XMPP::ClientStream::AllowPlainType plainAuthToXMPP(JabberAccountDetails::AllowPlainType type);
	XMPP::ClientStream * createClientStream(XMPP::AdvancedConnector *connector, XMPP::QCATLSHandler *tlsHandler) const;

private slots:
	void cleanUp();

	void tlsHandshaken();

	void streamNeedAuthParams(bool user, bool pass, bool realm);
	void streamAuthenticated();
	void streamSessionStarted();
	void streamWarning(int warning);
	void streamError(int error);

public:
	explicit JabberConnectionService(JabberProtocol *protocol);
	virtual ~JabberConnectionService();

	void connectToServer();
	void disconnectFromServer(const ::Status &status);

	XMPP::Jid jid() const;
	QString host() const;
	QString localAddress() const;

signals:
	void connected();

	void connectionError(const QString &message = QString());
	void connectionClosed(const QString &message = QString());

	void invalidPassword();

	void tlsCertificateAccepted();

};

}

#endif // JABBER_CONNECTION_SERVICE_H
