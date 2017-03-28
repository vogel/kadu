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

#include "qmqtt-message-receiver.h"

#include "qmqtt-buffer-size-exceeded-exception.h"
#include "qmqtt-message-size-exceeded-exception.h"
#include "qmqtt-reader.h"

#include "misc/misc.h"

#include <QtCore/QString>

namespace
{
constexpr int MAX_BUFFER_SIZE = 1 << 20;
constexpr int MAX_MESSAGE_SIZE = 1 << 20;
};

void QMqttMessageReceiver::writeBytes(const QByteArray &bytes)
{
    if (m_buffer.size() + bytes.size() > MAX_BUFFER_SIZE)
        throw QMqttBufferSizeExceededException{};
    m_buffer.append(bytes);
}

std::experimental::optional<QMqttMessage> QMqttMessageReceiver::readMessage()
{
    if (!m_header)
        m_header = QMqttReader::readHeader(m_buffer);

    if (!m_header)
        return {};

    if (m_header->size > MAX_MESSAGE_SIZE)
        throw QMqttMessageSizeExceededException{};

    if (m_buffer.size() < m_header->size + m_header->offset)
        return {};

    auto content = m_buffer.mid(m_header->offset, m_header->size);
    auto result = QMqttMessage{static_cast<uint8_t>(static_cast<uint8_t>(m_buffer[0]) >> 4),
                               static_cast<uint8_t>(static_cast<uint8_t>(m_buffer[0]) & 0x0F), content};
    m_buffer = m_buffer.mid(m_header->offset + m_header->size);
    m_header = std::experimental::nullopt;

    return result;
}
