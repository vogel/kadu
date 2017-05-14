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

#include "qfacebook-publish-inbox.h"

#include "qfacebook/qfacebook-json-reader.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

QFacebookPublishInbox QFacebookPublishInbox::decode(const QByteArray &content)
{
    auto result = QFacebookPublishInbox{};
    auto jsonReader = QFacebookJsonReader{content};
    result.unseen = jsonReader.readInt("unseen");
    result.unread = jsonReader.readInt("unread");
    result.recentUnread = jsonReader.readInt("recent_unread");
    result.seenTimestamp = jsonReader.readLongLong("seen_timestamp");
    result.realtimeViewerFbId = jsonReader.readUid("realtime_viewer_fbid");

    return result;
}
