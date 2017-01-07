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
#include <experimental/optional>
#include <tuple>

enum class QMqttMessageType : uint8_t;

class QMqttReader
{

public:
	struct QMqttHeader
	{
		uint32_t size;
		uint32_t offset;
	};

	static std::experimental::optional<QMqttHeader> readHeader(const QByteArray &data);

	explicit QMqttReader(const QByteArray &content);

	uint8_t readUint8();
	uint16_t readUint16();
	QByteArray readData();
	QByteArray readRaw();

private:
	const QByteArray &m_content;
	int m_pos {0};

};
