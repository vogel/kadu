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

#include "qfacebook-connect.h"

#include "qfacebook/qfacebook-zlib.h"
#include "qmqtt/qmqtt-message.h"
#include "qmqtt/qmqtt-types.h"
#include "qmqtt/qmqtt-writer.h"
#include "qthrift/qthrift-writer.h"
#include "qthrift/qthrift.h"

QMqttMessage QFacebookConnect::encode() const
{
	auto connectStruct = QThrift::Struct{
		{1, {cid}},
		{4, QThrift::Struct{
			{1, uid},
			{2, information},
			{3, unknownCp},
			{4, unknownEcp},
			{5, unknown},
			{6, noAutoFg},
			{7, visible},
			{8, did},
			{9, unknownFg},
			{10, unknownNwt},
			{11, unknownNwst},
			{12, mid},
			{14, QThrift::List<int32_t>{}},
			{15, token}
		}}
	};

	uint8_t flags =
			static_cast<uint8_t>(QMqttConnectFlag::User) |
			static_cast<uint8_t>(QMqttConnectFlag::Pass) |
			static_cast<uint8_t>(QMqttConnectFlag::Clr) |
			static_cast<uint8_t>(QMqttConnectFlag::QoS1);

	auto connectData = QByteArray{};
	auto writer = QThrift::Writer{connectData};
	writer.write(connectStruct);

	auto mqttWriter = QMqttWriter{};
	mqttWriter.write("MQTToT");
	mqttWriter.write(uint8_t{3});
	mqttWriter.write(uint8_t{flags});
	mqttWriter.write(keepAlive);
	mqttWriter.writeRaw(qfacebookDeflate(connectData));

	return QMqttMessage{
		static_cast<uint8_t>(messageType()),
		uint8_t{0},
		mqttWriter.result()
	};
}
