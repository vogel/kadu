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

#include "configuration/configuration-api.h"
#include "configuration/deprecated-configuration-api.h"
#include "notification/notification.h"
#include "notification/notification-event-repository.h"
#include "notification/notification-event.h"
#include "notification/notification-service.h"

FirewallNotificationService::FirewallNotificationService(QObject *parent) :
		QObject{parent},
		m_blockedMessageEvent{QStringLiteral("firewallNotification"), QStringLiteral(QT_TRANSLATE_NOOP("@default", "Message was firewalled"))}
{
}

FirewallNotificationService::~FirewallNotificationService()
{
}

void FirewallNotificationService::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void FirewallNotificationService::setNotificationEventRepository(NotificationEventRepository *notificationEventRepository)
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

	auto notification = Notification{data, m_blockedMessageEvent.name(), KaduIcon{"kadu_icons/blocking"}};
	notification.addCallback("chat-open");
	notification.addCallback("ignore");
	notification.setTitle(tr("Message was blocked"));
	notification.setText(m_configuration->deprecatedApi()->readEntry("Firewall", "notification_syntax",
		tr("%u writes")).replace("%u", Qt::escape(sender.display(true))).remove("%m"));
	notification.setDetails(Qt::escape(message));

	m_notificationService->notify(notification);
}

#include "moc_firewall-notification-service.cpp"
