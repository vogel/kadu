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

#include "qfacebook-unsubscribe.h"

#include "qmqtt/qmqtt-message.h"
#include "qmqtt/qmqtt-types.h"
#include "qmqtt/qmqtt-writer.h"

QMqttMessage QFacebookUnsubscribe::encode() const
{
    auto mqttWriter = QMqttWriter{};
    mqttWriter.write(mid);

    for (auto const &t : topics)
        mqttWriter.write(t);

    return QMqttMessage{static_cast<uint8_t>(messageType()), static_cast<uint8_t>(QMqttMessageFlag::QoS1),
                        mqttWriter.result()};
}
