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

#include "qmqtt-message-sender.h"

QByteArray QMqttMessageSender::writeMessage(const QMqttMessage &message)
{
	auto size = static_cast<uint32_t>(message.content.size());
	auto result = QByteArray{};
	auto byte = (static_cast<uint8_t>(message.type) << 4) | (message.flags & 0x0F);
	result.append(reinterpret_cast<char *>(&byte), 1);

	do
	{
		auto byte = static_cast<uint8_t>(size & 0x7F);
		size >>= 7;
		if (size > 0)
			byte |= 0x80;

		result.append(reinterpret_cast<char *>(&byte), 1);
	} while (size > 0);

	result.append(message.content);
	return result;
}
