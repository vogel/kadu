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

#include "qmqtt-message.h"

#include "qmqtt-reader.h"

#include <QtCore/QByteArray>
#include <experimental/optional>
#include <tuple>

class QMqttMessageReceiver
{
public:
    void writeBytes(const QByteArray &bytes);
    std::experimental::optional<QMqttMessage> readMessage();

private:
    QByteArray m_buffer;
    std::experimental::optional<QMqttReader::QMqttHeader> m_header;
};
