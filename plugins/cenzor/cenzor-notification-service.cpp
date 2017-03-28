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

#include "cenzor-notification-service.h"

#include "accounts/account.h"
#include "chat/chat.h"
#include "html/html-conversion.h"
#include "html/html-string.h"
#include "notification/notification-event-repository.h"
#include "notification/notification-event.h"
#include "notification/notification-service.h"
#include "notification/notification.h"

CenzorNotificationService::CenzorNotificationService(QObject *parent)
        : QObject{parent}, m_cenzoredEvent{QStringLiteral("cenzorNotification"),
                                           QStringLiteral(QT_TRANSLATE_NOOP("@default", "Message was cenzored"))}
{
}

CenzorNotificationService::~CenzorNotificationService()
{
}

void CenzorNotificationService::setNotificationEventRepository(NotificationEventRepository *notificationEventRepository)
{
    m_notificationEventRepository = notificationEventRepository;
}

void CenzorNotificationService::setNotificationService(NotificationService *notificationService)
{
    m_notificationService = notificationService;
}

void CenzorNotificationService::init()
{
    m_notificationEventRepository->addNotificationEvent(m_cenzoredEvent);
}

void CenzorNotificationService::done()
{
    m_notificationEventRepository->removeNotificationEvent(m_cenzoredEvent);
}

void CenzorNotificationService::notifyCenzored(const Chat &chat)
{
    auto data = QVariantMap{};
    data.insert(QStringLiteral("account"), qVariantFromValue(chat.chatAccount()));
    data.insert(QStringLiteral("chat"), qVariantFromValue(chat));

    auto notification = Notification{};
    notification.type = m_cenzoredEvent.name();
    notification.icon = KaduIcon{"kadu_icons/blocking"};
    notification.title = (tr("Cenzor"));
    notification.text = normalizeHtml(HtmlString{tr("Message was cenzored")});
    notification.details = normalizeHtml(HtmlString{tr("Your interlocutor used obscene word and became admonished")});
    notification.callbacks.append("chat-open");
    notification.callbacks.append("ignore");

    m_notificationService->notify(notification);
}

#include "moc_cenzor-notification-service.cpp"
