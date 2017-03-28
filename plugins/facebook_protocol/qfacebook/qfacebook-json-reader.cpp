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

#include "qfacebook-json-reader.h"

#include "qfacebook/exceptions/qfacebook-invalid-data-exception.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>

QFacebookJsonReader::QFacebookJsonReader(const QByteArray &content)
{
    auto error = QJsonParseError{};
    auto document = QJsonDocument::fromJson(content, &error);
    if (error.error != QJsonParseError::NoError)
        throw QFacebookInvalidDataException{};

    m_object = document.object();
    if (m_object.isEmpty())
        throw QFacebookInvalidDataException{};
}

QFacebookJsonReader::QFacebookJsonReader(const QJsonObject &object) : m_object{object}
{
}

bool QFacebookJsonReader::hasObject(const QString &name) const
{
    return m_object[name].isObject();
}

QFacebookJsonReader QFacebookJsonReader::readObject(const QString &name) const
{
    auto value = m_object[name];
    if (!value.isObject())
        throw QFacebookInvalidDataException{};

    return QFacebookJsonReader{value.toObject()};
}

std::vector<QFacebookJsonReader> QFacebookJsonReader::readArray(const QString &name) const
{
    auto value = m_object[name];
    if (!value.isArray())
        throw QFacebookInvalidDataException{};

    auto result = std::vector<QFacebookJsonReader>{};
    auto array = value.toArray();
    std::transform(std::begin(array), std::end(array), std::back_inserter(result), [](const QJsonValue &v) {
        if (!v.isObject())
            throw QFacebookInvalidDataException{};
        return QFacebookJsonReader{v.toObject()};
    });
    return result;
}

bool QFacebookJsonReader::readBool(const QString &name) const
{
    auto value = m_object[name];
    if (!value.isBool())
        throw QFacebookInvalidDataException{};

    return value.toBool();
}

bool QFacebookJsonReader::hasInt(const QString &name) const
{
    return m_object[name].isDouble();
}

int QFacebookJsonReader::readInt(const QString &name) const
{
    auto value = m_object[name];
    if (!value.isDouble())
        throw QFacebookInvalidDataException{};

    return value.toDouble();
}

long QFacebookJsonReader::readLong(const QString &name) const
{
    auto value = m_object[name];
    if (!value.isDouble())
        throw QFacebookInvalidDataException{};

    return static_cast<long>(value.toDouble());
}

long long QFacebookJsonReader::readLongLong(const QString &name) const
{
    auto value = m_object[name];
    if (!value.isDouble())
        throw QFacebookInvalidDataException{};

    return static_cast<long long>(value.toDouble());
}

QString QFacebookJsonReader::readString(const QString &name) const
{
    auto value = m_object[name];
    if (!value.isString())
        throw QFacebookInvalidDataException{};

    return value.toString();
}

QFacebookMsgId QFacebookJsonReader::readMsgId(const QString &name) const
{
    auto value = m_object[name];
    if (!value.isDouble())
        throw QFacebookInvalidDataException{};

    return static_cast<QFacebookMsgId>(value.toDouble());
}

QFacebookUid QFacebookJsonReader::readUid(const QString &name) const
{
    auto value = m_object[name];
    if (!value.isDouble())
        throw QFacebookInvalidDataException{};

    return static_cast<QFacebookUid>(value.toDouble());
}
