/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

	//UserAccount::SSLFlag ssl_;
	bool isOld_;

	XMPP::XData fields;

	XMPP::Form convertFromXData(const XMPP::XData& xdata);
	XMPP::XData convertToXData(const XMPP::Form& form);

public:
	JabberServerRegisterAccount(const QString &server, const QString &username, const QString &password, bool legacySSLProbe, bool legacySSL, bool forceSSL, const QString &host, quint16 port);

	void performAction();

	const QString & jid() const { return Jid; }

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
