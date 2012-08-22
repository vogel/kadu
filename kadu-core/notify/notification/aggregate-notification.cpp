/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
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

#include "protocols/connection-error-notification.h"

#include "aggregate-notification.h"


AggregateNotification::AggregateNotification(Notification *firstNotification)
		: Notification("aggregate", firstNotification->icon()), GroupKey(firstNotification->groupKey())
{
	Notifications = QList<Notification *>();
	Notifications.append(firstNotification);
	firstNotification->acquire();
}

AggregateNotification::~AggregateNotification()
{
	foreach (Notification *notification, Notifications)
		notification->release();
}

void AggregateNotification::addNotification(Notification* notification)
{
	Notifications.append(notification);
	notification->acquire();

	emit updated(this);
}

void AggregateNotification::close()
{
	foreach (Notification *notification, Notifications)
		notification->release();
	Notifications.clear();

	Notification::close();
}

const QString AggregateNotification::title() const
{
	return Notifications.size() > 1
			? QString("%1 (%2)").arg(Notifications.first()->title()).arg(Notifications.size())
			: Notifications.first()->title();
}

const QString AggregateNotification::text() const
{
	return Notifications.size() > 1
			? QString("%1 (%2)").arg(Notifications.first()->text()).arg(Notifications.size())
			: Notifications.first()->text();
}

const QStringList AggregateNotification::details() const
{
	QStringList details;

	foreach (Notification *n, Notifications)
		details += n->details();

	return details;
}
