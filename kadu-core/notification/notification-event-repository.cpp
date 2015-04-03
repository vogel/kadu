/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "notification-event-repository.h"

#include "notification/notification-event.h"

NotificationEventRepository::NotificationEventRepository(QObject *parent) :
		QObject{parent}
{
}

NotificationEventRepository::~NotificationEventRepository()
{
}

void NotificationEventRepository::addNotificationEvent(NotificationEvent event)
{
	auto it = std::find(std::begin(m_events), std::end(m_events), event);
	if (it == std::end(m_events))
	{
		m_events.push_back(event);
		emit notificationEventAdded(event);
	}
}

void NotificationEventRepository::removeNotificationEvent(NotificationEvent event)
{
	auto it = std::find(std::begin(m_events), std::end(m_events), event);
	if (it != std::end(m_events))
	{
		m_events.erase(it);
		emit notificationEventRemoved(event);
	}
}

const std::vector<NotificationEvent> & NotificationEventRepository::notificationEvents() const
{
	return m_events;
}

#include "moc_notification-event-repository.cpp"
