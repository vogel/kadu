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

#include "qfacebook-publish-mark-thread.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

QByteArray QFacebookPublishMarkThread::encode()
{
    auto json = QJsonObject{};
    json.insert("state", state);
    json.insert("syncSeqId", syncSeqId);
    json.insert("mark", QString::fromUtf8(mark));
    json.insert("otherUserFbId", static_cast<double>(otherUserFbId));

    auto document = QJsonDocument{};
    document.setObject(json);

    return document.toJson(QJsonDocument::Compact);
}
