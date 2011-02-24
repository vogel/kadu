/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include "notify/notification-manager.h"
#include "chat-notification.h"

class ContactSet;
class NotifyEvent;

class StatusChangedNotification : public ChatNotification
{
	Q_OBJECT

	static NotifyEvent *StatusChangedNotifyEvent;
	static NotifyEvent *StatusChangedToFreeForChatNotifyEvent;
	static NotifyEvent *StatusChangedToOnlineNotifyEvent;
	static NotifyEvent *StatusChangedToAwayNotifyEvent;
	static NotifyEvent *StatusChangedToNotAvailableNotifyEvent;
	static NotifyEvent *StatusChangedToDoNotDisturbNotifyEvent;
	static NotifyEvent *StatusChangedToOfflineNotifyEvent;

public:
	static void registerEvents();
	static void unregisterEvents();

	StatusChangedNotification(const QString &toStatus, const Contact &contact);
	virtual ~StatusChangedNotification() {}

};

#endif // STATUS_CHANGED_NOTIFICATION_H
