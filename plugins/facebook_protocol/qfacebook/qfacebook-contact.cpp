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

#include "qfacebook-contact.h"

#include "qfacebook/qfacebook-json-reader.h"

QFacebookContact QFacebookContact::fromJson(const QFacebookJsonReader &json)
{
    return QFacebookContact{json.readObject("represented_profile").readString("id").toUtf8(),
                            json.readObject("structured_name").readString("text"),
                            json.readObject("hugePictureUrl").readString("uri").toUtf8()};
}

QFacebookContact::QFacebookContact(QByteArray id, QString name, QByteArray avatarUrl)
        : m_id{std::move(id)}, m_name{std::move(name)}, m_avatarUrl{std::move(avatarUrl)}
{
}
