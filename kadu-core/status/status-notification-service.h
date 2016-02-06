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

#pragma once

#include "injeqt-type-roles.h"

#include "notification/notification-event.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ChatManager;
class ChatStorage;
class Chat;
class Contact;
class InjectedFactory;
class NotificationEventRepository;
class NotificationService;
class StatusTypeManager;
class Status;

class StatusNotificationService : public QObject
{
	Q_OBJECT
	INJEQT_TYPE_ROLE(SERVICE)

public:
	Q_INVOKABLE explicit StatusNotificationService(QObject *parent = nullptr);
	virtual ~StatusNotificationService();

public slots:
	void notifyStatusChanged(Contact contact, Status oldStatus);

private:
	QPointer<ChatManager> m_chatManager;
	QPointer<ChatStorage> m_chatStorage;
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<NotificationEventRepository> m_notificationEventRepository;
	QPointer<NotificationService> m_notificationService;
	QPointer<StatusTypeManager> m_statusTypeManager;

	NotificationEvent m_statusChangedEvent;
	NotificationEvent m_statusChangedToFreeForChatEvent;
	NotificationEvent m_statusChangedToOnlineEvent;
	NotificationEvent m_statusChangedToAwayEvent;
	NotificationEvent m_statusChangedToNotAvailableEvent;
	NotificationEvent m_statusChangedToDoNotDisturbEvent;
	NotificationEvent m_statusChangedToOfflineEvent;

private slots:
	INJEQT_SET void setChatManager(ChatManager *chatManager);
	INJEQT_SET void setChatStorage(ChatStorage *chatStorage);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setNotificationEventRepository(NotificationEventRepository *notificationEventRepository);
	INJEQT_SET void setNotificationService(NotificationService *notificationService);
	INJEQT_SET void setStatusTypeManager(StatusTypeManager *statusTypeManager);
	INJEQT_INIT void init();
	INJEQT_DONE void done();

};
