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

#include "mediaplayer-notification-service.h"
#include "mediaplayer-notification-service.moc"

#include "html/html-conversion.h"
#include "html/html-string.h"
#include "notification/notification-event-repository.h"
#include "notification/notification-event.h"
#include "notification/notification-service.h"
#include "notification/notification.h"

MediaplayerNotificationService::MediaplayerNotificationService(QObject *parent)
        : QObject{parent},
          m_playingTitleEvent{QStringLiteral("MediaPlayerOsd"),
                              QStringLiteral(QT_TRANSLATE_NOOP("@default", "Pseudo-OSD for MediaPlayer"))}
{
}

MediaplayerNotificationService::~MediaplayerNotificationService()
{
}

void MediaplayerNotificationService::setNotificationEventRepository(
    NotificationEventRepository *notificationEventRepository)
{
    m_notificationEventRepository = notificationEventRepository;
}

void MediaplayerNotificationService::setNotificationService(NotificationService *notificationService)
{
    m_notificationService = notificationService;
}

void MediaplayerNotificationService::init()
{
    m_notificationEventRepository->addNotificationEvent(m_playingTitleEvent);
}

void MediaplayerNotificationService::done()
{
    m_notificationEventRepository->removeNotificationEvent(m_playingTitleEvent);
}

void MediaplayerNotificationService::notifyPlayingTitle(const QString &title)
{
    auto notification = Notification{};
    notification.type = m_playingTitleEvent.name();
    notification.icon = KaduIcon{"external_modules/mediaplayer-media-playback-play"};
    notification.text = normalizeHtml(plainToHtml(title));

    m_notificationService->notify(notification);
}
