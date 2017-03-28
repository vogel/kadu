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

#include "qfacebook-http-api.h"

#include "qfacebook-http-reply.h"
#include "qfacebook-http-request.h"

#include <QtCore/QCryptographicHash>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <algorithm>

QFacebookHttpApi::QFacebookHttpApi(QByteArray deviceId) : m_deviceId{std::move(deviceId)}
{
}

QFacebookHttpReply *QFacebookHttpApi::auth(const QString &user, const QString &password)
{
    auto parameters = std::map<QByteArray, QByteArray>{{"email", user.toUtf8()}, {"password", password.toUtf8()}};
    auto authRequest = QFacebookHttpRequest{"https://b-api.facebook.com/method/auth.login", std::experimental::nullopt,
                                            "auth.login", "authenticate", parameters};
    return get(authRequest);
}

QFacebookHttpReply *QFacebookHttpApi::usersQuery(const QByteArray &token)
{
    auto jsonString = QStringLiteral(R"({"0": ["user"], "1": "50"})");
    auto parameters =
        std::map<QByteArray, QByteArray>{{"query_id", "10154444360806729"}, {"query_params", jsonString.toUtf8()}};
    auto usersQueryRequest =
        QFacebookHttpRequest{"https://graph.facebook.com/graphql", token, "get", "UsersQuery", parameters};
    return get(usersQueryRequest);
}

QFacebookHttpReply *QFacebookHttpApi::usersQueryAfter(const QByteArray &token, const QString &cursor)
{
    auto jsonString = QString{R"({"0": ["user"], "1": "%1", "2": "50"})"}.arg(cursor);
    auto parameters =
        std::map<QByteArray, QByteArray>{{"query_id", "10154444360816729"}, {"query_params", jsonString.toUtf8()}};
    auto usersQueryRequest = QFacebookHttpRequest{"https://graph.facebook.com/graphql", token, "get",
                                                  "FetchContactsFullWithAfterQuery", parameters};
    return get(usersQueryRequest);
}

QFacebookHttpReply *QFacebookHttpApi::usersQueryDelta(const QByteArray &token, const QString &deltaCursor)
{
    auto jsonString = QString{R"({"0": "%1", "1": ["user"], "2": "500"})"}.arg(deltaCursor);
    auto parameters =
        std::map<QByteArray, QByteArray>{{"query_id", "10154444360801729"}, {"query_params", jsonString.toUtf8()}};
    auto usersQueryRequest =
        QFacebookHttpRequest{"https://graph.facebook.com/graphql", token, "get", "FetchContactsDeltaQuery", parameters};
    return get(usersQueryRequest);
}

QFacebookHttpReply *QFacebookHttpApi::threadListQuery(const QByteArray &token)
{
    auto jsonString = QString{R"({"1": "0"})"};
    auto parameters =
        std::map<QByteArray, QByteArray>{{"query_id", "10153919752026729"}, {"query_params", jsonString.toUtf8()}};
    auto usersQueryRequest =
        QFacebookHttpRequest{"https://graph.facebook.com/graphql", token, "get", "ThreadListQuery", parameters};
    return get(usersQueryRequest);
}

QFacebookHttpReply *QFacebookHttpApi::unreadThreadListQuery(const QByteArray &token, int unreadCount)
{
    auto jsonString = QString{R"({"1": "%1", "2": "true", "12": "true", "13": "false"})"}.arg(unreadCount);
    auto parameters =
        std::map<QByteArray, QByteArray>{{"query_id", "10153919752026729"}, {"query_params", jsonString.toUtf8()}};
    auto usersQueryRequest =
        QFacebookHttpRequest{"https://graph.facebook.com/graphql", token, "get", "ThreadListQuery", parameters};
    return get(usersQueryRequest);
}

QFacebookHttpReply *
QFacebookHttpApi::unreadMessagesListQuery(const QByteArray &token, QFacebookUid uid, int unreadCount)
{
    auto jsonString =
        QString{R"({"0": ["%1"], "10": "true", "11": "true", "12": "%2", "13": "false"})"}.arg(uid).arg(unreadCount);
    auto parameters =
        std::map<QByteArray, QByteArray>{{"query_id", "10153919752036729"}, {"query_params", jsonString.toUtf8()}};
    auto usersQueryRequest =
        QFacebookHttpRequest{"https://graph.facebook.com/graphql", token, "get", "ThreadQuery", parameters};
    return get(usersQueryRequest);
}

QFacebookHttpReply *QFacebookHttpApi::get(const QFacebookHttpRequest &request)
{
    auto authUrl = QStringLiteral();
    auto params = request.parameters();
    params.insert(std::make_pair("api_key", "256002347743983"));
    params.insert(std::make_pair("device_id", m_deviceId));
    params.insert(std::make_pair("fb_api_req_friendly_name", request.friendlyName()));
    params.insert(std::make_pair("format", "json"));
    params.insert(std::make_pair("method", request.method()));
    params.insert(std::make_pair("local", "pl_PL"));

    std::vector<QByteArray> keys;
    std::transform(
        std::begin(params), std::end(params), std::back_inserter(keys),
        [](const std::pair<QByteArray, QByteArray> &x) { return x.first; });
    std::sort(std::begin(keys), std::end(keys));

    QCryptographicHash md5sum{QCryptographicHash::Md5};
    std::for_each(std::begin(keys), std::end(keys), [&params, &md5sum](const QByteArray &x) {
        md5sum.addData(x);
        md5sum.addData(QByteArray{"="});
        md5sum.addData(params[x]);
    });
    md5sum.addData(QByteArray{"374e60f8b9bb6b8cbb30f78030438895"});

    params.insert(std::make_pair("sig", md5sum.result().toHex()));

    auto urlQuery = QUrlQuery{};
    for (auto const &i : params)
        urlQuery.addQueryItem(QString::fromUtf8(i.first), QString::fromUtf8(i.second));

    auto url = QUrl{request.url()};
    url.setQuery(urlQuery);

    auto httpRequest = QNetworkRequest{url};
    auto token = request.token();
    if (token)
        httpRequest.setRawHeader("Authorization", QByteArray{"OAuth "} + *token);

    return new QFacebookHttpReply{m_nam.get(httpRequest)};
}
