/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

AggregateNotification::AggregateNotification(Notification *firstNotification) :
		Notification(firstNotification->type(), firstNotification->icon()),
		GroupKey(firstNotification->groupKey()),
		Identifier{firstNotification->identifier()},
		IsPeriodic{firstNotification->isPeriodic()},
		Period{firstNotification->period()}
{
	Notifications = QList<Notification *>();
	addNotification(firstNotification);
}

void AggregateNotification::addNotification(Notification* notification)
{
	Notifications.append(notification);

	connect(notification, SIGNAL(closed(Notification *)), this, SLOT(partialNotificationClosed(Notification *)));

	emit updated(this);
}

void AggregateNotification::close()
{
	if (!Closing)
	{
		Closing = true;

		foreach (Notification *n, Notifications)
		{
			n->close();
		}

		emit closed(this);
		deleteLater();
	}
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
	{
		details += n->details();
	}

	return details;
}

void AggregateNotification::clearCallbacks()
{
	if (!Notifications.empty())
		Notifications.first()->clearCallbacks();
}

void AggregateNotification::addCallback ( const QString& caption, const char* slot, const char* signature )
{
	if (!Notifications.empty())
		Notifications.first()->addCallback(caption, slot, signature);
}

void AggregateNotification::setDefaultCallback ( int timeout, const char* slot )
{
	if (!Notifications.empty())
		Notifications.first()->setDefaultCallback(timeout, slot);
}

bool AggregateNotification::requireCallback()
{
	if (!Notifications.empty())
		return Notifications.first()->requireCallback();
	else
		return false;
}

void AggregateNotification::callbackAccept()
{
	if (!Notifications.empty())
		Notifications.first()->callbackAccept();
}

void AggregateNotification::callbackDiscard()
{
	if (!Notifications.empty())
		Notifications.first()->callbackDiscard();
}

void AggregateNotification::clearDefaultCallback()
{
	if (!Notifications.empty())
		Notifications.first()->clearDefaultCallback();
}

void AggregateNotification::partialNotificationClosed(Notification *notification)
{
	Notifications.removeAll(notification);

	close();
}

#include "moc_aggregate-notification.cpp"
