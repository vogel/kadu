/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef MULTILOGON_NOTIFICATION_H
#define MULTILOGON_NOTIFICATION_H

#include "notification/notification-manager.h"
#include "notification/notification/notification.h"

class MultilogonSession;

class MultilogonNotification : public Notification
{
	Q_OBJECT

	MultilogonSession *Session;

private slots:
	void killSession();

public:
	MultilogonNotification(MultilogonSession *session, const QString &type, bool addKillCallback);
	virtual ~MultilogonNotification();

	static void registerEvents();
	static void unregisterEvents();
};

#endif // MULTILOGON_NOTIFICATION_H
