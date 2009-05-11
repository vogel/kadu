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
#include "contacts/contact-account-data.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/contacts-list-widget-menu-manager.h"
#include "gui/widgets/custom_input.h"
#include "notify/contact-notify-data.h"
#include "notify/notifier.h"
#include "notify/notify-configuration-ui-handler.h"
#include "notify/window-notifier.h"

#include "action.h"
#include "config_file.h"
#include "debug.h"
#include "kadu_main_window.h"
#include "main_configuration_window.h"
#include "message_box.h"
#include "misc/misc.h"

#include "new_message_notification.h"
#include "status_changed_notification.h"

NotificationManager *NotificationManager::Instance = 0;

NotificationManager * NotificationManager::instance()
{
	if (!Instance)
	{
		Instance = new NotificationManager();

		MessageNotification::registerEvents();
		StatusChangedNotification::registerEvents();
	}

	return Instance;
}

NotificationManager::NotificationManager()
{
	kdebugf();

	Instance = this; // TODO: 0.6.6, hack
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

	ContactsListWidgetMenuManager::instance()->addManagementActionDescription(notifyAboutUserActionDescription);

	new WindowNotifier(this);

	kdebugf2();
}

NotificationManager::~NotificationManager()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiHandler(UiHandler);

	ContactsListWidgetMenuManager::instance()->removeManagementActionDescription(notifyAboutUserActionDescription);
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

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	ContactSet contacts = window->contacts();

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
	connect(account, SIGNAL(contactStatusChanged(Account *, Contact, Status)),
			this, SLOT(statusChanged(Account *, Contact, Status)));

	ChatService *chatService = protocol->chatService();
	if (chatService)
	{
		connect(chatService, SIGNAL(messageReceived(Chat *, Contact, const QString &)),
				this, SLOT(messageReceived(Chat *, Contact, const QString &)));
	}
}

void NotificationManager::accountUnregistered(Account *account)
{
	Protocol *protocol = account->protocol();
	disconnect(protocol, SIGNAL(connectionError(Account *, const QString &, const QString &)),
			this, SLOT(connectionError(Account *, const QString &, const QString &)));
	disconnect(account, SIGNAL(contactStatusChanged(Account *, Contact, Status)),
			this, SLOT(statusChanged(Account *, Contact, Status)));

	ChatService *chatService = protocol->chatService();
	if (chatService)
	{
		disconnect(chatService, SIGNAL(messageReceived(Chat *, Contact, const QString &)),
				this, SLOT(messageReceived(Chat *, Contact, const QString &)));
	}
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

	if (cnd)
		delete cnd;

	if (!notify_contact && !config_file.readBoolEntry("Notify", "NotifyAboutAll"))
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: not notifying user AND not notifying all users\n");
		return;
	}

	if (contact.id(account) == account->id()) // myself
		return;

	ContactAccountData *data = contact.accountData(account);
	if (!data)
		return;

	if (oldStatus == data->status())
		return;

	if (config_file.readBoolEntry("Notify", "IgnoreOnlineToOnline") &&
		(data->status().isOnline() || data->status().isBusy()) &&
			(oldStatus.isOnline() || oldStatus.isBusy()))
				return;

	QString changedTo = "To" + Status::name(data->status(), false);

	ContactSet contacts(contact);

	StatusChangedNotification *statusChangedNotification = new StatusChangedNotification(changedTo, contacts, account);
	notify(statusChangedNotification);

	kdebugf2();
}

void NotificationManager::messageReceived(Chat *chat, Contact sender, const QString &message)
{
	kdebugf();

	ChatWidget *chatWidget = ChatWidgetManager::instance()->byChat(chat);
	if (!chatWidget) // new chat
		notify(new MessageNotification(MessageNotification::NewChat, chat, message));
	else // new message in chat
		if (!chatWidget->edit()->hasFocus() || !config_file.readBoolEntry("Notify", "NewMessageOnlyIfInactive"))
			notify(new MessageNotification(MessageNotification::NewMessage, chat, message));

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

ConfigurationUiHandler * NotificationManager::configurationUiHandler()
{
	return UiHandler;
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
