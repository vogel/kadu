/*
 * %kadu copyright begin%
 * Copyright 2017Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "facebook-account-data.h"

#include "qfacebook/qfacebook-device-id.h"

#include "accounts/account-shared.h"

FacebookAccountData::FacebookAccountData(AccountShared *data) :
		m_data{data}
{
}

FacebookAccountData::~FacebookAccountData()
{
}

QByteArray FacebookAccountData::deltaCursor() const
{
	return m_data->isValidStorage()
		? m_data->loadValue<QByteArray>("DeltaCursor")
		: QByteArray{};
}

void FacebookAccountData::setDeltaCursor(const QByteArray &deltaCursor) const
{
	m_data->ensureLoaded();
	if (m_data->isValidStorage())
		m_data->storeValue("DeltaCursor", deltaCursor);
}

QFacebookDeviceId FacebookAccountData::deviceId() const
{
	if (!m_data->isValidStorage())
		return QFacebookDeviceId::random();

	auto result = QFacebookDeviceId{
		m_data->loadValue<QByteArray>("DeviceId"),
		m_data->loadValue<QByteArray>("ClientId"),
		m_data->loadValue<int64_t>("MQTTId")
	};

	if (!result.isValid())
	{
		result = QFacebookDeviceId::random();
		m_data->storeValue("DeviceId", result.deviceId);
		m_data->storeValue("ClientId", result.clientId);
		m_data->storeValue("MQTTId", static_cast<qlonglong>(result.mqttId));
	}

	return result;
}
