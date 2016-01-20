/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "notification/notification-event.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Account;
class ChatWidgetRepository;
class Contact;
class InjectedFactory;
class NotificationManager;

class OtrNotifier : public QObject
{
	Q_OBJECT

	static QString OtrNotifyTopic;
	static QString CreatePrivateKeyStartedNotifyTopic;
	static QString CreatePrivateKeyFinishedNotifyTopic;

	QPointer<ChatWidgetRepository> MyChatWidgetRepository;
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<NotificationManager> m_notificationManager;

	NotificationEvent OtrNotificationEvent;
	NotificationEvent CreatePrivateKeyStartedNotificationEvent;
	NotificationEvent CreatePrivateKeyFinishedNotificationEvent;

	void notify(const QString &topic, const Account &account, const QString &message);
	void notify(const Contact &contact, const QString &message);

public:
	Q_INVOKABLE OtrNotifier();
	virtual ~OtrNotifier();

	QList<NotificationEvent> notifyEvents();

public slots:
	INJEQT_SET void setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setNotificationManager(NotificationManager *notificationManager);

	void notifyTryingToStartSession(const Contact &contact);
	void notifyTryingToRefreshSession(const Contact &contact);
	void notifyPeerEndedSession(const Contact &contact);
	void notifyGoneSecure(const Contact &contact);
	void notifyGoneInsecure(const Contact &contact);
	void notifyStillSecure(const Contact &contact);
	void notifyCreatePrivateKeyStarted(const Account &account);
	void notifyCreatePrivateKeyFinished(const Account &account, bool ok);

};
