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

#include "firewall-notification-service.h"

#include "accounts/account.h"
#include "chat/chat.h"
#include "configuration/configuration-api.h"
#include "configuration/deprecated-configuration-api.h"
#include "html/html-conversion.h"
#include "html/html-string.h"
#include "notification/notification-event-repository.h"
#include "notification/notification-event.h"
#include "notification/notification-service.h"
#include "notification/notification.h"

FirewallNotificationService::FirewallNotificationService(QObject *parent)
        : QObject{parent},
          m_blockedMessageEvent{QStringLiteral("firewallNotification"),
                                QStringLiteral(QT_TRANSLATE_NOOP("@default", "Message was firewalled"))}
{
}

FirewallNotificationService::~FirewallNotificationService()
{
}

void FirewallNotificationService::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void FirewallNotificationService::setNotificationEventRepository(
    NotificationEventRepository *notificationEventRepository)
{
    m_notificationEventRepository = notificationEventRepository;
}

void FirewallNotificationService::setNotificationService(NotificationService *notificationService)
{
    m_notificationService = notificationService;
}

void FirewallNotificationService::init()
{
    m_notificationEventRepository->addNotificationEvent(m_blockedMessageEvent);
}

void FirewallNotificationService::done()
{
    m_notificationEventRepository->removeNotificationEvent(m_blockedMessageEvent);
}

void FirewallNotificationService::notifyBlockedMessage(const Chat &chat, const Contact &sender, const QString &message)
{
    auto data = QVariantMap{};
    data.insert(QStringLiteral("account"), qVariantFromValue(chat.chatAccount()));
    data.insert(QStringLiteral("chat"), qVariantFromValue(chat));
    data.insert(QStringLiteral("contact"), qVariantFromValue(sender));

    auto notification = Notification{};
    notification.type = m_blockedMessageEvent.name();
    notification.icon = KaduIcon{"kadu_icons/blocking"};
    notification.title = tr("Message was blocked");
    notification.text = normalizeHtml(
        plainToHtml(
            m_configuration->deprecatedApi()
                ->readEntry("Firewall", "notification_syntax", tr("%u writes"))
                .replace("%u", sender.display(true))
                .remove("%m")));
    notification.details = normalizeHtml(plainToHtml(message));
    notification.callbacks.append("chat-open");
    notification.callbacks.append("ignore");
    notification.data = std::move(data);

    m_notificationService->notify(notification);
}

#include "moc_firewall-notification-service.cpp"
