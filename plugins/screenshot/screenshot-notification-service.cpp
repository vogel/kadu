/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "screenshot-notification-service.h"

#include "notification/notification.h"
#include "notification/notification-event-repository.h"
#include "notification/notification-event.h"
#include "notification/notification-service.h"

ScreenshotNotificationService::ScreenshotNotificationService(QObject *parent) :
		QObject{parent},
		m_sizeLimitEvent{QStringLiteral("ssSizeLimit"), QStringLiteral(QT_TRANSLATE_NOOP("@default", "ScreenShot images size limit"))}
{
}

ScreenshotNotificationService::~ScreenshotNotificationService()
{
}

void ScreenshotNotificationService::setNotificationEventRepository(NotificationEventRepository *notificationEventRepository)
{
	m_notificationEventRepository = notificationEventRepository;
}

void ScreenshotNotificationService::setNotificationService(NotificationService *notificationService)
{
	m_notificationService = notificationService;
}

void ScreenshotNotificationService::init()
{
	m_notificationEventRepository->addNotificationEvent(m_sizeLimitEvent);
}

void ScreenshotNotificationService::done()
{
	m_notificationEventRepository->removeNotificationEvent(m_sizeLimitEvent);
}

void ScreenshotNotificationService::notifySizeLimit(long size)
{
	auto notification = Notification{};
	notification.type = m_sizeLimitEvent.name();
	notification.icon = KaduIcon{"kadu_icons/blocking"};
	notification.title = tr("ScreenShot size limit");
	notification.text = tr("Images size limit exceed: %1 KB").arg(size/1024);

	m_notificationService->notify(notification);
}

#include "moc_screenshot-notification-service.cpp"
