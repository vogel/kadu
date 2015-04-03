/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtWidgets/QApplication>

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "gui/windows/message-dialog.h"
#include "notification/notification/aggregate-notification.h"
#include "notification/notifier.h"
#include "protocols/connection-error-notification.h"
#include "protocols/protocol.h"
#include "status/status-container-manager.h"
#include "debug.h"

#include "notification/notification-manager.h"

NotificationManager::NotificationManager(QObject *parent) :
		QObject{parent}
{
	//HACK force creating StatusContainerManager instance so Kadu won't crash at startup
	StatusContainerManager::instance();
}

NotificationManager::~NotificationManager()
{
	while (!Notifiers.isEmpty())
	{
		kdebugm(KDEBUG_WARNING, "WARNING: not unregistered notifiers found! (%u)\n", Notifiers.size());
		unregisterNotifier(Notifiers.at(0));
	}
}

void NotificationManager::registerNotifier(Notifier *notifier)
{
	kdebugf();

	if (Notifiers.contains(notifier))
	{
		kdebugm(KDEBUG_WARNING, "WARNING: '%s' already exists in notifiers! "
		"strange... unregistering old Notifier\n", qPrintable(notifier->name()));

		unregisterNotifier(notifier);
	}

	Notifiers.append(notifier);
	emit notiferRegistered(notifier);

	kdebugf2();
}

void NotificationManager::unregisterNotifier(Notifier *notifier)
{
	kdebugf();

	if (!Notifiers.contains(notifier))
	{
		kdebugm(KDEBUG_WARNING, "WARNING: '%s' not registered!\n", qPrintable(notifier->name()));
		return;
	}

	emit notiferUnregistered(notifier);
	Notifiers.removeAll(notifier);

	kdebugf2();
}

const QList<Notifier *> & NotificationManager::notifiers() const
{
	return Notifiers;
}

void NotificationManager::ignoreConnectionErrors(Account account)
{
	IgnoredAccounts.append(account.id());
	connect(account.protocolHandler(), SIGNAL(connected(Account)), this, SLOT(unignoreConnectionErrors(Account)));
}

void NotificationManager::unignoreConnectionErrors(Account account)
{
	IgnoredAccounts.removeAll(account.id());
}

void NotificationManager::notify(Notification *rawNotification)
{
	kdebugf();

	const ConnectionErrorNotification * const connectionErrorNotification = qobject_cast<const ConnectionErrorNotification * const>(rawNotification);
	if (connectionErrorNotification)
	{
		auto account = connectionErrorNotification->data()["account"].value<Account>();

		if (IgnoredAccounts.contains(account.id()))
		{
			rawNotification->close();
			return;
		}
	}

	if (rawNotification->isPeriodic())
	{
		if (PeriodicNotifications.contains(rawNotification->identifier()))
		{
			rawNotification->close();
			return;
		}
		else
		{
			PeriodicNotifications.insert(rawNotification->identifier(), 0);
		}
	}

	auto notification = findGroup(rawNotification);
	if (notification) // should update details
		return;

	notification = new AggregateNotification(rawNotification);
	connect(notification, SIGNAL(closed(Notification*)), this, SLOT(removeGrouped(Notification*)));
	ActiveNotifications.insert(notification->identifier(), notification);

	auto notifyType = rawNotification->key();
	auto foundNotifier = false;

	for (auto notifier : Notifiers)
	{
		if (Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Notify", notifyType + '_' + notifier->name()))
		{
			notifier->notify(notification);
			foundNotifier = true;
		}
	}

	if (!foundNotifier)
		notification->callbackDiscard();

	kdebugf2();
}

AggregateNotification * NotificationManager::findGroup(Notification *rawNotification)
{
	auto aggregate = ActiveNotifications.value(rawNotification->identifier());
	if (aggregate)
		aggregate->addNotification(rawNotification);

	return aggregate;
}

void NotificationManager::removeGrouped(Notification *notification)
{
	ActiveNotifications.remove(notification->identifier());

	if (notification->isPeriodic())
	{
		QTimer *timer = new QTimer();
		timer->setInterval(notification->period()*1000);
		connect(timer, SIGNAL(timeout()), this, SLOT(removePeriodicEntries()));
		timer->start();
		PeriodicNotifications.insert(notification->identifier(), timer);
	}
}

void NotificationManager::removePeriodicEntries()
{
	QTimer *t = qobject_cast<QTimer*>(sender());
	PeriodicNotifications.remove(PeriodicNotifications.key(t));
}

QString NotificationManager::notifyConfigurationKey(const QString &eventType)
{
	QString event = eventType;

	while (true)
	{
		int slashPosition = event.lastIndexOf('/');
		if (-1 == slashPosition)
			return event;

		if (Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Notify", event + "_UseCustomSettings", false))
			return event;

		event = event.left(slashPosition);
	}

	Q_ASSERT(false);
}

#include "moc_notification-manager.cpp"
