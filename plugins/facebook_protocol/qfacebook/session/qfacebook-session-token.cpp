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

#include "qfacebook/session/qfacebook-session-token.h"

#include "qfacebook/qfacebook-json-reader.h"

QFacebookSessionToken QFacebookSessionToken::fromJson(const QFacebookJsonReader &json)
{
    return QFacebookSessionToken{json.readString("session_key").toUtf8(), json.readLong("uid"),
                                 json.readString("secret").toUtf8(),      json.readString("access_token").toUtf8(),
                                 json.readString("machine_id").toUtf8(),  json.readString("user_storage_key").toUtf8()};
}
