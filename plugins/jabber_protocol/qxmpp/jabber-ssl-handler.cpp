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

#include "ssl/ssl-certificate-manager.h"

#include <QtNetwork/QSslError>
#include <QtNetwork/QSslSocket>

JabberSslHandler::JabberSslHandler(
    QXmppClient *parent, const std::function<void()> &onAccepted, const std::function<void()> &onRejected)
        : QObject{parent}, m_onAccepted{onAccepted}, m_onRejected{onRejected}
{
    parent->configuration().setIgnoreSslErrors(false);

    connect(parent, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslErrors(QList<QSslError>)));
}

JabberSslHandler::~JabberSslHandler()
{
}

void JabberSslHandler::setSslCertificateManager(SslCertificateManager *sslCertificateManager)
{
    m_sslCertificateManager = sslCertificateManager;
}

void JabberSslHandler::sslErrors(const QList<QSslError> &errors)
{
    auto client = static_cast<QXmppClient *>(parent());
    if (errors.size() == 0 ||
        m_sslCertificateManager->acceptCertificate(client->configuration().domain(), errors.first().certificate()))
        client->configuration().setIgnoreSslErrors(true);
    else
        m_sslCertificateManager->askForCertificateAcceptance(
            client->configuration().domain(), errors.first().certificate(), errors, m_onAccepted, m_onRejected);
}

#include "moc_jabber-ssl-handler.cpp"
