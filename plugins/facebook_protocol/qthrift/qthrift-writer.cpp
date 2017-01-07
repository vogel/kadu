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

#include "qthrift-writer.h"

#include "qthrift.h"

namespace QThrift
{

Writer::Writer(QByteArray &destination) :
		m_destination{destination}
{
}

void Writer::writeBool(bool x)
{
	if ((m_lastBool & 0x03) != 0x02)
	{
		writeUInt8(x ? 0x01 : 0x02);
		return;
	}

	auto pos = m_lastBool >> 3;
	m_lastBool = 0;

	if (pos < m_destination.size())
	{
		m_destination[pos] = m_destination[pos] & ~0x0F;
		m_destination[pos] = m_destination[pos] | (x ? 0x01 : 0x02);
	}
}

void Writer::writeUInt8(uint8_t x)
{
	m_destination.append(x);
}

void Writer::writeInt16(int16_t x)
{
	writeInt64(x);
}

void Writer::writeInt32(int32_t x)
{
	writeVInt64((x << 1) ^ (x >> 31));
}

void Writer::writeInt64(int64_t x)
{
	writeVInt64((x << 1) ^ (x >> 63));
}

void Writer::writeVInt32(uint32_t x)
{
	writeVInt64(x);
}

void Writer::writeVInt64(uint64_t x)
{
	while (true)
	{
		if (!(x & ~0x7F))
		{
			writeUInt8(x & 0x7F);
			return;
		}
		else
			writeUInt8(x | 0x80);

		x >>= 7;
	}
}

void Writer::writeHeader(FieldHeader header, int16_t lastId)
{
	if (header.type == FieldType::Bool)
		m_lastBool = (m_destination.size() << 3) | 0x02;

	auto diff = header.id - lastId;
	if (diff <= 0 || diff > 0x0F)
	{
		writeUInt8(static_cast<uint8_t>(header.type));
		writeInt16(header.id);
	}
	else
		writeUInt8((diff << 4) | static_cast<uint8_t>(header.type));
}

void Writer::write(bool x, int16_t id, int16_t lastId)
{
	writeHeader({FieldType::Bool, id}, lastId);
	writeBool(x);
}

void Writer::write(int32_t x, int16_t id, int16_t lastId)
{
	writeHeader({FieldType::Int32, id}, lastId);
	writeInt32(x);
}

void Writer::write(int64_t x, int16_t id, int16_t lastId)
{
	writeHeader({FieldType::Int64, id}, lastId);
	writeInt64(x);
}

void Writer::write(const QByteArray &x, int16_t id, int16_t lastId)
{
	writeHeader({FieldType::String, id}, lastId);

	writeVInt32(x.size());
	m_destination.append(x);
}

void Writer::startStruct(int16_t id, int16_t lastId)
{
	writeHeader({FieldType::Struct, id}, lastId);
}

void Writer::endStruct()
{
	writeUInt8(static_cast<uint8_t>(FieldType::Stop));
}

void Writer::startList(FieldType type, int size, int16_t id, int16_t lastId)
{
	writeHeader({FieldType::List, id}, lastId);

	if (size < 15)
	{
		writeUInt8((size << 4) | static_cast<uint8_t>(type));
		return;
	}

	writeVInt32(size);
	writeUInt8(0xF0 | static_cast<uint8_t>(type));
}

void Writer::endList()
{
	writeUInt8(static_cast<uint8_t>(FieldType::Stop));
}

struct WriteVisitor : public boost::static_visitor<>
{
	WriteVisitor(Writer &writer, int16_t id, int16_t lastId) : writer{writer}, id{id}, lastId{lastId} {}

	Writer &writer;
	int16_t id;
	int16_t lastId;

	void operator() (const bool &v) const { writer.write(v, id, lastId); }
	void operator() (const int32_t &v) const { writer.write(v, id, lastId); }
	void operator() (const int64_t &v) const { writer.write(v, id, lastId); }
	void operator() (const QByteArray &v) const { writer.write(v, id, lastId); }

	template<typename T>
	void operator() (const List<T> &v) const
	{
		writer.startList(Type<T>::type, v.size(), id, lastId);
		// TODO: implement
		writer.endList();
	}
	void operator() (const Struct &v) const
	{
		writer.startStruct(id, lastId);
		writer.write(v);
		writer.endStruct();
	}
};

void Writer::write(const Struct &s)
{
	auto lastId = int16_t{0};
	for (auto &&field : s.fields)
	{
		boost::apply_visitor(WriteVisitor{*this, field.first, lastId}, field.second);
		lastId = field.first;
	}
}

}