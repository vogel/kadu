/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STATUS_CHANGED_NOTIFICATION_H
#define STATUS_CHANGED_NOTIFICATION_H

#include "notify/notification-manager.h"
#include "chat-notification.h"

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

	StatusChangedNotification(const QString &toStatus, BuddySet &contacts, Account account);
	virtual ~StatusChangedNotification() {}

};

#endif // STATUS_CHANGED_NOTIFICATION_H
