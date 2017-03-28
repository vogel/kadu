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

#pragma once

#include <QtCore/QByteArray>
#include <experimental/optional>
#include <map>

class QFacebookHttpRequest
{
public:
    explicit QFacebookHttpRequest(
        QByteArray url, std::experimental::optional<QByteArray> token, QByteArray method, QByteArray friendlyName,
        std::map<QByteArray, QByteArray> parameters);

    QByteArray url() const
    {
        return m_url;
    }
    std::experimental::optional<QByteArray> token() const
    {
        return m_token;
    }
    QByteArray method() const
    {
        return m_method;
    }
    QByteArray friendlyName() const
    {
        return m_friendlyName;
    }
    std::map<QByteArray, QByteArray> parameters() const
    {
        return m_parameters;
    }

private:
    QByteArray m_url;
    std::experimental::optional<QByteArray> m_token;
    QByteArray m_method;
    QByteArray m_friendlyName;
    std::map<QByteArray, QByteArray> m_parameters;
};
