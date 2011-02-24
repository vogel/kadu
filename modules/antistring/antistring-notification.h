/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ANTISTRING_NOTIFICATION_H
#define ANTISTRING_NOTIFICATION_H

#include "notify/chat-notification.h"

class NotifyEvent;

class AntistringNotification : public ChatNotification
{
	Q_OBJECT

	static NotifyEvent *StringReceivedNotification;

public:
	static void registerNotifications();
	static void unregisterNotifications();

	static void notifyStringReceived(const Chat &chat);

	explicit AntistringNotification(const Chat &chat);
	virtual ~AntistringNotification();
};

#endif // ANTISTRING_NOTIFICATION_H
