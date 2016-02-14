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

#include "notification-dispatcher.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "notification/notification-configuration.h"
#include "notification/notification.h"
#include "notification/notifier-repository.h"
#include "notification/notifier.h"

NotificationDispatcher::NotificationDispatcher(QObject *parent) :
		QObject{parent}
{
}

NotificationDispatcher::~NotificationDispatcher()
{
}

void NotificationDispatcher::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void NotificationDispatcher::setNotificationConfiguration(NotificationConfiguration *notificationConfiguration)
{
	m_notificationConfiguration = notificationConfiguration;
}

void NotificationDispatcher::setNotifierRepository(NotifierRepository *notifierRepository)
{
	m_notifierRepository = notifierRepository;
}

bool NotificationDispatcher::dispatchNotification(const Notification &notification)
{
	auto notifyType = m_notificationConfiguration->notifyConfigurationKey(notification.type);
	auto foundNotifier = false;

	for (auto notifier : m_notifierRepository)
	{
		if (m_configuration->deprecatedApi()->readBoolEntry("Notify", notifyType + '_' + notifier->name()))
		{
			notifier->notify(notification);
			foundNotifier = true;
		}
	}

	return foundNotifier;
}
