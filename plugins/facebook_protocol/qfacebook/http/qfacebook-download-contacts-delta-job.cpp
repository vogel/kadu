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

#include "qfacebook-download-contacts-delta-job.h"

#include "qfacebook/exceptions/qfacebook-invalid-data-exception.h"
#include "qfacebook/http/qfacebook-download-contacts-delta-result.h"
#include "qfacebook/http/qfacebook-http-api.h"
#include "qfacebook/http/qfacebook-http-reply.h"
#include "qfacebook/http/qfacebook-http-request.h"
#include "qfacebook/qfacebook-contact.h"
#include "qfacebook/session/qfacebook-session-token.h"

#include <QtCore/QJsonArray>

namespace
{
QByteArray parseRemoved(const QByteArray &removed)
{
    auto parsed = QByteArray::fromBase64(removed).split(':');
    if (parsed.length() != 4)
        return {};

    if (parsed[0] != "contact")
        return {};

    return parsed[2];
}
}

QFacebookDownloadContactsDeltaJob::QFacebookDownloadContactsDeltaJob(
    QFacebookHttpApi &httpApi, QFacebookSessionToken facebookSessionToken, QByteArray deltaCursor, QObject *parent)
        : QObject{parent}, m_httpApi{httpApi}
{
    auto reply = m_httpApi.usersQueryDelta(facebookSessionToken.accessToken(), std::move(deltaCursor));
    connect(reply, &QFacebookHttpReply::finished, this, &QFacebookDownloadContactsDeltaJob::replyFinished);
}

QFacebookDownloadContactsDeltaJob::~QFacebookDownloadContactsDeltaJob()
{
}

void QFacebookDownloadContactsDeltaJob::replyFinished(
    const std::experimental::optional<QFacebookJsonReader> &result) try
{
    deleteLater();

    if (!result)
        return;

    auto deltas = result->readObject("viewer").readObject("messenger_contacts").readObject("deltas");
    auto pageInfo = deltas.readObject("page_info");
    if (pageInfo.readBool("has_next_page"))
    {
        emit finished(
            QFacebookDownloadContactsDeltaResult{QFacebookDownloadContactsDeltaStatus::ErrorManyPages, {}, {}, {}});
        return;
    }

    auto nodes = deltas.readArray("nodes");
    auto changes = std::vector<QFacebookJsonReader>{std::begin(nodes), std::end(nodes)};
    auto split = std::partition(
        std::begin(changes), std::end(changes), [](const QFacebookJsonReader &v) { return v.hasObject("added"); });

    auto friends = std::vector<QFacebookJsonReader>{};
    std::copy_if(std::begin(changes), split, std::back_inserter(friends), [](const QFacebookJsonReader &v) {
        return v.readObject("added").readObject("represented_profile").readString("friendship_status") == "ARE_FRIENDS";
    });
    std::transform(
        std::begin(friends), std::end(friends), std::back_inserter(m_added),
        [](const QFacebookJsonReader &v) { return QFacebookContact::fromJson(v.readObject("added")); });

    auto allRemoved = std::vector<QByteArray>{};
    std::transform(split, std::end(changes), std::back_inserter(allRemoved), [](const QFacebookJsonReader &v) {
        return parseRemoved(v.readString("removed").toUtf8());
    });
    std::copy_if(std::begin(allRemoved), std::end(allRemoved), std::back_inserter(m_removed), [](const QByteArray &b) {
        return !b.isEmpty();
    });

    emit finished(
        QFacebookDownloadContactsDeltaResult{QFacebookDownloadContactsDeltaStatus::OK,
                                             pageInfo.readString("end_cursor").toUtf8(), m_added, m_removed});
}
catch (QFacebookInvalidDataException &)
{
}
