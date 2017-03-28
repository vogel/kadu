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

#include "qfacebook-publish-send-message-response.h"

#include "qfacebook/qfacebook-json-reader.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

QFacebookPublishSendMessageResponse QFacebookPublishSendMessageResponse::decode(const QByteArray &content)
{
    auto result = QFacebookPublishSendMessageResponse{};
    auto jsonReader = QFacebookJsonReader{content};
    result.succeeded = jsonReader.readBool("succeeded");
    result.msgid = jsonReader.readMsgId("msgid");
    result.err = jsonReader.readInt("errno");
    result.isRetryable = jsonReader.readBool("isRetryable");

    return result;
}

QByteArray QFacebookPublishSendMessageResponse::encode()
{
    auto json = QJsonObject{};
    json.insert("succeeded", succeeded);
    json.insert("msgid", static_cast<double>(msgid));
    json.insert("errno", err);
    json.insert("isRetryable", isRetryable);

    auto document = QJsonDocument{};
    document.setObject(json);

    return document.toJson(QJsonDocument::Compact);
}
