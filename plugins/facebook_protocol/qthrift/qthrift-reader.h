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

#include "qthrift.h"

#include <QtCore/QByteArray>
#include <stdint.h>

namespace QThrift
{
class Reader
{
public:
    explicit Reader(const QByteArray &data);

    template <typename T>
    T readValue();

    QByteArray readString();

private:
    const QByteArray &m_data;
    int m_lastBool{0};
    int m_pos{0};

    FieldHeader readHeader(int16_t lastId);
    Value readValue(FieldType type);

    uint8_t readUInt8();
    int64_t readInt64();
    uint32_t readVInt32();
    uint64_t readVInt64();

    Value readList();
    template <typename T>
    List<T> readList(uint32_t size);
};
}
