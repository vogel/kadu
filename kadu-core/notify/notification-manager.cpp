/*
 * %kadu copyright begin%
 * Copyright 2007, 2008, 2009 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004 Tomasz Jarzynka (tomee@cpi.pl)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@go2.pl)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
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

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-shared.h"
#include "buddies/group.h"
#include "buddies/group-manager.h"
#include "chat/message/message.h"
#include "configuration/configuration-file.h"
#include "contacts/contact.h"
#include "contacts/contact-manager.h"
#include "gui/actions/action.h"
#include "gui/widgets/buddies-list-view-menu-manager.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/custom-input.h"
#include "gui/windows/main-configuration-window.h"
#include "gui/windows/main-window.h"
#include "gui/windows/message-dialog.h"
#include "notify/contact-notify-data.h"
#include "notify/notifier.h"
#include "notify/notify-configuration-ui-handler.h"
#include "notify/window-notifier.h"

#include "debug.h"
#include "misc/misc.h"

#include "new-message-notification.h"
#include "status-changed-notification.h"

NotificationManager *NotificationManager::Instance = 0;

NotificationManager * NotificationManager::instance()
{
	if (!Instance)
	{
		Instance = new NotificationManager();
		Instance->init();

		MessageNotification::registerEvents();
		StatusChangedNotification::registerEvents();
	}

	return Instance;
}

NotificationManager::NotificationManager()
{
}

void NotificationManager::init()
{
    	kdebugf();

	UiHandler = new NotifyConfigurationUiHandler(this);
	MainConfigurationWindow::registerUiHandler(UiHandler);

	createDefaultConfiguration();
	configurationUpdated();
	//TODO 0.6.6:
	//triggerAllAccountsRegistered();

	notifyAboutUserActionDescription = new ActionDescription(0,
		ActionDescription::TypeUser, "notifyAboutUserAction",
		this, SLOT(notifyAboutUserActionActivated(QAction *, bool)),
		"NotifyAboutUser", tr("Notify about user"), true, "",
		checkNotify
	);

	BuddiesListViewMenuManager::instance()->addManagementActionDescription(notifyAboutUserActionDescription);

	foreach (Group group, GroupManager::instance()->items())
		groupAdded(group);

	new WindowNotifier(this);
	kdebugf2();
}

NotificationManager::~NotificationManager()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiHandler(UiHandler);

	BuddiesListViewMenuManager::instance()->removeManagementActionDescription(notifyAboutUserActionDescription);
	delete notifyAboutUserActionDescription;
	notifyAboutUserActionDescription = 0;

	StatusChangedNotification::unregisterEvents();
	MessageNotification::unregisterEvents();

	triggerAllAccountsUnregistered();

	while (!Notifiers.isEmpty())
	{
		kdebugm(KDEBUG_WARNING, "WARNING: not unregistered notifiers found! (%u)\n", Notifiers.size());
		unregisterNotifier(Notifiers[0]);
	}

	kdebugf2();
}

void NotificationManager::notifyAboutUserActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	BuddySet buddies = window->contacts().toBuddySet();

	bool on = true;
	foreach (const Buddy buddy, buddies)
	{
		ContactNotifyData *cnd = 0;
		if (buddy.data())
			cnd = buddy.data()->moduleStorableData<ContactNotifyData>("notify");

		if (!cnd || !cnd->notify())
		{
			on = false;
			break;
		}
	}

	if (NotifyAboutAll)
	{
		NotifyAboutAll = false;
		config_file.writeEntry("Notify", "NotifyAboutAll", false);
	}

	foreach (const Buddy buddy, buddies)
	{
		if (buddy.isNull() || buddy.isAnonymous())
			continue;

		ContactNotifyData *cnd = 0;
		if (buddy.data())
			cnd = buddy.data()->moduleStorableData<ContactNotifyData>("notify");
		if (!cnd)
			continue;

		if (cnd->notify() == on)
		{
			cnd->setNotify(!on);
			cnd->store();
		}
	}

	foreach (Action *action, notifyAboutUserActionDescription->actions())
		if (action->contacts().toBuddySet() == buddies)
			action->setChecked(!on);

	kdebugf2();
}

void NotificationManager::accountRegistered(Account account)
{
	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return;
// 	TODO: 0.6.6
// 	connect(protocol, SIGNAL(connectionError(Account, const QString &, const QString &)),
// 			this, SLOT(connectionError(Account, const QString &, const QString &)));
	connect(account.data(), SIGNAL(buddyStatusChanged(Contact, Status)),
			this, SLOT(statusChanged(Contact, Status)));

	ChatService *chatService = protocol->chatService();
	if (chatService)
	{
		connect(chatService, SIGNAL(messageReceived(const Message &)),
				this, SLOT(messageReceived(const Message &)));
	}
}

void NotificationManager::accountUnregistered(Account account)
{
	Protocol *protocol = account.protocolHandler();

	if (!protocol)
		return;

// 	disconnect(protocol, SIGNAL(connectionError(Account, const QString &, const QString &)),
// 			this, SLOT(connectionError(Account, const QString &, const QString &))); // TODO: 0.6.6 fix
	disconnect(account.data(), SIGNAL(buddyStatusChanged(Contact, Status)),
			this, SLOT(statusChanged(Contact, Status)));

	ChatService *chatService = protocol->chatService();
	if (chatService)
	{
		disconnect(chatService, SIGNAL(messageReceived(const Message &)),
				this, SLOT(messageReceived(const Message &)));
	}
}

void NotificationManager::statusChanged(Contact contact, Status oldStatus)
{
	kdebugf();

	// TODO 0.6.6
	/*if (massively && config_file.readBoolEntry("Notify", "NotifyIgnoreOnConnection"))
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: ignore on connection\n");
		return;
	}*/

	// TODO 0.6.6 display -> uuid?
	bool notify_contact = true;
	ContactNotifyData *cnd = 0;
	Buddy buddy = contact.ownerBuddy();
	if (buddy.data())
		cnd = buddy.data()->moduleStorableData<ContactNotifyData>("notify");

	if (!cnd || !cnd->notify())
		notify_contact = false;

	if (!notify_contact && !NotifyAboutAll)
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: not notifying user AND not notifying all users\n");
		return;
	}

	if (!contact.contactAccount())
		return;

	if (contact == contact.contactAccount().accountContact()) // myself
		return;

	Status status = contact.currentStatus();
	if (oldStatus == status)
		return;

	if (config_file.readBoolEntry("Notify", "IgnoreOnlineToOnline") &&
			!status.isDisconnected() &&
			!oldStatus.isDisconnected())
		return;

	QString changedTo = "/To" + Status::name(status, false);

	ContactSet contacts(contact);

	StatusChangedNotification *statusChangedNotification;
	statusChangedNotification = new StatusChangedNotification(changedTo, contacts);

	notify(statusChangedNotification);

	kdebugf2();
}

void NotificationManager::messageReceived(const Message &message)
{
	kdebugf();

	ChatWidget *chatWidget = ChatWidgetManager::instance()->byChat(message.messageChat());
	if (!chatWidget) // new chat
		notify(new MessageNotification(MessageNotification::NewChat, message));
	else // new message in chat
		if (!chatWidget->edit()->hasFocus() || !config_file.readBoolEntry("Notify", "NewMessageOnlyIfInactive"))
			notify(new MessageNotification(MessageNotification::NewMessage, message));

	kdebugf2();
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

QList<Notifier *> NotificationManager::notifiers()
{
	return Notifiers;
}

QList<NotifyEvent *> NotificationManager::notifyEvents()
{
	return NotifyEvents;
}

void NotificationManager::notify(Notification *notification)
{
	kdebugf();

	QString notifyType = notification->key();
	bool foundNotifier = false;
	bool foundNotifierWithCallbackSupported = !notification->requireCallback();

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
		MessageDialog::msg(tr("Unable to find notifier for %1 event").arg(notification->type()), true, "Warning");

	kdebugf2();
}

void NotificationManager::groupAdded(Group group)
{
	connect(group, SIGNAL(updated()), this, SLOT(groupUpdated()));
}

void NotificationManager::groupUpdated()
{
	Group group = sender();
	if (group.isNull())
		return;

	bool notify = group.notifyAboutStatusChanges();

	if (NotifyAboutAll)
	{
		NotifyAboutAll = false;
		config_file.writeEntry("Notify", "NotifyAboutAll", false);
	}

	foreach (const Buddy buddy, BuddyManager::instance()->items())
	{
		if (buddy.isNull() || buddy.isAnonymous() || buddy.groups().contains(group))
			continue;

		ContactNotifyData *cnd = 0;
		if (buddy.data())
			buddy.data()->moduleStorableData<ContactNotifyData>("notify");
		if (!cnd)
			continue;

		cnd->setNotify(notify);
		cnd->store();
	}
}

void NotificationManager::configurationUpdated()
{
	NotifyAboutAll = config_file.readBoolEntry("Notify", "NotifyAboutAll");
}

void NotificationManager::createDefaultConfiguration()
{
	config_file.addVariable("Notify", "IgnoreOnlineToOnline", false);
	config_file.addVariable("Notify", "NewMessageOnlyIfInactive", true);
	config_file.addVariable("Notify", "NotifyAboutAll", true);
	config_file.addVariable("Notify", "NotifyIgnoreOnConnection", true);
}

QString NotificationManager::notifyConfigurationKey(const QString &eventType)
{
	QString event = eventType;

	while (true)
	{
		int slashPosition = event.lastIndexOf('/');
		if (-1 == slashPosition)
			return event;

		if (config_file.readBoolEntry("Notify", event + "_UseCustomSettings"))
			return event;

		event = event.left(slashPosition);
	}
}

ConfigurationUiHandler * NotificationManager::configurationUiHandler()
{
	return UiHandler;
}

void checkNotify(Action *action)
{
	kdebugf();

	action->setEnabled(true);

	bool on = true;
	foreach (const Buddy buddy, action->contacts().toBuddySet())
	{
		ContactNotifyData *cnd = 0;
		if (buddy.data())
			cnd = buddy.data()->moduleStorableData<ContactNotifyData>("notify");

		if (!cnd || !cnd->notify())
		{
			on = false;
			break;
		}
	}

	action->setChecked(on);

	kdebugf2();
}
