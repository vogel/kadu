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
	class JabberProtocol;

	class JabberClient : public QObject
	{
		Q_OBJECT

		JabberProtocol *Protocol;

	public:
		explicit JabberClient(QObject *parent = 0);
		virtual ~JabberClient();

		static void getErrorInfo(int err, AdvancedConnector *conn, Stream *stream, QCATLSHandler *tlsHandler, QString *_str, bool *_reconn);
	};
}

#endif // JABBER_CLIENT_H
