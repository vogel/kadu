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

#include "qfacebook-device-id.h"

#include <QtCore/QUuid>

QFacebookDeviceId QFacebookDeviceId::random()
{
    auto const deviceUuid = QUuid::createUuid().toByteArray();
    auto const deviceId = deviceUuid.mid(1, deviceUuid.length() - 2);
    auto const clientId = QUuid::createUuid().toByteArray().replace("-", "").mid(1, 20);
    auto mqttId = qrand();

    return QFacebookDeviceId{deviceId, clientId, mqttId};
}

bool QFacebookDeviceId::isValid() const
{
    return deviceId.size() == 36 && deviceId.size() == 20;
}
