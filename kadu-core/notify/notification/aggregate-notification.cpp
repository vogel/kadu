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
}

void AggregateNotification::addNotification(Notification* notification)
{
	const ConnectionErrorNotification * const connectionErrorNotification = qobject_cast<const ConnectionErrorNotification * const>(notification);
	if (connectionErrorNotification && Notifications.size() > 0)
	{
		notification->close();
		return;
	}

	Notifications.append(notification);

	emit updated(this);
}

void AggregateNotification::close()
{
	emit closed(this);
}

const QString AggregateNotification::title() const
{
	return QString("%1 (%2)").arg(Notifications.first()->title()).arg(Notifications.size());
}

const QString AggregateNotification::text() const
{
	return QString("%1 (%2)").arg(Notifications.first()->text()).arg(Notifications.size());
}

const QStringList AggregateNotification::details() const
{
	QStringList details;

	foreach (Notification *n, Notifications)
	{
		details += n->details();
	}

	return details;
}

void AggregateNotification::clearCallbacks()
{
	Notifications.first()->clearCallbacks();
}

void AggregateNotification::addCallback ( const QString& caption, const char* slot, const char* signature )
{
	Notifications.first()->addCallback(caption, slot, signature);
}

void AggregateNotification::setDefaultCallback ( int timeout, const char* slot )
{
	Notifications.first()->setDefaultCallback(timeout, slot);
}

bool AggregateNotification::requireCallback()
{
	return Notifications.first()->requireCallback();
}

void AggregateNotification::callbackAccept()
{
	close();

	Notifications.first()->callbackAccept();
}

void AggregateNotification::callbackDiscard()
{
	close();

	Notifications.first()->callbackDiscard();
}

void AggregateNotification::clearDefaultCallback()
{
	Notifications.first()->clearDefaultCallback();
}

