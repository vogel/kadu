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

#include "qthrift-reader.h"

#include "qthrift-integer-out-of-range-exception.h"
#include "qthrift-read-out-of-range-exception.h"
#include "qthrift-size-out-of-range-exception.h"
#include "qthrift-stop-expected-exception.h"
#include "qthrift-unknown-field-type-exception.h"
#include "qthrift.h"

namespace QThrift
{
Reader::Reader(const QByteArray &data) : m_data{data}
{
}

template <>
bool Reader::readValue<bool>();

template <>
int16_t Reader::readValue<int16_t>();

template <>
int32_t Reader::readValue<int32_t>();

template <>
Struct Reader::readValue<Struct>();

FieldHeader Reader::readHeader(int16_t lastId)
{
    auto byte = readUInt8();
    auto type = static_cast<FieldType>(byte & 0x0F);
    auto diff = (byte & uint8_t{0xF0}) >> 4;
    auto id = type != FieldType::Stop && diff == 0 ? readValue<int16_t>() : static_cast<int16_t>(lastId + diff);

    type = type == FieldType::Bool1 ? FieldType::Bool : type;
    if (type == FieldType::Bool1)
    {
        type = FieldType::Bool;
        m_lastBool = 0x05;
    }
    else if (type == FieldType::Bool)
        m_lastBool = 0x01;

    return {type, id};
}

Value Reader::readValue(FieldType type)
{
    switch (type)
    {
    case FieldType::Bool:
        return Value{readValue<bool>()};
    case FieldType::Int16:
        return Value{readValue<int16_t>()};
    case FieldType::Int32:
        return Value{readValue<int32_t>()};
    case FieldType::Int64:
        return Value{readInt64()};
    case FieldType::List:
        return readList();
    case FieldType::Struct:
        return Value{readValue<Struct>()};
    default:
        throw UnknownFieldTypeException{};
    }
}

template <>
bool Reader::readValue<bool>()
{
    if ((m_lastBool & 0x03) == 0x01)
    {
        auto r = (m_lastBool & 0x04) != 0;
        m_lastBool = 0;
        return r;
    }

    auto byte = readUInt8();
    m_lastBool = 0;
    return (byte & 0x0F) == 0x01;
}

template <>
int16_t Reader::readValue<int16_t>()
{
    return static_cast<int16_t>(readInt64());
}

template <>
int32_t Reader::readValue<int32_t>()
{
    return static_cast<int32_t>(readInt64());
}

template <>
Struct Reader::readValue<Struct>()
{
    auto result = Struct{};
    auto lastId = int16_t{0};
    while (true)
    {
        auto header = readHeader(lastId);
        if (header.type == FieldType::Stop)
            break;

        result.fields.insert(std::make_pair(header.id, readValue(header.type)));
        lastId = header.id;

        if (header.type == FieldType::List)
            break;
    }

    return result;
}

uint8_t Reader::readUInt8()
{
    if (m_pos >= m_data.size())
        throw ReadOutOfRangeException{};
    return static_cast<uint8_t>(m_data.at(m_pos++));
}

int64_t Reader::readInt64()
{
    auto x = readVInt64();
    x = (x >> 0x01) ^ -(x & 0x01);
    return *reinterpret_cast<int64_t *>(&x);
}

uint32_t Reader::readVInt32()
{
    return static_cast<uint32_t>(readVInt64());
}

uint64_t Reader::readVInt64()
{
    auto result = uint64_t{0};
    auto s = 0;
    while (true)
    {
        auto byte = readUInt8();
        result += static_cast<uint64_t>(byte & 0x7F) << s;
        s += 7;

        if ((byte & 0x80) == 0)
            break;
        if (s > 63)
            throw IntegerOutOfRangeException{};
    }

    return result;
}

Value Reader::readList()
{
    auto byte = readUInt8();
    auto type = static_cast<FieldType>(byte & 0x0F);
    auto size = static_cast<uint32_t>((byte & uint8_t{0xF0}) >> 4);
    if (size == 0x0F)
        size = readVInt32();

    switch (type)
    {
    case FieldType::Int32:
        return Value{readList<int32_t>(size)};
    case FieldType::Struct:
        return Value{readList<Struct>(size)};
    default:
        throw UnknownFieldTypeException{};
    }
}

template <typename T>
List<T> Reader::readList(uint32_t size)
{
    if (size > 1 << 20)
        throw SizeOutOfRangeException{};

    auto result = List<T>{};
    for (auto i = uint32_t{0}; i < size; i++)
        result.push_back(readValue<T>());

    auto header = readHeader(0);
    if (header.type != FieldType::Stop)
        throw StopExpectedException{};
    return result;
}

QByteArray Reader::readString()
{
    auto size = readVInt64();
    if (size > 1 << 20)
        throw SizeOutOfRangeException{};

    if (m_pos + size > m_data.size() + 1)
        throw ReadOutOfRangeException{};

    auto result = m_data.mid(m_pos, size);
    m_pos += size;
    return result;
}
}
