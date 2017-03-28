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

#pragma once

#include "qmqtt/qmqtt-connection.h"

#include "misc/misc.h"

#include <QtCore/QByteArray>
#include <QtCore/QObject>

class QFacebookConnectAck;
class QFacebookPong;
class QFacebookPublish;
struct QMqttMessage;

class QFacebookMqttConnection : public QObject
{
    Q_OBJECT

public:
    explicit QFacebookMqttConnection(QMqttConnection &mqttConnection, QObject *parent = nullptr);
    virtual ~QFacebookMqttConnection();

    template <typename MessageType>
    void send(const MessageType &message)
    {
        try
        {
            auto mqttMessage = message.encode();
            m_mqttConnection.sendMessage(mqttMessage);
        }
        catch (...)
        {
            emit invalidOutgoingMessage();
        }
    }

signals:
    void invalidOutgoingMessage();
    void invalidIncomingMessage(const QByteArray &data);

    void connectAckReceived(const QFacebookConnectAck &connectAck);
    void pongReceived(const QFacebookPong &pong);
    void publishReceived(const QFacebookPublish &publish);

private:
    QMqttConnection &m_mqttConnection;

    void messageReceived(const QMqttMessage &message);
    void sendPublishReceivedConfirmation(uint8_t flags, const QFacebookPublish &publish);
};
