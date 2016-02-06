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
#include "core/injected-factory.h"
#include "identities/identity.h"
#include "notification/notification/notification.h"
#include "notification/notification/status-changed-notification.h"
#include "notification/notification-event-repository.h"
#include "notification/notification-event.h"
#include "notification/notification-service.h"
#include "protocols/protocol.h"
#include "status/status-type-data.h"
#include "status/status-type-manager.h"

StatusNotificationService::StatusNotificationService(QObject *parent) :
		QObject{parent}
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

void StatusNotificationService::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
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
}

void StatusNotificationService::done()
{
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
		auto dateTime = contact.contactAccount().property("notify:notify-account-connected", QDateTime()).toDateTime();
		if (dateTime.isValid() && dateTime >= QDateTime::currentDateTime())
			return;
	}

	if (!contact.ownerBuddy().property("notify:Notify", true).toBool())
		return;

	if (contact == contact.contactAccount().accountContact()) // myself
		return;

	auto status = contact.currentStatus();
	if (oldStatus == status)
		return;

	auto statusDisplayName = status.displayName();
	auto description = status.description();

	if (contact.ownerBuddy().property("kadu:HideDescription", false).toBool())
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
	QString changedTo = "/To" + typeData.name();

	auto statusChangedNotification = m_injectedFactory->makeInjected<StatusChangedNotification>(m_statusTypeManager, m_chatManager, m_chatStorage, changedTo, contact, statusDisplayName, description);
	m_notificationService->notify(statusChangedNotification);
}
