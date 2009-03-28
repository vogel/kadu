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
#include "accounts/account_manager.h"
#include "chat/chat_manager.h"
#include "contacts/contact-account-data.h"
#include "gui/widgets/contacts-list-widget-menu-manager.h"
#include "gui/widgets/custom_input.h"

#include "action.h"
#include "config_file.h"
#include "debug.h"
#include "kadu_main_window.h"
#include "main_configuration_window.h"
#include "message_box.h"
#include "misc/misc.h"

#include "contact-notify-data.h"
#include "notify-configuration-ui-handler.h"

#include "connection_error_notification.h"
#include "new_message_notification.h"
#include "status_changed_notification.h"

extern "C" KADU_EXPORT int notify_init(bool firstLoad)
{
	kdebugf();

	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/notify.ui"), NotificationManager::instance()->configurationUiHandler());

	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void notify_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/notify.ui"), NotificationManager::instance()->configurationUiHandler());

	kdebugf2();
}

NotificationManager *NotificationManager::Instance = 0;

NotificationManager * NotificationManager::instance()
{
	if (!Instance)
	{
		Instance = new NotificationManager();

		MessageNotification::registerEvents();
		ConnectionErrorNotification::registerEvent();
		StatusChangedNotification::registerEvents();
	}

	return Instance;
}

NotificationManager::NotificationManager()
{
	kdebugf();

	Instance = this; // TODO: 0.6.6, hack
	UiHandler = new NotifyConfigurationUiHandler(this);

	createDefaultConfiguration();

	triggerAllAccountsRegistered();

	notifyAboutUserActionDescription = new ActionDescription(0,
		ActionDescription::TypeUser, "notifyAboutUserAction",
		this, SLOT(notifyAboutUserActionActivated(QAction *, bool)),
		"NotifyAboutUser", tr("Notify about user"), true, "",
		checkNotify
	);

	ContactsListWidgetMenuManager::instance()->addManagementActionDescription(notifyAboutUserActionDescription);

	kdebugf2();
}

NotificationManager::~NotificationManager()
{
	kdebugf();

	ContactsListWidgetMenuManager::instance()->removeManagementActionDescription(notifyAboutUserActionDescription);
	delete notifyAboutUserActionDescription;

	StatusChangedNotification::unregisterEvents();
	ConnectionErrorNotification::unregisterEvent();
	MessageNotification::unregisterEvents();

	triggerAllAccountsUnregistered();

	if (!Notifiers.isEmpty())
	{
		kdebugm(KDEBUG_WARNING, "WARNING: not unregistered notifiers found! (%u)\n", Notifiers.size());

		while (!Notifiers.isEmpty())
			unregisterNotifier(Notifiers[0]);
	}

	kdebugf2();
}

void NotificationManager::notifyAboutUserActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	ContactList contacts = window->contacts();

	bool on = true;
	foreach (const Contact contact, contacts)
	{
		ContactNotifyData *cnd = contact.moduleData<ContactNotifyData>();

		if (!cnd || !cnd->notify())
		{
			on = false;
			break;
		}

		delete cnd;
	}

	foreach (const Contact contact, contacts)
	{
		if (contact.isNull() || contact.isAnonymous())
			continue;

		ContactNotifyData *cnd = contact.moduleData<ContactNotifyData>();
		if (!cnd)
			continue;

		if (cnd->notify() == on)
		{
			cnd->setNotify(!on);
			cnd->storeConfiguration();
			delete cnd;
		}
	}

	foreach(KaduAction *action, notifyAboutUserActionDescription->actions())
	{
		if (action->contacts() == contacts)
			action->setChecked(!on);
	}

	kdebugf2();
}

void NotificationManager::accountRegistered(Account *account)
{
	Protocol *protocol = account->protocol();
	connect(protocol, SIGNAL(connectionError(Account *, const QString &, const QString &)),
			this, SLOT(connectionError(Account *, const QString &, const QString &)));
	connect(protocol, SIGNAL(messageReceived(Account *, ContactList, const QString&, time_t)),
		this, SLOT(messageReceived(Account *, ContactList, const QString&, time_t)));
	connect(account, SIGNAL(contactStatusChanged(Account *, Contact, Status)),
		this, SLOT(statusChanged(Account *, Contact, Status)));
}

void NotificationManager::accountUnregistered(Account *account)
{
	Protocol *protocol = account->protocol();
	disconnect(protocol, SIGNAL(connectionError(Account *, const QString &, const QString &)),
			this, SLOT(connectionError(Account *, const QString &, const QString &)));
	disconnect(protocol, SIGNAL(messageReceived(Account *, ContactList, const QString&, time_t)),
		this, SLOT(messageReceived(Account *, ContactList, const QString&, time_t)));
	disconnect(account, SIGNAL(contactStatusChanged(Account *, Contact, Status)),
		this, SLOT(statusChanged(Account *, Contact, Status)));
}

void NotificationManager::statusChanged(Account *account, Contact contact, Status oldStatus)
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
	ContactNotifyData *cnd = contact.moduleData<ContactNotifyData>();

	if (!cnd || !cnd->notify())
		notify_contact = false;

	delete cnd;

	if (!notify_contact && !config_file.readBoolEntry("Notify", "NotifyAboutAll"))
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: not notifying user AND not notifying all users\n");
		return;
	}

	if (contact.id(account) == account->id())
		return;

	ContactAccountData *data = contact.accountData(account);
	if (!data)
		return;

	if (config_file.readBoolEntry("Notify", "IgnoreOnlineToOnline") &&
		(data->status().isOnline() || data->status().isBusy()) &&
			(oldStatus.isOnline() || oldStatus.isBusy()))
				return;

	QString changedTo = "To" + Status::name(data->status(), false);

	ContactList contacts(contact);

	StatusChangedNotification *statusChangedNotification = new StatusChangedNotification(changedTo, contacts, account);
	notify(statusChangedNotification);

	kdebugf2();
}

void NotificationManager::messageReceived(Account *account, ContactList contacts, const QString &msg, time_t t)
{
	kdebugf();

	ChatWidget *chat = chat_manager->findChatWidget(contacts);
	if (!chat) // new chat
		notify(new MessageNotification(MessageNotification::NewChat, contacts, msg, account));
	else // new message in chat
		if (!chat->edit()->hasFocus() || !config_file.readBoolEntry("Notify", "NewMessageOnlyIfInactive"))
			notify(new MessageNotification(MessageNotification::NewMessage, contacts, msg, account));

	kdebugf2();
}

void NotificationManager::connectionError(Account *account, const QString &server, const QString &message)
{
	kdebugf();

	if (!ConnectionErrorNotification::activeError(message))
	{
		ConnectionErrorNotification *connectionErrorNotification = new ConnectionErrorNotification(server, message, account);
		notify(connectionErrorNotification);
	}

	kdebugf2();
}

void NotificationManager::registerNotifyEvent(NotifyEvent *notifyEvent)
{
	kdebugf();

	NotifyEvents.append(notifyEvent);
	emit notifyEventRegisterd(notifyEvent);

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
					(CallbackSupported == notifier->callbackCapacity());
		}
	}

	if (!foundNotifierWithCallbackSupported)
		foreach (Notifier *notifier, Notifiers)
		{
			if (CallbackSupported == notifier->callbackCapacity())
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
		MessageBox::msg(tr("Unable to find notifier for %1 event").arg(notification->type()), true, "Warning");

	kdebugf2();
}

void NotificationManager::createDefaultConfiguration()
{
	config_file.addVariable("Notify", "IgnoreOnlineToOnline", false);
	config_file.addVariable("Notify", "NewMessageOnlyIfInactive", true);
	config_file.addVariable("Notify", "NotifyAboutAll", true);
	config_file.addVariable("Notify", "NotifyIgnoreOnConnection", true);
}

void checkNotify(KaduAction *action)
{
	kdebugf();

	if (config_file.readBoolEntry("Notify", "NotifyAboutAll"))
	{
		action->setEnabled(false);
		return;
	}

	foreach(Contact contact, action->contacts())
		if (!contact.hasAccountData(contact.prefferedAccount()))
		{
			action->setEnabled(false);
			return;
		}

	action->setEnabled(true);

	bool on = true;
	foreach (const Contact contact, action->contacts())
	{
		ContactNotifyData *cnd = contact.moduleData<ContactNotifyData>();

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

ConfigurationUiHandler * NotificationManager::configurationUiHandler()
{
	return UiHandler;
}
