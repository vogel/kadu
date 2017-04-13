/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gadu-account-avatar-service.h"
#include "gadu-account-avatar-service.moc"

#include "oauth/oauth-manager.h"
#include "oauth/oauth-token-fetcher.h"

#include <QtCore/QBuffer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

GaduAccountAvatarService::GaduAccountAvatarService(Account account, QObject *parent) : AccountAvatarService{account, parent}
{
    m_oauth = make_owned<OAuthManager>(this);
    m_network = make_owned<QNetworkAccessManager>(this);
    connect(m_oauth, &OAuthManager::authorized, this, &GaduAccountAvatarService::authorized);
}

void GaduAccountAvatarService::upload(const QPixmap &avatar)
{
    m_avatar = avatar;
    m_oauth->authorize(OAuthConsumer(account().id().toUtf8(), account().password().toUtf8()));
}

void GaduAccountAvatarService::authorized(OAuthToken token)
{
    if (!token.isValid())
    {
        emit finished(false);
        return;
    }

    QBuffer avatarBuffer{};
    avatarBuffer.open(QIODevice::WriteOnly);
    m_avatar.save(&avatarBuffer, "PNG");
    avatarBuffer.close();

    auto url = QByteArray{"http://avatars.nowe.gg/upload"};
    auto payload = QByteArray{"uin=" + QUrl::toPercentEncoding(account().id())};
    payload += "&photo=";
    payload += QUrl::toPercentEncoding(avatarBuffer.buffer().toBase64());

    auto putAvatarRequest = QNetworkRequest{};
    putAvatarRequest.setUrl(QString{url});
    putAvatarRequest.setHeader(QNetworkRequest::ContentTypeHeader, QByteArray{"application/x-www-form-urlencoded"});

    putAvatarRequest.setRawHeader("Authorization", token.token());
    putAvatarRequest.setRawHeader("From", "avatars to avatars");

    auto reply = m_network->post(putAvatarRequest, payload);
    connect(reply, &QNetworkReply::finished, this, &GaduAccountAvatarService::transferFinished);
}

void GaduAccountAvatarService::transferFinished()
{
    auto reply = static_cast<QNetworkReply *>(sender());
    emit finished(QNetworkReply::NoError == reply->error());
    reply->deleteLater();
}
