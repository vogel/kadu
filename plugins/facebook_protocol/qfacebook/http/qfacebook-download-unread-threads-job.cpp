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

#include "qfacebook-download-unread-threads-job.h"

#include "qfacebook-download-unread-threads-result.h"
#include "qfacebook-http-api.h"
#include "qfacebook-http-reply.h"
#include "qfacebook-http-request.h"
#include "qfacebook/exceptions/qfacebook-invalid-data-exception.h"
#include "qfacebook/session/qfacebook-session-token.h"

#include <QtCore/QJsonArray>

QFacebookDownloadUnreadThreadsJob::QFacebookDownloadUnreadThreadsJob(
    QFacebookHttpApi &httpApi, QFacebookSessionToken facebookSessionToken, int unreadCount, QObject *parent)
        : QObject{parent}, m_httpApi{httpApi}, m_facebookSession{std::move(facebookSessionToken)}
{
    auto reply = m_httpApi.unreadThreadListQuery(m_facebookSession.accessToken(), unreadCount);
    connect(reply, &QFacebookHttpReply::finished, this, &QFacebookDownloadUnreadThreadsJob::replyFinished);
}

QFacebookDownloadUnreadThreadsJob::~QFacebookDownloadUnreadThreadsJob()
{
}

void QFacebookDownloadUnreadThreadsJob::replyFinished(
    const std::experimental::optional<QFacebookJsonReader> &result) try
{
    deleteLater();

    if (!result)
        return;

    auto unreadThreadsResult = QFacebookDownloadUnreadThreadsResult{};
    auto messageThreads = result->readObject("viewer").readObject("message_threads");
    auto nodes = messageThreads.readArray("nodes");

    std::transform(
        std::begin(nodes), std::end(nodes), std::back_inserter(unreadThreadsResult.unreadThreads),
        [](const QFacebookJsonReader &v) {
            auto threadKey = v.readObject("thread_key");
            return QFacebookDownloadUnreadThreadResult{threadKey.readString("other_user_id").toLongLong(),
                                                       v.readInt("unread_count")};
        });

    emit finished(unreadThreadsResult);
}
catch (QFacebookInvalidDataException &)
{
}
