/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QMap>
#include <QtCore/QString>

#include "notification/notification/notification.h"

class NotificationCallbackRepository;
class NotificationEventRepository;
class NotificationManager;

class ConnectionErrorNotification : public Notification
{
	Q_OBJECT

	QPointer<NotificationManager> m_notificationManager;

	QString ErrorServer;
	QString ErrorMessage;

private slots:
	void setNotificationManager(NotificationManager *notificationManager);

	void ignoreErrors();

public:
	static void registerEvent(Parser *parser, NotificationEventRepository *notificationEventRepository, NotificationCallbackRepository *notificationCallbackRepository);
	static void unregisterEvent(Parser *parser, NotificationEventRepository *notificationEventRepository);

	static void notifyConnectionError(NotificationManager *notificationManager, const Account &account, const QString &errorServer, const QString &errorMessage);

	explicit ConnectionErrorNotification(Account account, const QString &errorServer, const QString &errorMessage);
	virtual ~ConnectionErrorNotification();

	const QString & errorServer() const { return ErrorServer; }
	const QString & errorMessage() const { return ErrorMessage; }

	virtual bool isPeriodic() { return true; }
	virtual int period() { return 20; }
};
