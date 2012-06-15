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

#include <QtGui/QApplication>

#include <xmpp.h>

#include "jabber-error-helper.h"

void JabberErrorHelper::getErrorInfo(int err, XMPP::AdvancedConnector *conn, XMPP::Stream *stream, XMPP::QCATLSHandler *tlsHandler, QString *_str, bool *_reconn)
{
	QString str;
	bool reconn = false;

	if (err == -1)
	{
		str = qApp->translate("@default", "Disconnected");
		reconn = true;
	}
	else if (err == XMPP::ClientStream::ErrParse)
	{
		str = qApp->translate("@default", "XML Parsing Error");
		reconn = true;
	}
	else if (err == XMPP::ClientStream::ErrProtocol)
	{
		str = qApp->translate("@default", "XMPP Protocol Error");
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
			s = qApp->translate("@default", "Generic stream error");
		else if (x == XMPP::ClientStream::Conflict)
		{
			s = qApp->translate("@default", "Conflict(remote login replacing this one)");
			reconn = false;
		}
		else if (x == XMPP::ClientStream::ConnectionTimeout)
			s = qApp->translate("@default", "Timed out from inactivity");
		else if (x == XMPP::ClientStream::InternalServerError)
			s = qApp->translate("@default", "Internal server error");
		else if (x == XMPP::ClientStream::InvalidXml)
			s = qApp->translate("@default", "Invalid XML");
		else if (x == XMPP::ClientStream::PolicyViolation)
		{
			s = qApp->translate("@default", "Policy violation");
			reconn = false;
		}
		else if (x == XMPP::ClientStream::ResourceConstraint)
		{
			s = qApp->translate("@default", "Server out of resources");
			reconn = false;
		}
		else if (x == XMPP::ClientStream::SystemShutdown)
		{
			s = qApp->translate("@default", "Server is shutting down");
		}
		str = qApp->translate("@default", "XMPP Stream Error: %1").arg(s) + '\n' + detail;
	}
	else if (err == XMPP::ClientStream::ErrConnection)
	{
		int x = conn->errorCode();
		QString s;
		reconn = true;
		if (x == XMPP::AdvancedConnector::ErrConnectionRefused)
			s = qApp->translate("@default", "Unable to connect to server");
		else if (x == XMPP::AdvancedConnector::ErrHostNotFound)
			s = qApp->translate("@default", "Host not found");
		else if (x == XMPP::AdvancedConnector::ErrProxyConnect)
			s = qApp->translate("@default", "Error connecting to proxy");
		else if (x == XMPP::AdvancedConnector::ErrProxyNeg)
			s = qApp->translate("@default", "Error during proxy negotiation");
		else if (x == XMPP::AdvancedConnector::ErrProxyAuth)
		{
			s = qApp->translate("@default", "Proxy authentication failed");
			reconn = false;
		}
		else if (x == XMPP::AdvancedConnector::ErrStream)
			s = qApp->translate("@default", "Socket/stream error");
		str = qApp->translate("@default", "Connection Error: %1").arg(s);
	}
	else if (err == XMPP::ClientStream::ErrNeg)
	{
		QString s, detail;
		int x = stream->errorCondition();
		detail = stream->errorText();
		if (x  == XMPP::ClientStream::HostGone)
			s = qApp->translate("@default", "Host no longer hosted");
		else if (x == XMPP::ClientStream::HostUnknown)
			s = qApp->translate("@default", "Host unknown");
		else if (x == XMPP::ClientStream::RemoteConnectionFailed)
		{
			s = qApp->translate("@default", "A required remote connection failed");
			reconn = true;
		}
		else if (x == XMPP::ClientStream::SeeOtherHost)
			s = qApp->translate("@default", "See other host: %1").arg(stream->errorText());
		else if (x == XMPP::ClientStream::UnsupportedVersion)
			s = qApp->translate("@default", "Server does not support proper XMPP version");
		str = qApp->translate("@default", "Stream Negotiation Error: %1").arg(s) + '\n' + detail;
	}
	else if (err == XMPP::ClientStream::ErrTLS)
	{
		int x = stream->errorCondition();
		QString s;
		if (x == XMPP::ClientStream::TLSStart)
			s = qApp->translate("@default", "Server rejected STARTTLS");
		else if (x == XMPP::ClientStream::TLSFail)
		{
			int t = tlsHandler->tlsError();
			if (t == QCA::TLS::ErrorHandshake)
				s = qApp->translate("@default", "TLS handshake error");
			else
				s = qApp->translate("@default", "Broken security layer (TLS)");
		}
		str = s;
	}
	else if (err == XMPP::ClientStream::ErrAuth)
	{
		int x = stream->errorCondition();
		QString s;
		if (x == XMPP::ClientStream::GenericAuthError)
			s = qApp->translate("@default", "Unable to login");
		else if (x == XMPP::ClientStream::NoMech)
		{
			s = qApp->translate("@default", "No appropriate mechanism available for given security settings(e.g. SASL library too weak, or plaintext authentication not enabled)");
			s += '\n' + stream->errorText();
		}
		else if (x == XMPP::ClientStream::BadProto)
			s = qApp->translate("@default", "Bad server response");
		else if (x == XMPP::ClientStream::BadServ)
			s = qApp->translate("@default", "Server failed mutual authentication");
		else if (x == XMPP::ClientStream::EncryptionRequired)
			s = qApp->translate("@default", "Encryption required for chosen SASL mechanism");
		else if (x == XMPP::ClientStream::InvalidAuthzid)
			s = qApp->translate("@default", "Invalid account information");
		else if (x == XMPP::ClientStream::InvalidMech)
			s = qApp->translate("@default", "Invalid SASL mechanism");
		else if (x == XMPP::ClientStream::InvalidRealm)
			s = qApp->translate("@default", "Invalid realm");
		else if (x == XMPP::ClientStream::MechTooWeak)
			s = qApp->translate("@default", "SASL mechanism too weak for this account");
		else if (x == XMPP::ClientStream::NotAuthorized)
			s = qApp->translate("@default", "Not authorized");
		else if (x == XMPP::ClientStream::TemporaryAuthFailure)
			s = qApp->translate("@default", "Temporary auth failure");

		str = qApp->translate("@default", "Authentication error: %1").arg(s);
	}
	else if (err == XMPP::ClientStream::ErrSecurityLayer)
		str = qApp->translate("@default", "Broken security layer (SASL)");
	else
		str = qApp->translate("@default", "None");
	//printf("str[%s], reconn=%d\n", str.latin1(), reconn);
	*_str = str;
	*_reconn = reconn;
}
