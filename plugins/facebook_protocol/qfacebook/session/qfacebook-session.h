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

#include "qfacebook/qfacebook-msgid.h"
#include "qfacebook/qfacebook-uid.h"
#include "qfacebook/session/qfacebook-session-token.h"

#include <QtCore/QObject>
#include <memory>

class QFacebookHttpApi;
class QFacebookMqttApi;
struct QFacebookDownloadContactsDeltaResult;
struct QFacebookDownloadContactsResult;
struct QFacebookDownloadThreadsResult;
struct QFacebookDownloadUnreadMessagesResult;
struct QFacebookDownloadUnreadThreadsResult;
struct QFacebookPublishPresence;
struct QFacebookPublishSendMessageResponse;
struct QFacebookPublishOrcaMessageNotifications;

class QByteArray;

class QFacebookSession : public QObject
{
	Q_OBJECT

public:
	explicit QFacebookSession(QFacebookSessionToken sessionToken, std::unique_ptr<QFacebookHttpApi> &&httpApi, std::unique_ptr<QFacebookMqttApi> &&mqttApi);
	~QFacebookSession();

	void downloadContacts();
	void downloadContactsDelta(QByteArray deltaCursor);

	void downloadThreads();
	void downloadUnreadThreads(int unreadCount);
	void downloadUnreadMessages(QFacebookUid uid, int unreadCount);
	void markRead(QFacebookUid uid, int syncSequenceId);
	QFacebookMsgId sendMessage(QFacebookUid to, const QByteArray &body);

signals:
	void disconnected();
	void invalidDataReceived(const QByteArray &data);

	void contactsReceived(const QFacebookDownloadContactsResult &downloadContactsResult);
	void contactsDeltaReceived(const QFacebookDownloadContactsDeltaResult &downloadContactsDeltaResult);

	void presenceReceived(const QFacebookPublishPresence &presence);

	void threadsReceived(const QFacebookDownloadThreadsResult &result);
	void unreadThreadsReceived(const QFacebookDownloadUnreadThreadsResult &result);
	void unreadMessagesReceived(const QFacebookDownloadUnreadMessagesResult &result);
	void sendMessageResponseReceived(const QFacebookPublishSendMessageResponse &sendMessageResponse);
	void messageNotificationsReceived(const QFacebookPublishOrcaMessageNotifications &orcaMessageNotifications);

private:
	const QFacebookSessionToken m_sessionToken;
	const std::unique_ptr<QFacebookHttpApi> m_httpApi;
	const std::unique_ptr<QFacebookMqttApi> m_mqttApi;

};
