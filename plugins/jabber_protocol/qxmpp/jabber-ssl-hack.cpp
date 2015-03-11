/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "jabber-ssl-hack.h"

#include "jabber-public-sender-qobject.h"

#include "core/core.h"
#include "ssl/ssl-certificate-manager.h"

#include <QtNetwork/QSslError>
#include <QtNetwork/QSslSocket>

JabberSslHack::JabberSslHack(QXmppClient *parent) :
		QObject{parent}
{
	connect(parent, SIGNAL(stateChanged(QXmppClient::State)), this, SLOT(stateChanged(QXmppClient::State)));
}

JabberSslHack::~JabberSslHack()
{
}

void JabberSslHack::stateChanged(QXmppClient::State state)
{
	if (state != QXmppClient::State::ConnectingState)
		return;

	auto xmppClient = static_cast<JabberPublicSenderQObject *>(sender());
	auto sslSocket = qobject_cast<QSslSocket *>(xmppClient->publicSender());

	if (sslSocket)
		connect(sslSocket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslErrors(QList<QSslError>)), Qt::UniqueConnection);
}

void JabberSslHack::sslErrors(const QList<QSslError> &errors)
{
	Q_UNUSED(errors);

	auto client = static_cast<QXmppClient *>(parent());
	auto sslSocket = qobject_cast<QSslSocket *>(sender());
	if (Core::instance()->sslCertificateManager()->acceptCertificate(client->configuration().domain(), sslSocket->peerCertificate(), errors))
		sslSocket->ignoreSslErrors();
}
