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

#include "qfacebook-session.h"
#include "qfacebook-session.moc"

#include "qfacebook/http/qfacebook-download-contacts-delta-job.h"
#include "qfacebook/http/qfacebook-download-contacts-job.h"
#include "qfacebook/http/qfacebook-download-threads-job.h"
#include "qfacebook/http/qfacebook-download-unread-messages-job.h"
#include "qfacebook/http/qfacebook-download-unread-threads-job.h"
#include "qfacebook/http/qfacebook-http-api.h"
#include "qfacebook/mqtt/qfacebook-mqtt-api.h"

QFacebookSession::QFacebookSession(
    QFacebookSessionToken sessionToken, std::unique_ptr<QFacebookHttpApi> &&httpApi,
    std::unique_ptr<QFacebookMqttApi> &&mqttApi)
        : m_sessionToken{std::move(sessionToken)}, m_httpApi{std::move(httpApi)}, m_mqttApi{std::move(mqttApi)}
{
    connect(m_mqttApi.get(), &QFacebookMqttApi::disconnected, this, &QFacebookSession::disconnected);
    connect(m_mqttApi.get(), &QFacebookMqttApi::invalidDataReceived, this, &QFacebookSession::invalidDataReceived);

    connect(
        m_mqttApi.get(), &QFacebookMqttApi::sendMessageResponseReceived, this,
        &QFacebookSession::sendMessageResponseReceived);
    connect(
        m_mqttApi.get(), &QFacebookMqttApi::inboxReceived, this,
        &QFacebookSession::inboxReceived);
    connect(m_mqttApi.get(), &QFacebookMqttApi::presenceReceived, this, &QFacebookSession::presenceReceived);
}

QFacebookSession::~QFacebookSession() = default;

void QFacebookSession::downloadContacts()
{
    auto job = make_owned<QFacebookDownloadContactsJob>(*m_httpApi, m_sessionToken, this);
    connect(job, &QFacebookDownloadContactsJob::finished, this, &QFacebookSession::contactsReceived);
}

void QFacebookSession::downloadContactsDelta(QByteArray deltaCursor)
{
    auto job = make_owned<QFacebookDownloadContactsDeltaJob>(*m_httpApi, m_sessionToken, std::move(deltaCursor), this);
    connect(job, &QFacebookDownloadContactsDeltaJob::finished, this, &QFacebookSession::contactsDeltaReceived);
}

void QFacebookSession::downloadThreads()
{
    auto job = make_owned<QFacebookDownloadThreadsJob>(*m_httpApi, m_sessionToken, this);
    connect(job, &QFacebookDownloadThreadsJob::finished, this, &QFacebookSession::threadsReceived);
}

void QFacebookSession::downloadUnreadThreads(int unreadCount)
{
    auto job = make_owned<QFacebookDownloadUnreadThreadsJob>(*m_httpApi, m_sessionToken, unreadCount, this);
    connect(job, &QFacebookDownloadUnreadThreadsJob::finished, this, &QFacebookSession::unreadThreadsReceived);
}

void QFacebookSession::downloadUnreadMessages(QFacebookUid uid, int unreadCount)
{
    auto job = make_owned<QFacebookDownloadUnreadMessagesJob>(*m_httpApi, m_sessionToken, uid, unreadCount, this);
    connect(job, &QFacebookDownloadUnreadMessagesJob::finished, this, &QFacebookSession::unreadMessagesReceived);
}

void QFacebookSession::markRead(QFacebookUid uid, int syncSequenceId)
{
    m_mqttApi->markThread(uid, "read", true, syncSequenceId);
}

QFacebookMsgId QFacebookSession::sendMessage(QFacebookUid to, const QByteArray &body)
{
    return m_mqttApi->sendMessage(to, body);
}
