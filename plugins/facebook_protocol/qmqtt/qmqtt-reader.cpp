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

#include "qmqtt-reader.h"

#include <QtEndian>

std::experimental::optional<QMqttReader::QMqttHeader> QMqttReader::readHeader(const QByteArray &data)
{
    auto size = uint32_t{0};
    auto m = uint32_t{1};
    auto i = uint32_t{1};
    auto byte = uint8_t{0};
    do
    {
        if (data.size() < static_cast<int>(i + 1))
            return std::experimental::nullopt;

        byte = static_cast<uint8_t>(data[i]);
        size += (byte & 0x7F) * m;
        m <<= 7;
        i++;
    } while ((byte & 0x80) != 0);

    return QMqttHeader{size, i};
}

QMqttReader::QMqttReader(const QByteArray &content) : m_content{content}
{
}

uint8_t QMqttReader::readUint8()
{
    return static_cast<uint8_t>(m_content[m_pos++]);
}

uint16_t QMqttReader::readUint16()
{
    auto res = qFromBigEndian(*reinterpret_cast<const uint16_t *>(&(m_content.data()[m_pos])));
    m_pos += 2;
    return res;
}

QByteArray QMqttReader::readData()
{
    auto size = readUint16();
    auto res = m_content.mid(m_pos, size);
    m_pos += size;
    return res;
}

QByteArray QMqttReader::readRaw()
{
    auto res = m_content.mid(m_pos);
    m_pos = m_content.size();
    return res;
}
