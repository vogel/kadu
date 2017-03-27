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

#include "qmqtt-writer.h"

#include <QtEndian>

QMqttWriter::QMqttWriter()
{
}

void QMqttWriter::write(uint8_t x)
{
	m_result.append(reinterpret_cast<char *>(&x), 1);
}

void QMqttWriter::write(uint16_t x)
{
	x = qToBigEndian(x);
	m_result.append(reinterpret_cast<char *>(&x), 2);
}

void QMqttWriter::write(const QByteArray &x)
{
	write(static_cast<uint16_t>(x.size()));
	writeRaw(x);
}

void QMqttWriter::writeRaw(const QByteArray &x)
{
	m_result.append(x);
}

QByteArray QMqttWriter::result() const
{
	return m_result;
}
