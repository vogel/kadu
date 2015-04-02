/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef STATUS_CHANGED_NOTIFICATION_H
#define STATUS_CHANGED_NOTIFICATION_H

#include "notification/notification-manager.h"
#include "notification/notification/notification.h"

class ContactSet;
class NotificationEvent;

class StatusChangedNotification : public Notification
{
	Q_OBJECT

	Contact CurrentContact;

public:
	static void registerEvents();
	static void unregisterEvents();

	virtual QString groupKey() const { return CurrentContact.id(); }

	StatusChangedNotification(const QString &toStatus, const Contact &contact, const QString &statusDisplayName, const QString &description);
	virtual ~StatusChangedNotification() {}
};

#endif // STATUS_CHANGED_NOTIFICATION_H
