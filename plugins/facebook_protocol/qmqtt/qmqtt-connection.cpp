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

#include "qmqtt-connection.h"
#include "qmqtt-connection.moc"

#include "qmqtt/qmqtt-error.h"
#include "qmqtt/qmqtt-exception.h"
#include "qmqtt/qmqtt-message-sender.h"
#include "qmqtt/qmqtt-reader.h"

#include "misc/misc.h"

#include <QtCore/QIODevice>

QMqttConnection::QMqttConnection(QIODevice &ioDevice, QObject *parent) :
		QObject{parent},
		m_ioDevice{ioDevice}
{
	connect(&m_ioDevice, &QIODevice::readyRead, this, &QMqttConnection::readyRead);
}

QMqttConnection::~QMqttConnection()
{
}

void QMqttConnection::sendMessage(const QMqttMessage &message)
{
	auto data = QMqttMessageSender::writeMessage(message);
	m_ioDevice.write(data);
}

void QMqttConnection::readyRead()
{
	try
	{
		auto data = m_ioDevice.readAll();
		m_messageReceiver.writeBytes(data);
		while (auto message = m_messageReceiver.readMessage())
			emit messageReceived(*message);
	}
	catch (QMqttException &)
	{
		m_ioDevice.close();
		emit connectionClosed(QMqttError::MessageTooLarge);
	}
}
