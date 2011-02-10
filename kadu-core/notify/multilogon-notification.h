/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "notify/notification-manager.h"
#include "notify/account-notification.h"

class MultilogonSession;
class NotifyEvent;

class MultilogonNotification : public AccountNotification
{
	Q_OBJECT

	static NotifyEvent *MultilogonSessionNotifyEvent;
	static NotifyEvent *MultilogonSessionConnectedNotifyEvent;
	static NotifyEvent *MultilogonSessionDisconnectedNotifyEvent;

	MultilogonSession *Session;

	MultilogonNotification(MultilogonSession *session, const QString &type, bool addKillCallback);
	virtual ~MultilogonNotification();

private slots:
	void killSession();

public:
	static void registerEvents();
	static void unregisterEvents();

	static void notifyMultilogonSessionConnected(MultilogonSession *session);
	static void notifyMultilogonSessionDisonnected(MultilogonSession *session);

};

#endif // MULTILOGON_NOTIFICATION_H
