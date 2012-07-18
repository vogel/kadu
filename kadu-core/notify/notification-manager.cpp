/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004 Tomasz Jarzynka (tomee@cpi.pl)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2007, 2008, 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "buddies/buddy-manager.h"
#include "buddies/group-manager.h"
#include "buddies/group.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "core/core.h"
#include "gui/actions/action.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/custom-input.h"
#include "gui/widgets/talkable-menu-manager.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/main-configuration-window.h"
#include "gui/windows/main-window.h"
#include "gui/windows/message-dialog.h"
#include "message/message-manager.h"
#include "message/message.h"
#include "multilogon/multilogon-session.h"
#include "notify/account-notification.h"
#include "notify/notification.h"
#include "notify/notifier.h"
#include "notify/multilogon-notification.h"
#include "notify/new-message-notification.h"
#include "notify/notify-configuration-ui-handler.h"
#include "notify/status-changed-notification.h"
#include "notify/window-notifier.h"
#include "protocols/services/multilogon-service.h"
#include "status/status-container-manager.h"
#include "status/status-type-data.h"
#include "status/status-type-manager.h"

#include "misc/misc.h"
#include "activate.h"
#include "debug.h"


NotificationManager *NotificationManager::Instance = 0;

NotificationManager * NotificationManager::instance()
{
	if (!Instance)
	{
		Instance = new NotificationManager();
		Instance->init();
	}

	return Instance;
}

NotificationManager::NotificationManager()
{
}

void NotificationManager::init()
{
	kdebugf();

	//TODO 0.10.0:
	//triggerAllAccountsRegistered();

	connect(StatusContainerManager::instance(), SIGNAL(statusUpdated()), this, SLOT(statusUpdated()));

	foreach (const Group &group, GroupManager::instance()->items())
		groupAdded(group);

	new WindowNotifier(this);
	kdebugf2();
}

NotificationManager::~NotificationManager()
{
	kdebugf();

	triggerAllAccountsUnregistered();

	while (!Notifiers.isEmpty())
	{
		kdebugm(KDEBUG_WARNING, "WARNING: not unregistered notifiers found! (%u)\n", Notifiers.size());
		unregisterNotifier(Notifiers.at(0));
	}

	kdebugf2();
}

void NotificationManager::statusUpdated()
{
/*	if (SilentModeWhenDnD && !silentMode() && StatusContainerManager::instance()->status().type() == StatusTypeDoNotDisturb)
	{
		foreach (Action *action, SilentModeActionDescription->actions())
			action->setChecked(false);

		AutoSilentMode = true;
	}
	else if (!silentMode() && AutoSilentMode)
	{
		foreach (Action *action, SilentModeActionDescription->actions())
			action->setChecked(true);

		AutoSilentMode = false;
	}*/
}

void NotificationManager::accountRegistered(Account account)
{
	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return;

	connect(account, SIGNAL(buddyStatusChanged(Contact, Status)),
			this, SLOT(contactStatusChanged(Contact, Status)));
	connect(account, SIGNAL(connected()), this, SLOT(accountConnected()));

	MultilogonService *multilogonService = protocol->multilogonService();
	if (multilogonService)
	{
		connect(multilogonService, SIGNAL(multilogonSessionConnected(MultilogonSession*)),
				this, SLOT(multilogonSessionConnected(MultilogonSession*)));
		connect(multilogonService, SIGNAL(multilogonSessionDisconnected(MultilogonSession*)),
				this, SLOT(multilogonSessionDisconnected(MultilogonSession*)));
	}
}

void NotificationManager::accountUnregistered(Account account)
{
	Protocol *protocol = account.protocolHandler();

	if (!protocol)
		return;

	disconnect(account, 0, this, 0);

	MultilogonService *multilogonService = protocol->multilogonService();
	if (multilogonService)
		disconnect(multilogonService, 0, this, 0);
}

void NotificationManager::accountConnected()
{
/*	Account account(sender());
	if (!account)
		return;

	if (NotifyIgnoreOnConnection)
		account.addProperty("notify:notify-account-connected", QDateTime::currentDateTime().addSecs(10), CustomProperties::NonStorable);*/
}

void NotificationManager::contactStatusChanged(Contact contact, Status oldStatus)
{
	kdebugf();
/*
	if (contact.isAnonymous() || !contact.contactAccount())
		return;

	Protocol *protocol = contact.contactAccount().protocolHandler();
	if (!protocol || !protocol->isConnected())
		return;

	if (NotifyIgnoreOnConnection)
	{
		QDateTime dateTime = contact.contactAccount().property("notify:notify-account-connected", QDateTime()).toDateTime();
		if (dateTime.isValid() && dateTime >= QDateTime::currentDateTime())
			return;
	}

	bool notify_contact = true;
	if (!contact.ownerBuddy().property("notify:Notify", false).toBool())
		notify_contact = false;

	if (!notify_contact && !NotifyAboutAll)
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: not notifying user AND not notifying all users\n");
		return;
	}

	if (contact == contact.contactAccount().accountContact()) // myself
		return;

	Status status = contact.currentStatus();
	if (oldStatus == status)
		return;

	if (IgnoreOnlineToOnline &&
			!status.isDisconnected() &&
			!oldStatus.isDisconnected())
		return;

	const StatusTypeData &typeData = StatusTypeManager::instance()->statusTypeData(status.type());
	QString changedTo = "/To" + typeData.name();

	StatusChangedNotification *statusChangedNotification = new StatusChangedNotification(changedTo, contact);

	notify(statusChangedNotification);
*/
	kdebugf2();
}

void NotificationManager::multilogonSessionConnected(MultilogonSession *session)
{
	MultilogonNotification::notifyMultilogonSessionConnected(session);
}

void NotificationManager::multilogonSessionDisconnected(MultilogonSession *session)
{
	MultilogonNotification::notifyMultilogonSessionDisonnected(session);
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

void NotificationManager::notify(Notification *notification)
{
	kdebugf();

	QString notifyType = notification->key();
	bool foundNotifier = false;
	bool foundNotifierWithCallbackSupported = !notification->requireCallback();

// 	if (ignoreNotifications())
// 	{
// 		notification->callbackDiscard();
// 		return;
// 	}

	notification->acquire();

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

	notification->release();

	if (!foundNotifierWithCallbackSupported)
		MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), tr("Unable to find notifier for %1 event").arg(notification->type()));

	kdebugf2();
}

void NotificationManager::groupAdded(const Group &group)
{
	connect(group, SIGNAL(updated()), this, SLOT(groupUpdated()));
}

void NotificationManager::groupUpdated()
{
/*	Group group = sender();
	if (group.isNull())
		return;

	bool notify = group.notifyAboutStatusChanges();

	if (NotifyAboutAll && !notify)
	{
		NotifyAboutAll = false;
		config_file.writeEntry("Notify", "NotifyAboutAll", false);
	}

	foreach (const Buddy &buddy, BuddyManager::instance()->items())
	{
		if (buddy.isNull() || buddy.isAnonymous() || buddy.groups().contains(group))
			continue;

		if (notify)
			buddy.addProperty("notify:Notify", true, CustomProperties::Storable);
		else
			buddy.removeProperty("notify:Notify");
	}*/
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

void checkNotify(Action *action)
{
	kdebugf();

	action->setEnabled(!action->context()->buddies().isEmpty());

	bool on = true;
	foreach (const Buddy &buddy, action->context()->contacts().toBuddySet())
		if (buddy.data())
		{
			if (!buddy.data()->customProperties()->property("notify:Notify", false).toBool())
			{
				on = false;
				break;
			}
		}

	action->setChecked(on);

	kdebugf2();
}
