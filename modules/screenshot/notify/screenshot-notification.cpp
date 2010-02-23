/*
 * %kadu copyright begin%
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

#include "notify/notification-manager.h"
#include "notify/notify-event.h"

#include "screenshot-notification.h"

NotifyEvent * ScreenshotNotification::SizeLimitNotification = 0;

void ScreenshotNotification::registerNotifications()
{
	SizeLimitNotification = new NotifyEvent("ssSizeLimit", NotifyEvent::CallbackNotRequired, "ScreenShot images size limit");
	NotificationManager::instance()->registerNotifyEvent(SizeLimitNotification);
}

void ScreenshotNotification::unregisterNotifiactions()
{
	NotificationManager::instance()->unregisterNotifyEvent(SizeLimitNotification);
	delete SizeLimitNotification;
	SizeLimitNotification = 0;
}

void ScreenshotNotification::notifySizeLimit()
{

}

ScreenshotNotification::ScreenshotNotification(const QString &type, const QString &text) :
		Notification(type, QIcon())
{
	setText(text);
}

ScreenshotNotification::~ScreenshotNotification()
{

}
