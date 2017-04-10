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

#include "qfacebook-mqtt-api.h"
#include "qfacebook-mqtt-api.moc"

#include "qfacebook/messages/qfacebook-connect-ack.h"
#include "qfacebook/messages/qfacebook-connect.h"
#include "qfacebook/messages/qfacebook-disconnect.h"
#include "qfacebook/messages/qfacebook-message-type.h"
#include "qfacebook/messages/qfacebook-ping.h"
#include "qfacebook/messages/qfacebook-publish.h"
#include "qfacebook/messages/qfacebook-subscribe.h"
#include "qfacebook/messages/qfacebook-unsubscribe.h"
#include "qfacebook/mqtt/qfacebook-mqtt-connection.h"
#include "qfacebook/publish/qfacebook-publish-foreground-state.h"
#include "qfacebook/publish/qfacebook-publish-mark-thread.h"
#include "qfacebook/publish/qfacebook-publish-orca-message-notifications.h"
#include "qfacebook/publish/qfacebook-publish-presence.h"
#include "qfacebook/publish/qfacebook-publish-send-message-2.h"
#include "qfacebook/publish/qfacebook-publish-send-message-response.h"
#include "qfacebook/qfacebook-zlib.h"
#include "qmqtt/qmqtt-connection.h"
#include "qthrift/qthrift-reader.h"
#include "qthrift/qthrift.h"

#include "misc/misc.h"

#include <QtNetwork/QSslSocket>

constexpr auto const KEEP_ALIVE = uint16_t{60};

QFacebookMqttApi::QFacebookMqttApi(QFacebookSessionToken session, QFacebookDeviceId deviceId, QObject *parent)
        : QObject{parent}, m_session{std::move(session)}, m_deviceId{std::move(deviceId)},
          m_socket{make_owned<QSslSocket>(this)}
{
    m_pingTimer.setInterval(KEEP_ALIVE * 1000);
    m_connectionTimeoutTimer.setInterval(KEEP_ALIVE * 1500);

    connect(&m_pingTimer, &QTimer::timeout, this, &QFacebookMqttApi::sendPing);
    connect(&m_connectionTimeoutTimer, &QTimer::timeout, this, &QFacebookMqttApi::connectionTimeOut);
    connect(m_socket, &QAbstractSocket::connected, this, &QFacebookMqttApi::socketConnected);
    connect(m_socket, &QAbstractSocket::disconnected, this, &QFacebookMqttApi::socketDisconnected);
    m_socket->connectToHostEncrypted("mqtt.facebook.com", 443);
    m_connectionTimeoutTimer.start();
}

QFacebookMqttApi::~QFacebookMqttApi()
{
    sendDisconnect();
}

void QFacebookMqttApi::socketConnected()
{
    m_connectionTimeoutTimer.stop();

    m_mqttConnection = make_owned<QMqttConnection>(*m_socket, this);
    m_facebookMqttConnection = make_owned<QFacebookMqttConnection>(*m_mqttConnection, this);
    connect(
        m_facebookMqttConnection, &QFacebookMqttConnection::invalidOutgoingMessage, this,
        &QFacebookMqttApi::invalidOutgoingMessage);
    connect(
        m_facebookMqttConnection, &QFacebookMqttConnection::invalidIncomingMessage, this,
        &QFacebookMqttApi::invalidIncomingMessage);
    connect(
        m_facebookMqttConnection, &QFacebookMqttConnection::connectAckReceived, this,
        &QFacebookMqttApi::connectAckReceived);
    connect(m_facebookMqttConnection, &QFacebookMqttConnection::pongReceived, this, &QFacebookMqttApi::pongReceived);
    connect(
        m_facebookMqttConnection, &QFacebookMqttConnection::publishReceived, this, &QFacebookMqttApi::publishReceived);

    sendConnect();
}

void QFacebookMqttApi::socketDisconnected()
{
    m_pingTimer.stop();
    m_facebookMqttConnection.reset();
    m_mqttConnection.reset();

    emit disconnected();
    m_connectionTimeoutTimer.stop();
}

void QFacebookMqttApi::connectionTimeOut()
{
    m_socket->disconnectFromHost();
}

void QFacebookMqttApi::invalidOutgoingMessage()
{
}

void QFacebookMqttApi::invalidIncomingMessage(const QByteArray &data)
{
    m_socket->disconnectFromHost();
    emit invalidDataReceived(data);
}

void QFacebookMqttApi::sendConnect()
{
    auto connect = QFacebookConnect{};
    connect.cid = m_deviceId.clientId;
    connect.uid = m_session.uid();
    connect.information = "[FBAN/Orca-Android;FBAV/38.0.0.22.155;FBBV/14477681]";
    connect.visible = true;
    connect.did = m_deviceId.deviceId;
    connect.mid = m_deviceId.mqttId;
    connect.token = m_session.accessToken();
    connect.keepAlive = KEEP_ALIVE;

    m_facebookMqttConnection->send(connect);
    m_connectionTimeoutTimer.start();
}

void QFacebookMqttApi::sendDisconnect()
{
    if (m_facebookMqttConnection)
        m_facebookMqttConnection->send(QFacebookDisconnect{});
}

void QFacebookMqttApi::connectAckReceived(const QFacebookConnectAck &connectAck)
{
    m_connectionTimeoutTimer.stop();

    if (connectAck.errorCode != 0)
    {
        m_socket->disconnectFromHost();
        return;
    }

    m_pingTimer.start(KEEP_ALIVE * 1000);
    m_connectionTimeoutTimer.setInterval(KEEP_ALIVE * 500);

    auto foregroundState = QFacebookPublishForegroundState{true, 60};
    sendPublish("/foreground_state", foregroundState.encode());
    sendSubscribe(
        {"/mercury", "/messaging_events", "/orca_presence", "/orca_typing_notifications", "/orca_message_notifications",
         "/pp", "/t_ms", "/t_p", "/t_rtc", "/webrtc", "/webrtc_response"});

    emit connected();
}

void QFacebookMqttApi::sendPing()
{
    m_facebookMqttConnection->send(QFacebookPing{});
    m_connectionTimeoutTimer.start();
}

void QFacebookMqttApi::pongReceived(const QFacebookPong &)
{
    m_connectionTimeoutTimer.stop();
}

void QFacebookMqttApi::sendPublish(const QByteArray &topic, const QByteArray &content)
{
    auto publish = QFacebookPublish{};
    publish.topic = topic;
    publish.mid = ++m_messageId;
    publish.content = content;

    m_facebookMqttConnection->send(publish);
}

void QFacebookMqttApi::publishReceived(const QFacebookPublish &publish)
{
    try
    {
        if (publish.topic == "/t_p")
            emit presenceReceived(QFacebookPublishPresence::decode(publish.content));
        else if (publish.topic == "/send_message_response")
            emit sendMessageResponseReceived(QFacebookPublishSendMessageResponse::decode(publish.content));
        else if (publish.topic == "/orca_message_notifications")
            emit orcaMessageNotificationsReceived(QFacebookPublishOrcaMessageNotifications::decode(publish.content));
    }
    catch (...)
    {
        invalidIncomingMessage(publish.content);
    }
}

void QFacebookMqttApi::sendSubscribe(const std::vector<QByteArray> &topics)
{
    auto subscribe = QFacebookSubscribe{};
    subscribe.topics = topics;
    subscribe.mid = ++m_messageId;

    m_facebookMqttConnection->send(subscribe);
}

void QFacebookMqttApi::sendUnsubscribe(const std::vector<QByteArray> &topics)
{
    auto unsubscribe = QFacebookUnsubscribe{};
    unsubscribe.topics = topics;
    unsubscribe.mid = ++m_messageId;

    m_facebookMqttConnection->send(unsubscribe);
}

QFacebookMsgId QFacebookMqttApi::sendMessage(QFacebookUid to, const QByteArray &body)
{
    auto sendMessage2 = QFacebookPublishSendMessage2{};
    sendMessage2.body = body;

    auto m = static_cast<uint64_t>(QDateTime::currentMSecsSinceEpoch() / 1000);
    auto i = static_cast<uint32_t>(qrand());
    auto msgid = static_cast<QFacebookMsgId>((i & 0x3FFFFF) | (m << 22));

    sendMessage2.msgid = msgid;
    sendMessage2.uid = m_session.uid();
    sendMessage2.to = to;

    sendPublish("/send_message2", sendMessage2.encode());
    return msgid;
}

void QFacebookMqttApi::markThread(QFacebookUid theadId, const QByteArray &mark, bool state, int syncSequenceId)
{
    auto markThread = QFacebookPublishMarkThread{};
    markThread.otherUserFbId = theadId;
    markThread.mark = mark;
    markThread.state = state;
    markThread.syncSeqId = syncSequenceId;

    sendPublish("/mark_thread", markThread.encode());
}
