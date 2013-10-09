/*
 * %kadu copyright begin%
 * Copyright 2008, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef CONNECTION_ERROR_NOTIFICATION_H
#define CONNECTION_ERROR_NOTIFICATION_H

#include <QtCore/QMap>
#include <QtCore/QString>

#include "notify/notification/account-notification.h"

class NotifyEvent;

class ConnectionErrorNotification : public AccountNotification
{
	Q_OBJECT

	static NotifyEvent *ConnectionErrorNotifyEvent;
	QString ErrorServer;
	QString ErrorMessage;

private slots:
	void ignoreErrors();

public:
	static void registerEvent();
	static void unregisterEvent();

	static void notifyConnectionError(const Account &account, const QString &errorServer, const QString &errorMessage);

	ConnectionErrorNotification(Account account, const QString &errorServer, const QString &errorMessage);
	virtual ~ConnectionErrorNotification();

	const QString & errorServer() const { return ErrorServer; }
	const QString & errorMessage() const { return ErrorMessage; }

	virtual bool isPeriodic() { return true; }
	virtual int period() { return 20; }
};

#endif // CONNECTION_ERROR_NOTIFICATION_H
