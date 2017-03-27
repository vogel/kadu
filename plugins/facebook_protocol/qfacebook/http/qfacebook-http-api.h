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

#include "qfacebook/qfacebook-uid.h"

#include <QtNetwork/QNetworkAccessManager>

class QFacebookHttpReply;
class QFacebookHttpRequest;

class QFacebookHttpApi
{
public:
	explicit QFacebookHttpApi(QByteArray deviceId);

	QFacebookHttpReply * auth(const QString &user, const QString &password);
	QFacebookHttpReply * usersQuery(const QByteArray &token);
	QFacebookHttpReply * usersQueryAfter(const QByteArray &token, const QString &cursor);
	QFacebookHttpReply * usersQueryDelta(const QByteArray &token, const QString &deltaCursor);
	QFacebookHttpReply * threadListQuery(const QByteArray &token);
	QFacebookHttpReply * unreadThreadListQuery(const QByteArray &token, int unreadCount);
	QFacebookHttpReply * unreadMessagesListQuery(const QByteArray &token, QFacebookUid uid, int unreadCount);
	QFacebookHttpReply * get(const QFacebookHttpRequest &request);

private:
	QNetworkAccessManager m_nam;
	QByteArray m_deviceId;

};
