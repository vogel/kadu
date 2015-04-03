/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "core/core.h"
#include "notification/notification-event.h"
#include "notification/notification-event-repository.h"
#include "notification/notification-manager.h"

#include "screenshot-notification.h"

void ScreenshotNotification::registerNotifications()
{
	Core::instance()->notificationEventRepository()->addNotificationEvent(NotificationEvent{"ssSizeLimit", QT_TRANSLATE_NOOP("@default", "ScreenShot images size limit")});
}

void ScreenshotNotification::unregisterNotifiactions()
{
	Core::instance()->notificationEventRepository()->removeNotificationEvent(NotificationEvent{"ssSizeLimit", QT_TRANSLATE_NOOP("@default", "ScreenShot images size limit")});
}

void ScreenshotNotification::notifySizeLimit(long size)
{
	ScreenshotNotification *notification = new ScreenshotNotification();
	notification->setTitle(tr("ScreenShot size limit"));
	notification->setText(tr("Images size limit exceed: %1 KB").arg(size/1024));
	Core::instance()->notificationManager()->notify(notification);
}

ScreenshotNotification::ScreenshotNotification() :
		Notification(Account::null, Chat::null, "ssSizeLimit", KaduIcon("kadu_icons/blocking"))
{
}

ScreenshotNotification::~ScreenshotNotification()
{

}

#include "moc_screenshot-notification.cpp"
