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

#pragma once

#include <QtCore/QByteArray>

class QFacebookJsonReader;

class QFacebookSessionToken
{
public:
    static QFacebookSessionToken fromJson(const QFacebookJsonReader &json);

    QFacebookSessionToken() = default;
    QFacebookSessionToken(
        QByteArray sessionKey, long uid, QByteArray secret, QByteArray accessToken, QByteArray machineId,
        QByteArray userStorageKey)
            : m_sessionKey{std::move(sessionKey)}, m_uid{uid}, m_secret{std::move(secret)},
              m_accessToken{std::move(accessToken)}, m_machineId{std::move(machineId)},
              m_userStorageKey{std::move(userStorageKey)}
    {
    }

    QByteArray sessionKey() const
    {
        return m_sessionKey;
    }
    long uid() const
    {
        return m_uid;
    }
    QByteArray secret() const
    {
        return m_secret;
    }
    QByteArray accessToken() const
    {
        return m_accessToken;
    }
    QByteArray machineId() const
    {
        return m_machineId;
    }
    QByteArray userStorageKey() const
    {
        return m_userStorageKey;
    }

private:
    QByteArray m_sessionKey;
    long m_uid;
    QByteArray m_secret;
    QByteArray m_accessToken;
    QByteArray m_machineId;
    QByteArray m_userStorageKey;
};
