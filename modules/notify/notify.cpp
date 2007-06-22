/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "chat_widget.h"
#include "chat_manager.h"
#include "config_file.h"
#include "configuration_window_widgets.h"
#include "connection_error_notification.h"
#include "debug.h"
#include "kadu.h"
#include "misc.h"
#include "new_message_notification.h"
#include "notify.h"
#include "notify_slots.h"
#include "message_box.h"
#include "status_changed_notification.h"
#include "userbox.h"

extern "C" int notify_init()
{
	kdebugf();

	notification_manager = new Notify(NULL, "notify");
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/notify.ui"), notification_manager);

	kdebugf2();
	return 0;
}

extern "C" void notify_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/notify.ui"), notification_manager);
	delete notification_manager;
	notification_manager = 0;

	kdebugf2();
}

Notify::Notify(QObject *parent, const char *name) : QObject(parent, name),
	notifiers()
{
	kdebugf();

	connect(gadu, SIGNAL(connectionError(Protocol *, const QString &)), this, SLOT(connectionError(Protocol *, const QString &)));
	connect(gadu, SIGNAL(messageReceived(Protocol *, UserListElements, const QString&, time_t)),
			this, SLOT(messageReceived(Protocol *, UserListElements, const QString&, time_t)));
	connect(userlist, SIGNAL(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)),
		this, SLOT(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)));

// 	ConfigDialog::addGrid("Notify", "Notify" ,"listboxy",3);

// 		ConfigDialog::addGrid("Notify", "listboxy", "listbox1", 1);
// 			ConfigDialog::addLabel("Notify", "listbox1", QT_TRANSLATE_NOOP("@default", "Available"));
// 			ConfigDialog::addListBox("Notify", "listbox1","available");

// 		ConfigDialog::addGrid("Notify", "listboxy", "listbox2", 1);
// 			ConfigDialog::addPushButton("Notify", "listbox2", 0, "AddToNotifyList", 0, "forward");
// 			ConfigDialog::addPushButton("Notify", "listbox2", 0, "RemoveFromNotifyList", 0, "back");

// 		ConfigDialog::addGrid("Notify", "listboxy", "listbox3", 1);
// 			ConfigDialog::addLabel("Notify", "listbox3", QT_TRANSLATE_NOOP("@default", "Tracked"));
// 			ConfigDialog::addListBox("Notify", "listbox3", "track");

	notify_slots = new NotifySlots();

// 	ConfigDialog::connectSlot("Notify", 0, SIGNAL(clicked()), notify_slots, SLOT(_Right()), "forward");
// 	ConfigDialog::connectSlot("Notify", 0, SIGNAL(clicked()), notify_slots, SLOT(_Left()), "back");
// 	ConfigDialog::connectSlot("Notify", "available", SIGNAL(doubleClicked(QListBoxItem *)),
// 		notify_slots, SLOT(_Right2(QListBoxItem *)));
// 	ConfigDialog::connectSlot("Notify", "track", SIGNAL(doubleClicked(QListBoxItem *)),
// 		notify_slots, SLOT(_Left2(QListBoxItem *)));

// 	ConfigDialog::registerSlotOnCreateTab("Notify", notify_slots, SLOT(onCreateTabNotify()));
// 	ConfigDialog::registerSlotOnApplyTab("Notify", notify_slots, SLOT(onApplyTabNotify()));

	MessageNotification::registerEvents(this);
	ConnectionErrorNotification::registerEvent(this);
	StatusChangedNotification::registerEvents(this);

	kdebugf2();
}

Notify::~Notify()
{
	kdebugf();

	StatusChangedNotification::unregisterEvents(this);
	ConnectionErrorNotification::unregisterEvent(this);
	MessageNotification::unregisterEvents(this);

	disconnect(gadu, SIGNAL(connectionError(Protocol *, const QString &)), this, SLOT(connectionError(Protocol *, const QString &)));
	disconnect(gadu, SIGNAL(messageReceived(Protocol *, UserListElements, const QString&, time_t)),
			this, SLOT(messageReceived(Protocol *, UserListElements, const QString&, time_t)));
	disconnect(userlist, SIGNAL(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)),
		this, SLOT(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)));

	if (!notifiers.isEmpty())
	{
		kdebugm(KDEBUG_WARNING, "WARNING: not unregistered notifiers found! (%u)\n", notifiers.size());
		QValueList<QString> notifierNames = notifiers.keys();
		CONST_FOREACH(name, notifierNames)
			unregisterNotifier(*name);
	}

	delete notify_slots;
	notify_slots = NULL;

	kdebugf2();
}

void Notify::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	ConfigComboBox *notifications = dynamic_cast<ConfigComboBox *>(mainConfigurationWindow->widgetById("notify/notifications"));

	QStringList captions;
	QStringList values;

	CONST_FOREACH(notifyEvent, NotifyEvents)
	{
		captions.append((*notifyEvent).description);
		values.append((*notifyEvent).name);
	}

	notifications->setItems(values, captions);

	allUsers = dynamic_cast<QListBox *>(mainConfigurationWindow->widgetById("notify/userList"));
	notifiedUsers = dynamic_cast<QListBox *>(mainConfigurationWindow->widgetById("notify/notifyList"));

	CONST_FOREACH(user, *userlist)
		if ((*user).usesProtocol("Gadu") && !(*user).isAnonymous())
			if (!(*user).notify())
				allUsers->insertItem((*user).altNick());
			else
				notifiedUsers->insertItem((*user).altNick());

	allUsers->sort();
	notifiedUsers->sort();
	allUsers->setSelectionMode(QListBox::Extended);
	notifiedUsers->setSelectionMode(QListBox::Extended);

	connect(mainConfigurationWindow->widgetById("notify/up"), SIGNAL(clicked()), this, SLOT(moveUp()));
	connect(mainConfigurationWindow->widgetById("notify/down"), SIGNAL(clicked()), this, SLOT(moveDown()));
	connect(notifiedUsers, SIGNAL(doubleClicked(QListBoxItem *)), this, SLOT(moveUp()));
	connect(allUsers, SIGNAL(doubleClicked(QListBoxItem *)), this, SLOT(moveDown()));

	QWidget *notifyAll = mainConfigurationWindow->widgetById("notify/notifyAll");
	connect(notifyAll, SIGNAL(toggled(bool)), allUsers, SLOT(setDisabled(bool)));
	connect(notifyAll, SIGNAL(toggled(bool)), notifiedUsers, SLOT(setDisabled(bool)));
	connect(notifyAll, SIGNAL(toggled(bool)), mainConfigurationWindow->widgetById("notify/up"), SLOT(setDisabled(bool)));
	connect(notifyAll, SIGNAL(toggled(bool)), mainConfigurationWindow->widgetById("notify/down"), SLOT(setDisabled(bool)));

	connect(mainConfigurationWindow, SIGNAL(configurationWindowApplied()), this, SLOT(configurationWindowApplied()));
}

void Notify::configurationWindowApplied()
{
	int count = notifiedUsers->count();
	for (int i = 0; i < count; ++i)
		userlist->byAltNick(notifiedUsers->text(i)).setNotify(true);

	count = allUsers->count();
	for (int i = 0; i < count; ++i)
		userlist->byAltNick(allUsers->text(i)).setNotify(false);

	userlist->writeToConfig();
}

void Notify::moveUp()
{
	int count = notifiedUsers->count();

	for (int i = count - 1; i >= 0; i--)
		if (notifiedUsers->isSelected(i))
		{
			allUsers->insertItem(notifiedUsers->text(i));
			notifiedUsers->removeItem(i);
		}

	notifiedUsers->sort();
}

void Notify::moveDown()
{
	int count = allUsers->count();

	for (int i = count - 1; i >= 0; i--)
		if (allUsers->isSelected(i))
		{
			notifiedUsers->insertItem(allUsers->text(i));
			allUsers->removeItem(i);
		}

	allUsers->sort();
}

void Notify::statusChanged(UserListElement elem, QString protocolName,
					const UserStatus &oldStatus, bool massively, bool /*last*/)
{
	kdebugf();

	if (massively && config_file.readBoolEntry("Notify", "NotifyIgnoreOnConnection"))
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: ignore on connection\n");
		return;
	}

	if (!elem.notify() && !config_file.readBoolEntry("Notify", "NotifyAboutAll"))
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: not notifying user AND not notifying all users\n");
		return;
	}

	if (elem.ID("Gadu") == config_file.readEntry("General", "UIN") &&
	    config_file.readBoolEntry("Notify", "NotifyAboutAll"))
		return;

	if (config_file.readBoolEntry("Notify", "IgnoreOnlineToOnline"))
		if (elem.status("Gadu").isOnline() || elem.status("Gadu").isBusy())
			if (oldStatus.isOnline() || oldStatus.isBusy())
				return;

	QString changedTo = "";
	switch (elem.status("Gadu").status())
	{
		case Online:	changedTo = "ToOnline"; break;
		case Busy:		changedTo = "ToBusy"; break;
		case Invisible:	changedTo = "ToInvisible"; break;
		case Offline:	changedTo = "ToOffline"; break;
		default:
			return;
	}

	UserListElements elems;
	elems.append(elem);

	StatusChangedNotification *statusChangedNotification = new StatusChangedNotification(changedTo, elems);
	notify(statusChangedNotification);

	kdebugf2();
}

void Notify::messageReceived(Protocol *protocol, UserListElements senders, const QString &msg, time_t t)
{
	kdebugf();

	ChatWidget *chat = chat_manager->findChatWidget(senders);
	if (!chat) // new chat
		notify(new MessageNotification(MessageNotification::NewChat, senders, msg));
	else // new message in chat
	{
		bool alwaysNotify = !config_file.readBoolEntry("Notify", "NewMessageOnlyIfInactive");
		if (alwaysNotify || !chat->isActiveWindow())
			notify(new MessageNotification(MessageNotification::NewMessage, senders, msg));
	}

	kdebugf2();
}

void Notify::connectionError(Protocol *protocol, const QString &message)
{
	kdebugf();

	ConnectionErrorNotification *connectionErrorNotification = new ConnectionErrorNotification(message);
	notify(connectionErrorNotification);

	kdebugf2();
}

void Notify::registerEvent(const QString &name, const char *description, CallbackRequirement callbackRequirement)
{
	kdebugf();

	NotifyEvent event;
	event.name = name;
	event.description = description;
	event.callbackRequirement = callbackRequirement;

	NotifyEvents.append(event);

	kdebugf2();
}

void Notify::unregisterEvent(const QString &name)
{
	kdebugf();

	FOREACH(it, NotifyEvents)// look for matching event
		if ((*it).name == name)
		{
			NotifyEvents.remove(it);
			break;
		}

	kdebugf2();
}

void Notify::import_connection_from_0_5_0(const QString &notifierName, const QString &oldConnectionName, const QString &newConnectionName)
{
	if (config_file.readBoolEntry("Notify", oldConnectionName + "_" + notifierName, false))
	{
		config_file.writeEntry("Notify", newConnectionName + "_" + notifierName, true);
		config_file.removeVariable("Notify", oldConnectionName + "_" + notifierName);
	}
}

void Notify::registerNotifier(const QString &name, Notifier *notifier)
{
	kdebugf();
	if (notifiers.contains(name))
	{
		kdebugm(KDEBUG_WARNING, "WARNING: '%s' already exists in notifiers! "
		"strange... unregistering old Notifier\n", name.local8Bit().data());

		unregisterNotifier(name);
	}
	// TODO: remove after 0.6 release
	if (config_file.readBoolEntry("Notify", "StatusChanged_" + name, false))
	{
		QStringList addToMe;
		addToMe << "toAvailable" << "toBusy" << "toInvisible" << "toNotAvailable";

		CONST_FOREACH(i, addToMe)
		{
			if (!config_file.readBoolEntry("Notify", *i + '_' + name))
			{
				notifier->copyConfiguration("StatusChanged", *i);
				config_file.writeEntry("Notify", *i + '_' + name, true);
			}
		}

		config_file.removeVariable("Notify", "StatusChanged_" + name);
	}

	import_connection_from_0_5_0(name, "ConnError", "ConnectionError");
	import_connection_from_0_5_0(name, "toAvailable", "StatusChanged/ToOnline");
	import_connection_from_0_5_0(name, "toBusy", "StatusChanged/ToBusy");
	import_connection_from_0_5_0(name, "toInvisible", "StatusChanged/ToInvisible");
	import_connection_from_0_5_0(name, "toOffline", "StatusChanged/ToOffline");

	notifiers[name] = notifier;

	kdebugf2();
}

void Notify::unregisterNotifier(const QString &name)
{
	kdebugf();
	if (!notifiers.contains(name))
	{
		kdebugm(KDEBUG_WARNING, "WARNING: '%s' not registered!\n", name.local8Bit().data());
		return;
	}

	notifiers.remove(name);
	kdebugf2();
}

QStringList Notify::notifiersList() const
{
	return QStringList(notifiers.keys());
}

const QValueList<Notify::NotifyEvent> &Notify::notifyEvents()
{
	return NotifyEvents;
}

void Notify::notify(Notification *notification)
{
	kdebugf();
	QString notifyType = notification->type();
	bool foundNotifier = false;
	bool foundNotifierWithCallbackSupported = notification->getCallbacks().count() == 0;

	notification->acquire();

	CONST_FOREACH(i, notifiers)
		if (config_file.readBoolEntry("Notify", notifyType + '_' + i.key()))
		{
			(*i)->notify(notification);
			foundNotifier = true;
			foundNotifierWithCallbackSupported = foundNotifierWithCallbackSupported || ((*i)->callbackCapacity() == CallbackSupported);
		}

	if (!foundNotifierWithCallbackSupported)
		CONST_FOREACH(i, notifiers)
		{
			if ((*i)->callbackCapacity() == CallbackSupported)
			{
				(*i)->notify(notification);
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

Notify *notification_manager = 0;
NotifySlots *notify_slots = 0;
