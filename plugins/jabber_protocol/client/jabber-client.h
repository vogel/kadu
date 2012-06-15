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

class JabberProtocol;

namespace XMPP
{
	class JabberClient : public QObject
	{
		Q_OBJECT

		XMPP::Client *Client;

		JabberProtocol *Protocol;

	private slots:
		void slotResourceAvailable(const Jid &, const Resource &);
		void slotResourceUnavailable(const Jid &, const Resource &);

	public:
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

		static void getErrorInfo(int err, AdvancedConnector *conn, Stream *stream, QCATLSHandler *tlsHandler, QString *_str, bool *_reconn);

	signals:
		void resourceAvailable(const XMPP::Jid &jid, const XMPP::Resource &resource);
		void resourceUnavailable(const XMPP::Jid &jid, const XMPP::Resource &resource);
	};
}

#endif // JABBER_CLIENT_H
