/*
 * %kadu copyright begin%
 * Copyright 2017 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "facebook-avatar-downloader.h"
#include "facebook-avatar-downloader.moc"

#include <QtGui/QImage>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

FacebookAvatarDownloader::FacebookAvatarDownloader(ContactAvatarId id, QObject *parent)
        : QObject{parent}, m_reply{}, m_redirectCount{0}, m_id{std::move(id)}
{
    m_nam = make_owned<QNetworkAccessManager>(this);
    fetch(m_id.id);
}

FacebookAvatarDownloader::~FacebookAvatarDownloader()
{
}

void FacebookAvatarDownloader::done(QByteArray avatar)
{
    emit downloaded(m_id, std::move(avatar));
    deleteLater();
}

void FacebookAvatarDownloader::failed()
{
    deleteLater();
}

void FacebookAvatarDownloader::requestFinished()
{
    auto redirect = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    m_reply->deleteLater();

    if (redirect.isNull())
    {
        parseReply();
        return;
    }

    if (m_redirectCount > 5)
    {
        failed();
        return;
    }

    m_redirectCount++;

    fetch(redirect.toString());
}

void FacebookAvatarDownloader::fetch(const QString &url)
{
    QNetworkRequest request;
    request.setUrl(url);

    m_reply = m_nam->get(request);
    connect(m_reply, SIGNAL(finished()), this, SLOT(requestFinished()));
}

void FacebookAvatarDownloader::parseReply()
{
    done(m_reply->readAll());
}
