/*
 * %kadu copyright begin%
 * Copyright 2017 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * Based on bitlbee-facebook by James Geboski (jgeboski@gmail.com) and
 * dequis <dx@dxzone.com.ar>.
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

#include "qfacebook-download-unread-messages-job.h"

#include "qfacebook-download-unread-messages-result.h"
#include "qfacebook-http-api.h"
#include "qfacebook-http-reply.h"
#include "qfacebook-http-request.h"
#include "qfacebook/exceptions/qfacebook-invalid-data-exception.h"
#include "qfacebook/session/qfacebook-session-token.h"

#include <QtCore/QJsonArray>

QFacebookDownloadUnreadMessagesJob::QFacebookDownloadUnreadMessagesJob(
    QFacebookHttpApi &httpApi, QFacebookSessionToken facebookSessionToken, QFacebookUid uid, int unreadCount,
    QObject *parent)
        : QObject{parent}, m_httpApi{httpApi}, m_facebookSession{std::move(facebookSessionToken)}
{
    auto reply = m_httpApi.unreadMessagesListQuery(m_facebookSession.accessToken(), uid, unreadCount);
    connect(reply, &QFacebookHttpReply::finished, this, &QFacebookDownloadUnreadMessagesJob::replyFinished);
}

QFacebookDownloadUnreadMessagesJob::~QFacebookDownloadUnreadMessagesJob()
{
}

void QFacebookDownloadUnreadMessagesJob::replyFinished(
    const std::experimental::optional<QFacebookJsonReader> &result) try
{
    deleteLater();

    if (!result)
        return;

    auto unreadMessagesResult = QFacebookDownloadUnreadMessagesResult{};

    auto key = result->keys()[0];
    auto nodes = result->readObject(key).readObject("messages").readArray("nodes");
    std::transform(
        std::begin(nodes), std::end(nodes), std::back_inserter(unreadMessagesResult.unreadMessages),
        [](const QFacebookJsonReader &v) {
            return QFacebookDownloadUnreadMessageResult{
                v.readObject("message_sender").readObject("messaging_actor").readString("id").toLongLong(),
                v.readObject("message").readString("text"),
                QDateTime::fromMSecsSinceEpoch(v.readString("timestamp_precise").toLongLong())};
        });

    emit finished(unreadMessagesResult);
}
catch (QFacebookInvalidDataException &)
{
}
