/*
 * %kadu copyright begin%
 * Copyright 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "jabber-account-details.h"

class QTimer;

class JabberProtocol;
class NetworkProxy;

class JabberConnectionService : public QObject
{
	Q_OBJECT

	QTimer *CleanUpTimer;
	JabberProtocol *ParentProtocol;

//	QPointer<AdvancedConnector> Connector;
	//QPointer<QCATLSHandler> TLSHandler;
	//QPointer<ClientStream> Stream;

	//Jid MyJid;
	QString Password;
	QString LocalAddress;

	bool forceTLS() const;
	bool useSSL() const;

	//AdvancedConnector::Proxy createProxyConfiguration(NetworkProxy proxy) const;
	//AdvancedConnector * createConnector();
	//QCATLSHandler * createTLSHandler();

	//static ClientStream::AllowPlainType plainAuthToXMPP(JabberAccountDetails::AllowPlainType type);
	//ClientStream * createClientStream(AdvancedConnector *connector, QCATLSHandler *tlsHandler) const;

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

	//Jid jid() const;
	QString host() const;
	QString localAddress() const;

signals:
	void connected();

	void connectionError(const QString &message = QString());
	void connectionClosed(const QString &message = QString());

	void invalidPassword();

	void tlsCertificateAccepted();

};

#endif // JABBER_CONNECTION_SERVICE_H
