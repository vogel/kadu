/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include "notify/chat-notification.h"

class NotifyEvent;

class FirewallNotification : public ChatNotification
{
	Q_OBJECT

	static NotifyEvent *FirewalledNotification;

public:
	static void registerNotifications();
	static void unregisterNotifications();

	static void notify(Chat chat, Contact sender, const QString &message);

	explicit FirewallNotification(Chat chat);
	virtual ~FirewallNotification();

};

#endif // FIREWALL_NOTIFICATION_H
