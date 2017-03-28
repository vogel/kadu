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

#include "qfacebook/qfacebook-device-id.h"
#include "qfacebook/qfacebook-msgid.h"
#include "qfacebook/qfacebook-uid.h"
#include "qfacebook/session/qfacebook-session-token.h"

#include "misc/memory.h"

#include <QtCore/QObject>
#include <QtCore/QTimer>

class QFacebookConnectAck;
class QFacebookPong;
class QFacebookPublish;
class QFacebookMqttConnection;
class QMqttConnection;
struct QFacebookPublishOrcaMessageNotifications;
struct QFacebookPublishPresence;
struct QFacebookPublishSendMessageResponse;
struct QMqttMessage;

class QSslSocket;

class QFacebookMqttApi : public QObject
{
    Q_OBJECT

public:
    explicit QFacebookMqttApi(QFacebookSessionToken session, QFacebookDeviceId deviceId, QObject *parent = nullptr);
    virtual ~QFacebookMqttApi();

    QFacebookMsgId sendMessage(QFacebookUid to, const QByteArray &body);
    void markThread(QFacebookUid threadId, const QByteArray &mark, bool state, int syncSequenceId);

signals:
    void connected();
    void disconnected();

    void invalidDataReceived(const QByteArray &data);
    void presenceReceived(const QFacebookPublishPresence &presence);
    void sendMessageResponseReceived(const QFacebookPublishSendMessageResponse &sendMessageResponse);
    void orcaMessageNotificationsReceived(const QFacebookPublishOrcaMessageNotifications &orcaMessageNotifications);

private:
    QFacebookSessionToken m_session;
    QFacebookDeviceId m_deviceId;
    QTimer m_pingTimer;
    QTimer m_connectionTimeoutTimer;
    owned_qptr<QSslSocket> m_socket;
    owned_qptr<QMqttConnection> m_mqttConnection;
    owned_qptr<QFacebookMqttConnection> m_facebookMqttConnection;
    uint16_t m_messageId{0};

    void socketConnected();
    void socketDisconnected();
    void connectionTimeOut();

    void sendConnect();
    void sendDisconnect();
    void sendPing();
    void sendPublish(const QByteArray &topic, const QByteArray &content);
    void sendSubscribe(const std::vector<QByteArray> &topics);
    void sendUnsubscribe(const std::vector<QByteArray> &topics);

    void invalidOutgoingMessage();
    void invalidIncomingMessage(const QByteArray &data);

    void connectAckReceived(const QFacebookConnectAck &connectAck);
    void pongReceived(const QFacebookPong &pong);
    void publishReceived(const QFacebookPublish &publish);
};
