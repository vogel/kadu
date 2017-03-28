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

#include "qfacebook-publish-orca-message-notifications.h"

#include "qfacebook/qfacebook-json-reader.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

QFacebookPublishOrcaMessageNotifications QFacebookPublishOrcaMessageNotifications::decode(const QByteArray &content)
{
    auto result = QFacebookPublishOrcaMessageNotifications{};
    auto jsonReader = QFacebookJsonReader{content};
    result.otherUserFbid = jsonReader.readString("other_user_fbid").toLongLong();
    result.senderFbid = jsonReader.readUid("sender_fbid");
    result.timestamp = jsonReader.readLongLong("timestamp");
    result.offlineThreadingId = jsonReader.readString("offline_threading_id").toLongLong();
    result.body = jsonReader.readString("body");

    return result;
}
