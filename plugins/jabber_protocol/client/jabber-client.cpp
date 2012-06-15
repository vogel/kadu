/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2009, 2010, 2010, 2011 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QCryptographicHash>
#include <QtCore/QRegExp>
#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtCrypto>

#include <bsocket.h>
#include <xmpp_tasks.h>

#include "accounts/account.h"
#include "identities/identity.h"
#include "network/proxy/network-proxy-manager.h"
#include "debug.h"

#include "certificates/certificate-helpers.h"
#include "client/pong-server.h"
#include "resource/jabber-resource-pool.h"
#include "services/jabber-client-info-service.h"
#include "services/jabber-server-info-service.h"
#include "jabber-account-details.h"
#include "jabber-client.h"
#include "jabber-protocol.h"

#define JABBER_PENALTY_TIME 2

namespace XMPP
{

JabberClient::JabberClient(XMPP::Client *client, QObject *parent) :
		QObject(parent)
{
	new PongServer(client->rootTask());
}

JabberClient::~JabberClient()
{
}

void JabberClient::getErrorInfo(int err, AdvancedConnector *conn, Stream *stream, QCATLSHandler *tlsHandler, QString *_str, bool *_reconn)
{
	QString str;
	bool reconn = false;

	if (err == -1)
	{
		str = tr("Disconnected");
		reconn = true;
	}
	else if (err == XMPP::ClientStream::ErrParse)
	{
		str = tr("XML Parsing Error");
		reconn = true;
	}
	else if (err == XMPP::ClientStream::ErrProtocol)
	{
		str = tr("XMPP Protocol Error");
		reconn = true;
	}
	else if (err == XMPP::ClientStream::ErrStream)
	{
		int x;
		QString s, detail;
		reconn = true;
		if (stream)  // Stream can apparently be gone if you disconnect in time
		{
			x = stream->errorCondition();
			detail = stream->errorText();
		}
		else
		{
			x = XMPP::Stream::GenericStreamError;
			reconn = false;
		}

		if (x == XMPP::Stream::GenericStreamError)
			s = tr("Generic stream error");
		else if (x == XMPP::ClientStream::Conflict)
		{
			s = tr("Conflict(remote login replacing this one)");
			reconn = false;
		}
		else if (x == XMPP::ClientStream::ConnectionTimeout)
			s = tr("Timed out from inactivity");
		else if (x == XMPP::ClientStream::InternalServerError)
			s = tr("Internal server error");
		else if (x == XMPP::ClientStream::InvalidXml)
			s = tr("Invalid XML");
		else if (x == XMPP::ClientStream::PolicyViolation)
		{
			s = tr("Policy violation");
			reconn = false;
		}
		else if (x == XMPP::ClientStream::ResourceConstraint)
		{
			s = tr("Server out of resources");
			reconn = false;
		}
		else if (x == XMPP::ClientStream::SystemShutdown)
		{
			s = tr("Server is shutting down");
		}
		str = tr("XMPP Stream Error: %1").arg(s) + '\n' + detail;
	}
	else if (err == XMPP::ClientStream::ErrConnection)
	{
		int x = conn->errorCode();
		QString s;
		reconn = true;
		if (x == XMPP::AdvancedConnector::ErrConnectionRefused)
			s = tr("Unable to connect to server");
		else if (x == XMPP::AdvancedConnector::ErrHostNotFound)
			s = tr("Host not found");
		else if (x == XMPP::AdvancedConnector::ErrProxyConnect)
			s = tr("Error connecting to proxy");
		else if (x == XMPP::AdvancedConnector::ErrProxyNeg)
			s = tr("Error during proxy negotiation");
		else if (x == XMPP::AdvancedConnector::ErrProxyAuth)
		{
			s = tr("Proxy authentication failed");
			reconn = false;
		}
		else if (x == XMPP::AdvancedConnector::ErrStream)
			s = tr("Socket/stream error");
		str = tr("Connection Error: %1").arg(s);
	}
	else if (err == XMPP::ClientStream::ErrNeg)
	{
		QString s, detail;
		int x = stream->errorCondition();
		detail = stream->errorText();
		if (x  == XMPP::ClientStream::HostGone)
			s = tr("Host no longer hosted");
		else if (x == XMPP::ClientStream::HostUnknown)
			s = tr("Host unknown");
		else if (x == XMPP::ClientStream::RemoteConnectionFailed)
		{
			s = tr("A required remote connection failed");
			reconn = true;
		}
		else if (x == XMPP::ClientStream::SeeOtherHost)
			s = tr("See other host: %1").arg(stream->errorText());
		else if (x == XMPP::ClientStream::UnsupportedVersion)
			s = tr("Server does not support proper XMPP version");
		str = tr("Stream Negotiation Error: %1").arg(s) + '\n' + detail;
	}
	else if (err == XMPP::ClientStream::ErrTLS)
	{
		int x = stream->errorCondition();
		QString s;
		if (x == XMPP::ClientStream::TLSStart)
			s = tr("Server rejected STARTTLS");
		else if (x == XMPP::ClientStream::TLSFail)
		{
			int t = tlsHandler->tlsError();
			if (t == QCA::TLS::ErrorHandshake)
				s = tr("TLS handshake error");
			else
				s = tr("Broken security layer (TLS)");
		}
		str = s;
	}
	else if (err == XMPP::ClientStream::ErrAuth)
	{
		int x = stream->errorCondition();
		QString s;
		if (x == XMPP::ClientStream::GenericAuthError)
			s = tr("Unable to login");
		else if (x == XMPP::ClientStream::NoMech)
		{
			s = tr("No appropriate mechanism available for given security settings(e.g. SASL library too weak, or plaintext authentication not enabled)");
			s += '\n' + stream->errorText();
		}
		else if (x == XMPP::ClientStream::BadProto)
			s = tr("Bad server response");
		else if (x == XMPP::ClientStream::BadServ)
			s = tr("Server failed mutual authentication");
		else if (x == XMPP::ClientStream::EncryptionRequired)
			s = tr("Encryption required for chosen SASL mechanism");
		else if (x == XMPP::ClientStream::InvalidAuthzid)
			s = tr("Invalid account information");
		else if (x == XMPP::ClientStream::InvalidMech)
			s = tr("Invalid SASL mechanism");
		else if (x == XMPP::ClientStream::InvalidRealm)
			s = tr("Invalid realm");
		else if (x == XMPP::ClientStream::MechTooWeak)
			s = tr("SASL mechanism too weak for this account");
		else if (x == XMPP::ClientStream::NotAuthorized)
			s = tr("Not authorized");
		else if (x == XMPP::ClientStream::TemporaryAuthFailure)
			s = tr("Temporary auth failure");

		str = tr("Authentication error: %1").arg(s);
	}
	else if (err == XMPP::ClientStream::ErrSecurityLayer)
		str = tr("Broken security layer (SASL)");
	else
		str = tr("None");
	//printf("str[%s], reconn=%d\n", str.latin1(), reconn);
	*_str = str;
	*_reconn = reconn;
}

}
