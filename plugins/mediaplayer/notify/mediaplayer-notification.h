/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef MEDIAPLAYER_NOTIFICATION_H
#define MEDIAPLAYER_NOTIFICATION_H

#include "notify/notification/notification.h"

#include "mediaplayer_exports.h"

class NotifyEvent;

class MEDIAPLAYERAPI MediaPlayerNotification : public Notification
{
	Q_OBJECT

	static NotifyEvent *TitleHintNotification;

public:
	static void registerNotifications();
	static void unregisterNotifications();

	static void notifyTitleHint(const QString &title);

	MediaPlayerNotification();
	virtual ~MediaPlayerNotification();

};

#endif // MEDIAPLAYER_NOTIFICATION_H
