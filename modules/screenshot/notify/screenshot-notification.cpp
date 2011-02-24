/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "icons-manager.h"

#include "screenshot-notification.h"

NotifyEvent * ScreenshotNotification::SizeLimitNotification = 0;

void ScreenshotNotification::registerNotifications()
{
	if (SizeLimitNotification)
		return;

	SizeLimitNotification = new NotifyEvent("ssSizeLimit", NotifyEvent::CallbackNotRequired, "ScreenShot images size limit");
	NotificationManager::instance()->registerNotifyEvent(SizeLimitNotification);
}

void ScreenshotNotification::unregisterNotifiactions()
{
	if (!SizeLimitNotification)
		return;

	NotificationManager::instance()->unregisterNotifyEvent(SizeLimitNotification);
	delete SizeLimitNotification;
	SizeLimitNotification = 0;
}

void ScreenshotNotification::notifySizeLimit(int size)
{
	ScreenshotNotification *notification = new ScreenshotNotification();
	notification->setTitle(tr("ScreenShot size limit"));
	notification->setText(tr("Images size limit exceed: %1 KB").arg(size/1024));
	NotificationManager::instance()->notify(notification);
}

ScreenshotNotification::ScreenshotNotification() :
		Notification("ssSizeLimit", "kadu_icons/blocking")
{
}

ScreenshotNotification::~ScreenshotNotification()
{

}
