/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2010, 2011 Wojciech Treter (juzefwt@gmail.com)
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

#ifndef JABBER_CLIENT_H
#define JABBER_CLIENT_H

#include <QtCore/QObject>

// include these because of namespace reasons
#include <iris/im.h>
#include <iris/xmpp.h>

#include <QtCrypto>

#include "accounts/account.h"

#include "jabber-account-details.h"

namespace XMPP
{
	class AdvancedConnector;
}

class JabberProtocol;

/**
 * This class provides an interface to the Iris subsystem. The goal is to
 * abstract the Iris layer and manage it via a single, simple to use class.
 * By default, @ref JabberClient will attempt to establish a connection
 * using XMPP 1.0. This means that apart from the JID and password, no
 * further details are necessary to connect. The server and port will be
 * determined using a SRV lookup. If TLS is possible (meaning, the TLS
 * plugin is available and the server supports TLS) it will automatically
 * be used. Otherwise, a non-encrypted connection will be established.
 * If XMPP 1.0 is not possible, the connection will fall back to the old
 * protocol. By default, this connection is not encrypted. You can, however,
 * use @ref setUseSSL to immediately attempt an SSL connection. This is
 * most useful if you want to establish an SSL connection to a non-standard
 * port, in which case you will also have to use @ref setOverrideHost. In case
 * XMPP 1.0 does not work, an automatic attempt to connect to the standard port
 * 5223 with SSL can be made with @ref setProbeSSL. If the attempt is not
 * sucessful, the connection will fall back to an unencrypted attempt
 * at port 5222.
 * @brief Provides a Jabber client
 * @author Till Gerken
 */

namespace XMPP
{
	class JabberClient : public QObject
	{
		Q_OBJECT

		// XMPP backend
		XMPP::Client *Client;

		JabberProtocol *Protocol;

	private slots:
		/* Someone on our contact list had(another) resource come online. */
		void slotResourceAvailable(const Jid &, const Resource &);

		/* Someone on our contact list had a resource go offline. */
		void slotResourceUnavailable(const Jid &, const Resource &);

		/* Called from Psi: debug messages from the backend. */
		void slotIncomingXML(const QString &msg);
		void slotOutgoingXML(const QString &msg);

	public:
		/**
		 * Error codes indicating problems during operation.
		 */
		enum ErrorCode
		{
			Ok,					/** No error. */
			InvalidPassword,	/** Password used to connect to the server was incorrect. */
			AlreadyConnected,	/** A new connection was attempted while the previous one hasn't been closed. */
			NoTLS,				/** Use of TLS has been forced (see @ref forceTLS) but TLS is not available, either server- or client-side. */
			InvalidPasswordForMUC = 401,	/** A password is require to enter on this Multi-User Chat /
			NicknameConflict = 409,		** There is already someone with that nick connected to the Multi-User Chat /
			BannedFromThisMUC = 403,	** You can't join this Multi-User Chat because you were bannished */
			MaxUsersReachedForThisMuc = 503	/** You can't join this Multi-User Chat because it is full */
									};

		explicit JabberClient(XMPP::Client *client, QObject *parent = 0);
		virtual ~JabberClient();

		/**
		 * Disconnect from Jabber server with reason
		 * @param reason The reason for disconnecting
		 */
		void disconnect(XMPP::Status &reason);

		static void getErrorInfo(int err, AdvancedConnector *conn, Stream *stream, QCATLSHandler *tlsHandler, QString *_str, bool *_reconn);

	signals:
		/**
		 * Client stream was disconnected.
		 */
		void csDisconnected();

		/**
		 * New resource is available for a contact.
		 */
		void resourceAvailable(const XMPP::Jid &jid, const XMPP::Resource &resource);

		/**
		 * An existing resource has been removed.
		 */
		void resourceUnavailable(const XMPP::Jid &jid, const XMPP::Resource &resource);

		/**
		 * Dispatches a debug message. Debug messages
		 * include incoming and outgoing XML packets
		 * as well as internal status messages.
		 */
		void debugMessage(const QString &message);
		void incomingXML(const QString &msg);
		void outgoingXML(const QString &msg);
	};
}

#endif // JABBER_CLIENT_H
