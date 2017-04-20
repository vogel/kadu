/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "http-avatar-downloader.h"
#include "http-avatar-downloader.moc"

#include <QtGui/QImage>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

HttpAvatarDownloader::HttpAvatarDownloader(ContactAvatarId id, QString url, QObject *parent)
        : QObject{parent}, m_reply{}, m_redirectCount{0}, m_id{std::move(id)}
{
    m_nam = make_owned<QNetworkAccessManager>(this);
    fetch(std::move(url));
}

HttpAvatarDownloader::~HttpAvatarDownloader()
{
}

void HttpAvatarDownloader::done(QByteArray avatar)
{
    emit downloaded(m_id, std::move(avatar));
    deleteLater();
}

void HttpAvatarDownloader::failed()
{
    deleteLater();
}

void HttpAvatarDownloader::requestFinished()
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

void HttpAvatarDownloader::fetch(QString url)
{
    QNetworkRequest request;
    request.setUrl(std::move(url));

    m_reply = m_nam->get(request);
    connect(m_reply, SIGNAL(finished()), this, SLOT(requestFinished()));
}

void HttpAvatarDownloader::parseReply()
{
    done(m_reply->readAll());
}
