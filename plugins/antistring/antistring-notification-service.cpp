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

#include "antistring-notification-service.h"

#include "notification/notification.h"
#include "notification/notification-event-repository.h"
#include "notification/notification-event.h"
#include "notification/notification-service.h"

AntistringNotificationService::AntistringNotificationService(QObject *parent) :
		QObject{parent},
		m_stringReceivedEvent{QStringLiteral("Antistring"), QStringLiteral(QT_TRANSLATE_NOOP("@default", "Antistring notifications"))}
{
}

AntistringNotificationService::~AntistringNotificationService()
{
}

void AntistringNotificationService::setNotificationEventRepository(NotificationEventRepository *notificationEventRepository)
{
	m_notificationEventRepository = notificationEventRepository;
}

void AntistringNotificationService::setNotificationService(NotificationService *notificationService)
{
	m_notificationService = notificationService;
}

void AntistringNotificationService::init()
{
	m_notificationEventRepository->addNotificationEvent(m_stringReceivedEvent);
}

void AntistringNotificationService::done()
{
	m_notificationEventRepository->removeNotificationEvent(m_stringReceivedEvent);
}

void AntistringNotificationService::notifyStringReceived(const Chat &chat)
{
	auto data = QVariantMap{};
	data.insert(QStringLiteral("account"), qVariantFromValue(chat.chatAccount()));
	data.insert(QStringLiteral("chat"), qVariantFromValue(chat));

	auto notification = Notification{QVariantMap{}, m_stringReceivedEvent.name(), KaduIcon{"kadu_icons/blocking"}};
	notification.addCallback("chat-open");
	notification.addCallback("ignore");
	notification.setTitle(tr("Antistring"));
	notification.setText(tr("Your interlocutor send you love letter"));

	m_notificationService->notify(notification);
}

#include "moc_antistring-notification-service.cpp"
