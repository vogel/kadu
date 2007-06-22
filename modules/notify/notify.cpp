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

NotifyCheckBox::NotifyCheckBox(const QString &notificator, const QString &caption, QWidget *parent, char *name)
	: QCheckBox(caption, parent, name), Notificator(notificator)
{
	connect(this, SIGNAL(toggled(bool)), this, SLOT(toggledSlot(bool)));
}

void NotifyCheckBox::toggledSlot(bool toggle)
{
	emit toggled(Notificator, toggle);
}

Notify::Notify(QObject *parent, const char *name)
	: QObject(parent, name)
{
	kdebugf();

	connect(gadu, SIGNAL(connectionError(Protocol *, const QString &)), this, SLOT(connectionError(Protocol *, const QString &)));
	connect(gadu, SIGNAL(messageReceived(Protocol *, UserListElements, const QString&, time_t)),
			this, SLOT(messageReceived(Protocol *, UserListElements, const QString&, time_t)));
	connect(userlist, SIGNAL(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)),
		this, SLOT(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)));

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

	if (!Notifiers.isEmpty())
	{
		kdebugm(KDEBUG_WARNING, "WARNING: not unregistered notifiers found! (%u)\n", Notifiers.size());
		QValueList<QString> notifierNames = Notifiers.keys();
		CONST_FOREACH(name, notifierNames)
			unregisterNotifier(*name);
	}

	kdebugf2();
}

void Notify::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	notifications = dynamic_cast<ConfigComboBox *>(mainConfigurationWindow->widgetById("notify/notifications"));
	connect(notifications, SIGNAL(activated(int)), this, SLOT(eventSwitched(int)));

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

	ConfigGroupBox *groupBox = mainConfigurationWindow->configGroupBox("Notifications", "General", "Notifications");
	QGridLayout *layout = groupBox->layout();

	FOREACH(notifierData, Notifiers)
	{
		int numRows = layout->numRows();
		NotifyCheckBox *enable = new NotifyCheckBox(notifierData.key(), tr(notifierData.key()), groupBox->widget());
		connect(enable, SIGNAL(toggled(const QString &, bool)), this, SLOT(notifierToggled(const QString &, bool)));

		(*notifierData).configurationCheckBox = enable;
		layout->addWidget(enable, numRows, 0);

		NotifierConfigurationWidget *notifyConfigurationWidget = (*notifierData).notifier->createConfigurationWidget(groupBox->widget());
		if (!notifyConfigurationWidget)
			continue;

		connect(enable, SIGNAL(toggled(bool)), notifyConfigurationWidget, SLOT(setEnabled(bool)));
		(*notifierData).configurationWidget = notifyConfigurationWidget;
		layout->addWidget(notifyConfigurationWidget, numRows, 1);

		notifyConfigurationWidget->loadNotifyConfigurations();
	}

	eventSwitched(0);
}

void Notify::eventSwitched(int index)
{
	kdebugf();

	CurrentEvent = notifications->currentItemValue();
	FOREACH(notifierData, Notifiers)
	{
		if ((*notifierData).configurationWidget)
			(*notifierData).configurationWidget->switchToEvent(CurrentEvent);

		if (!(*notifierData).events.contains(CurrentEvent))
			(*notifierData).events[CurrentEvent] = config_file.readBoolEntry("Notify", CurrentEvent + '_' + notifierData.key());

		(*notifierData).configurationCheckBox->setChecked((*notifierData).events[CurrentEvent]);
	}
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

	FOREACH(notifierData, Notifiers)
	{
		if ((*notifierData).configurationWidget)
			(*notifierData).configurationWidget->saveNotifyConfigurations();

		FOREACH(event, ((*notifierData).events))
			config_file.writeEntry("Notify", event.key() + '_' + notifierData.key(), *event);
	}
}

void Notify::notifierToggled(const QString &notifier, bool toggled)
{
	kdebugf();

	Notifiers[notifier].events[CurrentEvent] = toggled;
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
	if (Notifiers.contains(name))
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

	Notifiers[name].notifier = notifier;
	Notifiers[name].configurationWidget = 0;
	Notifiers[name].configurationCheckBox = 0;

	kdebugf2();
}

void Notify::unregisterNotifier(const QString &name)
{
	kdebugf();
	if (!Notifiers.contains(name))
	{
		kdebugm(KDEBUG_WARNING, "WARNING: '%s' not registered!\n", name.local8Bit().data());
		return;
	}

	Notifiers.remove(name);
	kdebugf2();
}

QStringList Notify::notifiersList() const
{
	return QStringList(Notifiers.keys());
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

	CONST_FOREACH(i, Notifiers)
		if (config_file.readBoolEntry("Notify", notifyType + '_' + i.key()))
		{
			(*i).notifier->notify(notification);
			foundNotifier = true;
			foundNotifierWithCallbackSupported = foundNotifierWithCallbackSupported || ((*i).notifier->callbackCapacity() == CallbackSupported);
		}

	if (!foundNotifierWithCallbackSupported)
		CONST_FOREACH(i, Notifiers)
		{
			if ((*i).notifier->callbackCapacity() == CallbackSupported)
			{
				(*i).notifier->notify(notification);
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
