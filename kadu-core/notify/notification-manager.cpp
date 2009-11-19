/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QApplication>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "chat/message/message.h"
#include "configuration/configuration-file.h"
#include "buddies/buddy-manager.h"
#include "buddies/group.h"
#include "buddies/group-manager.h"
#include "contacts/contact.h"
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
	//TODO 0.6.6:
	//triggerAllAccountsRegistered();

	notifyAboutUserActionDescription = new ActionDescription(0,
		ActionDescription::TypeUser, "notifyAboutUserAction",
		this, SLOT(notifyAboutUserActionActivated(QAction *, bool)),
		"NotifyAboutUser", tr("Notify about user"), true, "",
		checkNotify
	);

	BuddiesListViewMenuManager::instance()->addManagementActionDescription(notifyAboutUserActionDescription);

	foreach (Group group, GroupManager::instance()->groups())
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

	if (NotifyAboutAll)
	{
		NotifyAboutAll = false;
		config_file.writeEntry("Notify", "NotifyAboutAll", false);
	}

	BuddySet buddies = window->buddies();

	bool on = true;
	foreach (const Buddy buddy, buddies)
	{
		ContactNotifyData *cnd = buddy.moduleData<ContactNotifyData>();

		if (!cnd || !cnd->notify())
		{
			on = false;
			break;
		}

		delete cnd;
	}

	foreach (const Buddy buddy, buddies)
	{
		if (buddy.isNull() || buddy.isAnonymous())
			continue;

		ContactNotifyData *cnd = buddy.moduleData<ContactNotifyData>();
		if (!cnd)
			continue;

		if (cnd->notify() == on)
		{
			cnd->setNotify(!on);
			cnd->storeConfiguration();
			delete cnd;
		}
	}

	foreach (Action *action, notifyAboutUserActionDescription->actions())
		if (action->buddies() == buddies)
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
	connect(account.data(), SIGNAL(buddyStatusChanged(Account, Buddy, Status)),
			this, SLOT(statusChanged(Account, Buddy, Status)));

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
	disconnect(protocol, SIGNAL(connectionError(Account, const QString &, const QString &)),
			this, SLOT(connectionError(Account, const QString &, const QString &))); // TODO: fix
	disconnect(account.data(), SIGNAL(buddyStatusChanged(Account, Buddy, Status)),
			this, SLOT(statusChanged(Account, Buddy, Status)));

	ChatService *chatService = protocol->chatService();
	if (chatService)
	{
		disconnect(chatService, SIGNAL(messageReceived(const Message &)),
				this, SLOT(messageReceived(const Message &)));
	}
}

void NotificationManager::statusChanged(Account account, Buddy buddy, Status oldStatus)
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
	ContactNotifyData *cnd = buddy.moduleData<ContactNotifyData>();

	if (!cnd || !cnd->notify())
		notify_contact = false;

	if (cnd)
		delete cnd;

	if (!notify_contact && !NotifyAboutAll)
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: not notifying user AND not notifying all users\n");
		return;
	}

	if (buddy.id(account) == account.id()) // myself
		return;

	Contact data = buddy.contact(account);
	if (data.isNull())
		return;

	if (oldStatus == data.currentStatus())
		return;

	if (config_file.readBoolEntry("Notify", "IgnoreOnlineToOnline") &&
			!data.currentStatus().isDisconnected() &&
			!oldStatus.isDisconnected())
		return;

	QString changedTo = "/To" + Status::name(data.currentStatus(), false);

	BuddySet buddies(buddy);

	StatusChangedNotification *statusChangedNotification;
	if (config_file.readBoolEntry("Notify", "StatusChanged" + changedTo + "_UseCustomSettings", true))
		statusChangedNotification = new StatusChangedNotification(changedTo, buddies, account);
	else
		statusChangedNotification = new StatusChangedNotification("", buddies, account);

	notify(statusChangedNotification);

	kdebugf2();
}

void NotificationManager::messageReceived(const Message &message)
{
	kdebugf();

	ChatWidget *chatWidget = ChatWidgetManager::instance()->byChat(message.chat());
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

	QString notifyType = notification->type();
	bool foundNotifier = false;
	bool foundNotifierWithCallbackSupported = notification->getCallbacks().count() == 0;

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

	foreach (const Buddy buddy, BuddyManager::instance()->buddies())
	{
		if (buddy.isNull() || buddy.isAnonymous() || buddy.groups().contains(group))
			continue;

		ContactNotifyData *cnd = buddy.moduleData<ContactNotifyData>();
		if (!cnd)
			continue;

		cnd->setNotify(notify);
		cnd->storeConfiguration();
		delete cnd;
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

ConfigurationUiHandler * NotificationManager::configurationUiHandler()
{
	return UiHandler;
}

void checkNotify(Action *action)
{
	kdebugf();

	foreach(Buddy buddy, action->buddies())
		if (!buddy.hasContact(buddy.prefferedAccount()))
		{
			action->setEnabled(false);
			return;
		}

	action->setEnabled(true);

	bool on = true;
	foreach (const Buddy buddy, action->buddies())
	{
		ContactNotifyData *cnd = buddy.moduleData<ContactNotifyData>();

		if (!cnd || !cnd->notify())
		{
			on = false;
			break;
		}

		delete cnd;
	}

	action->setChecked(on);

	kdebugf2();
}
