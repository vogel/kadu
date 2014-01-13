/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2009, 2010, 2010, 2011, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
		str = QCoreApplication::translate("@default", "Disconnected");
		reconn = true;
	}
	else if (err == XMPP::ClientStream::ErrParse)
	{
		str = QCoreApplication::translate("@default", "XML Parsing Error");
		reconn = true;
	}
	else if (err == XMPP::ClientStream::ErrProtocol)
	{
		str = QCoreApplication::translate("@default", "XMPP Protocol Error");
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
			s = QCoreApplication::translate("@default", "Generic stream error");
		else if (x == XMPP::ClientStream::Conflict)
		{
			s = QCoreApplication::translate("@default", "Conflict(remote login replacing this one)");
			reconn = false;
		}
		else if (x == XMPP::ClientStream::ConnectionTimeout)
			s = QCoreApplication::translate("@default", "Timed out from inactivity");
		else if (x == XMPP::ClientStream::InternalServerError)
			s = QCoreApplication::translate("@default", "Internal server error");
		else if (x == XMPP::ClientStream::InvalidXml)
			s = QCoreApplication::translate("@default", "Invalid XML");
		else if (x == XMPP::ClientStream::PolicyViolation)
		{
			s = QCoreApplication::translate("@default", "Policy violation");
			reconn = false;
		}
		else if (x == XMPP::ClientStream::ResourceConstraint)
		{
			s = QCoreApplication::translate("@default", "Server out of resources");
			reconn = false;
		}
		else if (x == XMPP::ClientStream::SystemShutdown)
		{
			s = QCoreApplication::translate("@default", "Server is shutting down");
		}
		str = QCoreApplication::translate("@default", "XMPP Stream Error: %1").arg(s) + '\n' + detail;
	}
	else if (err == XMPP::ClientStream::ErrConnection)
	{
		int x = conn->errorCode();
		QString s;
		reconn = true;
		if (x == XMPP::AdvancedConnector::ErrConnectionRefused)
			s = QCoreApplication::translate("@default", "Unable to connect to server");
		else if (x == XMPP::AdvancedConnector::ErrHostNotFound)
			s = QCoreApplication::translate("@default", "Host not found");
		else if (x == XMPP::AdvancedConnector::ErrProxyConnect)
			s = QCoreApplication::translate("@default", "Error connecting to proxy");
		else if (x == XMPP::AdvancedConnector::ErrProxyNeg)
			s = QCoreApplication::translate("@default", "Error during proxy negotiation");
		else if (x == XMPP::AdvancedConnector::ErrProxyAuth)
		{
			s = QCoreApplication::translate("@default", "Proxy authentication failed");
			reconn = false;
		}
		else if (x == XMPP::AdvancedConnector::ErrStream)
			s = QCoreApplication::translate("@default", "Socket/stream error");
		str = QCoreApplication::translate("@default", "Connection Error: %1").arg(s);
	}
	else if (err == XMPP::ClientStream::ErrNeg)
	{
		QString s, detail;
		int x = stream->errorCondition();
		detail = stream->errorText();
		if (x  == XMPP::ClientStream::HostGone)
			s = QCoreApplication::translate("@default", "Host no longer hosted");
		else if (x == XMPP::ClientStream::HostUnknown)
			s = QCoreApplication::translate("@default", "Host unknown");
		else if (x == XMPP::ClientStream::RemoteConnectionFailed)
		{
			s = QCoreApplication::translate("@default", "A required remote connection failed");
			reconn = true;
		}
		else if (x == XMPP::ClientStream::SeeOtherHost)
			s = QCoreApplication::translate("@default", "See other host: %1").arg(stream->errorText());
		else if (x == XMPP::ClientStream::UnsupportedVersion)
			s = QCoreApplication::translate("@default", "Server does not support proper XMPP version");
		str = QCoreApplication::translate("@default", "Stream Negotiation Error: %1").arg(s) + '\n' + detail;
	}
	else if (err == XMPP::ClientStream::ErrTLS)
	{
		int x = stream->errorCondition();
		QString s;
		if (x == XMPP::ClientStream::TLSStart)
			s = QCoreApplication::translate("@default", "Server rejected STARTTLS");
		else if (x == XMPP::ClientStream::TLSFail)
		{
			int t = tlsHandler->tlsError();
			if (t == QCA::TLS::ErrorHandshake)
				s = QCoreApplication::translate("@default", "TLS handshake error");
			else
				s = QCoreApplication::translate("@default", "Broken security layer (TLS)");
		}
		str = s;
	}
	else if (err == XMPP::ClientStream::ErrAuth)
	{
		int x = stream->errorCondition();
		QString s;
		if (x == XMPP::ClientStream::GenericAuthError)
			s = QCoreApplication::translate("@default", "Unable to login");
		else if (x == XMPP::ClientStream::NoMech)
		{
			s = QCoreApplication::translate("@default", "No appropriate mechanism available for given security settings(e.g. SASL library too weak, or plaintext authentication not enabled)");
			s += '\n' + stream->errorText();
		}
		else if (x == XMPP::ClientStream::BadProto)
			s = QCoreApplication::translate("@default", "Bad server response");
		else if (x == XMPP::ClientStream::BadServ)
			s = QCoreApplication::translate("@default", "Server failed mutual authentication");
		else if (x == XMPP::ClientStream::EncryptionRequired)
			s = QCoreApplication::translate("@default", "Encryption required for chosen SASL mechanism");
		else if (x == XMPP::ClientStream::InvalidAuthzid)
			s = QCoreApplication::translate("@default", "Invalid account information");
		else if (x == XMPP::ClientStream::InvalidMech)
			s = QCoreApplication::translate("@default", "Invalid SASL mechanism");
		else if (x == XMPP::ClientStream::InvalidRealm)
			s = QCoreApplication::translate("@default", "Invalid realm");
		else if (x == XMPP::ClientStream::MechTooWeak)
			s = QCoreApplication::translate("@default", "SASL mechanism too weak for this account");
		else if (x == XMPP::ClientStream::NotAuthorized)
			s = QCoreApplication::translate("@default", "Not authorized");
		else if (x == XMPP::ClientStream::TemporaryAuthFailure)
			s = QCoreApplication::translate("@default", "Temporary auth failure");

		str = QCoreApplication::translate("@default", "Authentication error: %1").arg(s);
	}
	else if (err == XMPP::ClientStream::ErrSecurityLayer)
		str = QCoreApplication::translate("@default", "Broken security layer (SASL)");
	else
		str = QCoreApplication::translate("@default", "None");
	//printf("str[%s], reconn=%d\n", str.latin1(), reconn);
	*_str = str;
	*_reconn = reconn;
}
