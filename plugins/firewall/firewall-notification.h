/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef FIREWALL_NOTIFICATION_H
#define FIREWALL_NOTIFICATION_H

#include "notification/notification/notification.h"

class Chat;

class FirewallNotification : public Notification
{
	Q_OBJECT

public:
	static void registerNotifications();
	static void unregisterNotifications();

	static void notify(const Chat &chat, const Contact &sender, const QString &message);

	explicit FirewallNotification(const Chat &chat);
	virtual ~FirewallNotification();

	virtual QString groupKey() const { return "firewall"; }

};

#endif // FIREWALL_NOTIFICATION_H
