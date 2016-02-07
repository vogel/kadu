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

#include "status-notification-service.h"

#include "chat/chat-manager.h"
#include "chat/chat-storage.h"
#include "chat/chat.h"
#include "chat/type/chat-type-contact.h"
#include "identities/identity.h"
#include "misc/memory.h"
#include "notification/notification.h"
#include "notification/notification-event-repository.h"
#include "notification/notification-event.h"
#include "notification/notification-service.h"
#include "protocols/protocol.h"
#include "status/status-type-data.h"
#include "status/status-type-manager.h"

StatusNotificationService::StatusNotificationService(QObject *parent) :
		QObject{parent},
		m_statusChangedEvent{QStringLiteral("StatusChanged"), QT_TRANSLATE_NOOP("@default", QStringLiteral("User changed status"))},
		m_statusChangedToFreeForChatEvent{QStringLiteral("StatusChanged/ToFreeForChat"), QT_TRANSLATE_NOOP("@default", QStringLiteral("to free for chat"))},
		m_statusChangedToOnlineEvent{QStringLiteral("StatusChanged/ToOnline"), QT_TRANSLATE_NOOP("@default", QStringLiteral("to online"))},
		m_statusChangedToAwayEvent{QStringLiteral("StatusChanged/ToAway"), QT_TRANSLATE_NOOP("@default", QStringLiteral("to away"))},
		m_statusChangedToNotAvailableEvent{QStringLiteral("StatusChanged/ToNotAvailable"), QT_TRANSLATE_NOOP("@default", QStringLiteral("to not available"))},
		m_statusChangedToDoNotDisturbEvent{QStringLiteral("StatusChanged/ToDoNotDisturb"), QT_TRANSLATE_NOOP("@default", QStringLiteral("to do not disturb"))},
		m_statusChangedToOfflineEvent{QStringLiteral("StatusChanged/ToOffline"), QT_TRANSLATE_NOOP("@default", QStringLiteral("to offline"))}
{
}

StatusNotificationService::~StatusNotificationService()
{
}

void StatusNotificationService::setChatManager(ChatManager *chatManager)
{
	m_chatManager = chatManager;
}

void StatusNotificationService::setChatStorage(ChatStorage *chatStorage)
{
	m_chatStorage = chatStorage;
}

void StatusNotificationService::setNotificationEventRepository(NotificationEventRepository *notificationEventRepository)
{
	m_notificationEventRepository = notificationEventRepository;
}

void StatusNotificationService::setNotificationService(NotificationService *notificationService)
{
	m_notificationService = notificationService;
}

void StatusNotificationService::setStatusTypeManager(StatusTypeManager *statusTypeManager)
{
	m_statusTypeManager = statusTypeManager;
}

void StatusNotificationService::init()
{
	m_notificationEventRepository->addNotificationEvent(m_statusChangedEvent);
	m_notificationEventRepository->addNotificationEvent(m_statusChangedToFreeForChatEvent);
	m_notificationEventRepository->addNotificationEvent(m_statusChangedToOnlineEvent);
	m_notificationEventRepository->addNotificationEvent(m_statusChangedToAwayEvent);
	m_notificationEventRepository->addNotificationEvent(m_statusChangedToNotAvailableEvent);
	m_notificationEventRepository->addNotificationEvent(m_statusChangedToDoNotDisturbEvent);
	m_notificationEventRepository->addNotificationEvent(m_statusChangedToOfflineEvent);
}

void StatusNotificationService::done()
{
	m_notificationEventRepository->removeNotificationEvent(m_statusChangedEvent);
	m_notificationEventRepository->removeNotificationEvent(m_statusChangedToFreeForChatEvent);
	m_notificationEventRepository->removeNotificationEvent(m_statusChangedToOnlineEvent);
	m_notificationEventRepository->removeNotificationEvent(m_statusChangedToAwayEvent);
	m_notificationEventRepository->removeNotificationEvent(m_statusChangedToNotAvailableEvent);
	m_notificationEventRepository->removeNotificationEvent(m_statusChangedToDoNotDisturbEvent);
	m_notificationEventRepository->removeNotificationEvent(m_statusChangedToOfflineEvent);
}
 
void StatusNotificationService::notifyStatusChanged(Contact contact, Status oldStatus)
{
	if (contact.isAnonymous() || !contact.contactAccount())
		return;

	auto protocol = contact.contactAccount().protocolHandler();
	if (!protocol || !protocol->isConnected())
		return;

	if (m_notificationService->notifyIgnoreOnConnection())
	{
		auto dateTime = contact.contactAccount().property(QStringLiteral("notify:notify-account-connected"), QDateTime()).toDateTime();
		if (dateTime.isValid() && dateTime >= QDateTime::currentDateTime())
			return;
	}

	if (!contact.ownerBuddy().property(QStringLiteral("notify:Notify"), true).toBool())
		return;

	if (contact == contact.contactAccount().accountContact()) // myself
		return;

	auto status = contact.currentStatus();
	if (oldStatus == status)
		return;

	auto statusDisplayName = status.displayName();
	auto description = status.description();

	if (contact.ownerBuddy().property(QStringLiteral("kadu:HideDescription"), false).toBool())
	{
		if (oldStatus.type() == status.type())
			return;
		else
			description.clear();
	}

	if (m_notificationService->ignoreOnlineToOnline() &&
			!status.isDisconnected() &&
			!oldStatus.isDisconnected())
		return;

	auto const &typeData = m_statusTypeManager->statusTypeData(status.type());
	auto notificationEventName = QStringLiteral("StatusChanged/To%1").arg(typeData.name());

	auto chat = ChatTypeContact::findChat(m_chatManager, m_chatStorage, contact, ActionCreateAndAdd);
	auto data = QVariantMap{};
	data.insert(QStringLiteral("chat"), qVariantFromValue(chat));

	auto icon = contact.contactAccount().protocolHandler()->statusIcon(Status{m_statusTypeManager, contact.currentStatus().type()});
	
	auto notification = make_unique<Notification>(data, notificationEventName, icon);
	notification->addChatCallbacks();
	notification->setDetails(Qt::escape(description));
	notification->setText(tr("<b>%1</b> changed status to <i>%2</i>").arg(Qt::escape(contact.display(true)), Qt::escape(statusDisplayName)));
	notification->setTitle(tr("Status changed"));

	m_notificationService->notify(notification.release());
}
