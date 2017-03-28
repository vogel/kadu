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

#include "qfacebook-http-reply.h"
#include "qfacebook-http-reply.moc"

#include "qfacebook/exceptions/qfacebook-invalid-data-exception.h"
#include "qfacebook/qfacebook-json-reader.h"

#include <QtNetwork/QNetworkReply>

QFacebookHttpReply::QFacebookHttpReply(owned_qptr<QNetworkReply> networkReply) : m_networkReply{std::move(networkReply)}
{
    connect(m_networkReply, &QNetworkReply::finished, this, &QFacebookHttpReply::httpReplyFinished);
}

QFacebookHttpReply::~QFacebookHttpReply()
{
    m_networkReply->deleteLater();
}

void QFacebookHttpReply::httpReplyFinished()
{
    deleteLater();

    if (m_networkReply->error() != 0)
    {
        emit finished({});
        return;
    }

    auto content = m_networkReply->readAll();
    try
    {
        auto reader = QFacebookJsonReader{content};
        emit finished(reader);
    }
    catch (QFacebookInvalidDataException &)
    {
        emit finished({});
        return;
    }
}
