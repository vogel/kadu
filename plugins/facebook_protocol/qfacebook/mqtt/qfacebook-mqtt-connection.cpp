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

#include "qfacebook-mqtt-connection.h"
#include "qfacebook-mqtt-connection.moc"

#include "qfacebook/messages/qfacebook-connect-ack.h"
#include "qfacebook/messages/qfacebook-connect.h"
#include "qfacebook/messages/qfacebook-message-type.h"
#include "qfacebook/messages/qfacebook-pong.h"
#include "qfacebook/messages/qfacebook-publish-ack.h"
#include "qfacebook/messages/qfacebook-publish-recorded.h"
#include "qfacebook/messages/qfacebook-publish.h"
#include "qmqtt/qmqtt-types.h"
#include "qthrift/qthrift-exception.h"

QFacebookMqttConnection::QFacebookMqttConnection(QMqttConnection &mqttConnection, QObject *parent) :
		QObject{parent},
		m_mqttConnection{mqttConnection}
{
	connect(&m_mqttConnection, &QMqttConnection::messageReceived, this, &QFacebookMqttConnection::messageReceived);
}

QFacebookMqttConnection::~QFacebookMqttConnection()
{
}

void QFacebookMqttConnection::messageReceived(const QMqttMessage &message)
{
	try
	{
		auto type = static_cast<QFacebookMessageType>(message.type);
		switch (type)
		{
			case QFacebookMessageType::ConnectAck:
				emit connectAckReceived(QFacebookConnectAck::decode(message));
				break;

			case QFacebookMessageType::Publish:
			{
				auto publish = QFacebookPublish::decode(message);
				sendPublishReceivedConfirmation(message.flags, publish);
				emit publishReceived(publish);
				break;
			}

			case QFacebookMessageType::PublishAck:
			case QFacebookMessageType::PubComp:
			case QFacebookMessageType::SubscribeAck:
			case QFacebookMessageType::UnsubscribeAck:
				break;

			case QFacebookMessageType::Pong:
				emit pongReceived(QFacebookPong::decode(message));
				break;

			default:
				break;
		};
	}
	catch (...)
	{
		emit invalidIncomingMessage(message.content);
	}
}

void QFacebookMqttConnection::sendPublishReceivedConfirmation(uint8_t flags, const QFacebookPublish &publish)
{
	if (publish.mid == 0)
		return;

	auto qos1 = (flags & static_cast<int8_t>(QMqttMessageFlag::QoS1)) == static_cast<int8_t>(QMqttMessageFlag::QoS1);
	auto qos2 = (flags & static_cast<int8_t>(QMqttMessageFlag::QoS2)) == static_cast<int8_t>(QMqttMessageFlag::QoS2);

	if (qos1)
		send(QFacebookPublishAck{publish.mid});
	if (qos2)
		send(QFacebookPublishRecorded{publish.mid});
}
