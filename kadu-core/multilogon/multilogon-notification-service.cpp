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

#include "multilogon-notification-service.h"

#include "accounts/account.h"
#include "multilogon/multilogon-session.h"
#include "notification/notification.h"
#include "notification/notification-callback-repository.h"
#include "notification/notification-callback.h"
#include "notification/notification-event-repository.h"
#include "notification/notification-event.h"
#include "notification/notification-service.h"
#include "protocols/protocol.h"
#include "protocols/services/multilogon-service.h"

MultilogonNotificationService::MultilogonNotificationService(QObject *parent) :
		QObject{parent},
		m_mutlilogonDisconnectCallback{QStringLiteral("multilogon-disconnect"), tr("Disconnect session"),
			[this](const Notification &notification){ return disconnectSession(notification); }},
		m_multilogonEvent{QStringLiteral("multilogon"), QStringLiteral(QT_TRANSLATE_NOOP("@default", "Multilogon"))},
		m_multilogonConnectedEvent{QStringLiteral("multilogon/sessionConnected"), QStringLiteral(QT_TRANSLATE_NOOP("@default", "Multilogon session connected"))},
		m_multilogonDisconnectedEvent{QStringLiteral("multilogon/sessionDisconnected"), QStringLiteral(QT_TRANSLATE_NOOP("@default", "Multilogon session disconnected"))}
{
}

MultilogonNotificationService::~MultilogonNotificationService()
{
}

void MultilogonNotificationService::setNotificationCallbackRepository(NotificationCallbackRepository *notificationCallbackRepository)
{
	m_notificationCallbackRepository = notificationCallbackRepository;
}

void MultilogonNotificationService::setNotificationEventRepository(NotificationEventRepository *notificationEventRepository)
{
	m_notificationEventRepository = notificationEventRepository;
}

void MultilogonNotificationService::setNotificationService(NotificationService *notificationService)
{
	m_notificationService = notificationService;
}

void MultilogonNotificationService::init()
{
	m_notificationEventRepository->addNotificationEvent(m_multilogonEvent);
	m_notificationEventRepository->addNotificationEvent(m_multilogonConnectedEvent);
	m_notificationEventRepository->addNotificationEvent(m_multilogonDisconnectedEvent);

	m_notificationCallbackRepository->addCallback(m_mutlilogonDisconnectCallback);
}

void MultilogonNotificationService::done()
{
	m_notificationEventRepository->removeNotificationEvent(m_multilogonEvent);
	m_notificationEventRepository->removeNotificationEvent(m_multilogonConnectedEvent);
	m_notificationEventRepository->removeNotificationEvent(m_multilogonDisconnectedEvent);

	m_notificationCallbackRepository->removeCallback(m_mutlilogonDisconnectCallback);
}

void MultilogonNotificationService::notifyMultilogonSessionConnected(const MultilogonSession &session)
{
	auto data = QVariantMap{};
	data.insert(QStringLiteral("account"), qVariantFromValue(session.account));
	data.insert(QStringLiteral("multilogon-session"), qVariantFromValue(session));

	auto notification = Notification{data, QStringLiteral("multilogon/sessionConnected"), KaduIcon{}};
	notification.setTitle(tr("Multilogon"));
	notification.setText(tr("Multilogon session connected from %1 at %2 with %3 for %4 account").arg(
			session.remoteAddress.toString(),
			session.logonTime.toString(),
			session.name,
			session.account.id()));
	notification.addCallback(QStringLiteral("ignore"));
	notification.addCallback(m_mutlilogonDisconnectCallback.name());

	m_notificationService->notify(notification);
}

void MultilogonNotificationService::notifyMultilogonSessionDisonnected(const MultilogonSession &session)
{
	auto data = QVariantMap{};
	data.insert(QStringLiteral("account"), qVariantFromValue(session.account));

	auto notification = Notification{data, QStringLiteral("multilogon/sessionDisonnected"), KaduIcon{}};
	notification.setTitle(tr("Multilogon"));
	notification.setText(tr("Multilogon session disconnected from %1 at %2 with %3 for %4 account").arg(
			session.remoteAddress.toString(),
			session.logonTime.toString(),
			session.name,
			session.account.id()));

	m_notificationService->notify(notification);
}

void MultilogonNotificationService::disconnectSession(const Notification &notification)
{
	auto session = qvariant_cast<MultilogonSession>(notification.data()[QStringLiteral("multilogon-session")]);
	if (session == MultilogonSession{})
		return;

	auto protocolHandler = session.account.protocolHandler();
	if (!protocolHandler)
		return;

	auto multilogonService = protocolHandler->multilogonService();
	if (!multilogonService)
		return;

	multilogonService->killSession(session);
}
