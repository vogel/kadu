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

#include "jabber-ssl-handler.h"

#include "core/core.h"
#include "ssl/ssl-certificate-manager.h"

#include <QtNetwork/QSslError>
#include <QtNetwork/QSslSocket>

JabberSslHandler::JabberSslHandler(QXmppClient *parent) :
		QObject{parent}
{
	parent->configuration().setIgnoreSslErrors(false);

	connect(parent, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslErrors(QList<QSslError>)));
}

JabberSslHandler::~JabberSslHandler()
{
}

void JabberSslHandler::sslErrors(const QList<QSslError> &errors)
{
	auto client = static_cast<QXmppClient *>(parent());
	if (errors.size() == 0 || Core::instance()->sslCertificateManager()->acceptCertificate(client->configuration().domain(), errors.first().certificate(), errors))
		client->configuration().setIgnoreSslErrors(true);
}
