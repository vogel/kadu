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
#include <stdint.h>

namespace QThrift
{
enum class FieldType : int8_t;
struct FieldHeader;
struct Struct;

class Writer
{
    friend struct WriteVisitor;

public:
    explicit Writer(QByteArray &destination);

    void write(const Struct &s);

private:
    QByteArray &m_destination;
    int m_lastBool{0};

    void writeBool(bool x);
    void writeUInt8(uint8_t x);
    void writeInt16(int16_t x);
    void writeInt32(int32_t x);
    void writeInt64(int64_t x);
    void writeVInt32(uint32_t x);
    void writeVInt64(uint64_t x);
    void writeHeader(FieldHeader header, int16_t lastId);

    void write(bool x, int16_t id, int16_t lastId);
    void write(int32_t x, int16_t id, int16_t lastId);
    void write(int64_t x, int16_t id, int16_t lastId);
    void write(const QByteArray &x, int16_t id, int16_t lastId);

    void startStruct(int16_t id, int16_t lastId);
    void endStruct();

    void startList(FieldType type, int size, int16_t id, int16_t lastId);
    void endList();
};
}
