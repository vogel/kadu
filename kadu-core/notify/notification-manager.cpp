/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004 Tomasz Jarzynka (tomee@cpi.pl)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2007, 2008, 2009, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008, 2009 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
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

#include <QtGui/QApplication>

#include "configuration/configuration-file.h"
#include "gui/windows/message-dialog.h"
#include "notify/notification/aggregate-notification.h"
#include "notify/notifier.h"
#include "protocols/connection-error-notification.h"
#include "protocols/protocol.h"
#include "status/status-container-manager.h"
#include "debug.h"

#include "notify/notification-manager.h"


NotificationManager *NotificationManager::Instance = 0;

NotificationManager * NotificationManager::instance()
{
	if (!Instance)
	{
		Instance = new NotificationManager();

		//HACK force creating StatusContainerManager instance so Kadu won't crash at startup
		StatusContainerManager::instance();
	}

	return Instance;
}

NotificationManager::NotificationManager()
{
}

NotificationManager::~NotificationManager()
{
	while (!Notifiers.isEmpty())
	{
		kdebugm(KDEBUG_WARNING, "WARNING: not unregistered notifiers found! (%u)\n", Notifiers.size());
		unregisterNotifier(Notifiers.at(0));
	}
}

void NotificationManager::registerNotifyEvent(NotifyEvent *notifyEvent)
{
	kdebugf();

	NotifyEvents.append(notifyEvent);
	emit notifyEventRegistered(notifyEvent);

	kdebugf2();
}

void NotificationManager::unregisterNotifyEvent(NotifyEvent *notifyEvent)
{
	kdebugf();

	NotifyEvents.removeAll(notifyEvent);
	emit notifyEventUnregistered(notifyEvent);

	kdebugf2();
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

const QList<NotifyEvent *> & NotificationManager::notifyEvents() const
{
	return NotifyEvents;
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
		Account account = connectionErrorNotification->account();

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

	Notification *notification = findGroup(rawNotification);

	QString notifyType = rawNotification->key();
	bool foundNotifier = false;
	bool foundNotifierWithCallbackSupported = !rawNotification->requireCallback();

	foreach (Notifier *notifier, Notifiers)
	{
		if (config_file.readBoolEntry("Notify", notifyType + '_' + notifier->name()))
		{
			notifier->notify(notification);
			foundNotifier = true;
			foundNotifierWithCallbackSupported = foundNotifierWithCallbackSupported ||
					(Notifier::CallbackSupported == notifier->callbackCapacity());
		}
	}

	if (!foundNotifierWithCallbackSupported)
		foreach (Notifier *notifier, Notifiers)
		{
			if (Notifier::CallbackSupported == notifier->callbackCapacity())
			{
				notifier->notify(notification);
				foundNotifier = true;
				foundNotifierWithCallbackSupported = true;
				break;
			}
		}

	if (!foundNotifier)
		notification->callbackDiscard();

	if (!foundNotifierWithCallbackSupported)
		MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), tr("Unable to find notifier for %1 event").arg(rawNotification->type()));

	kdebugf2();
}

Notification * NotificationManager::findGroup(Notification *rawNotification)
{
	AggregateNotification *aggregate = ActiveNotifications.value(rawNotification->identifier());

	if (aggregate)
	{
		aggregate->addNotification(rawNotification);
	}
	else
	{
		aggregate = new AggregateNotification(rawNotification);
		connect(aggregate, SIGNAL(closed(Notification*)), this, SLOT(removeGrouped(Notification*)));
	}

	ActiveNotifications.insert(aggregate->identifier(), aggregate);

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

		if (config_file.readBoolEntry("Notify", event + "_UseCustomSettings", false))
			return event;

		event = event.left(slashPosition);
	}

	Q_ASSERT(false);
}

#include "moc_notification-manager.cpp"
