/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "chat.h"
#include "chat_manager.h"
// #include "config_dialog.h"
#include "config_file.h"
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

	kdebugf2();
	return 0;
}

extern "C" void notify_close()
{
	kdebugf();
	delete notification_manager;
	notification_manager = NULL;
	kdebugf2();
}

Notify::Notify(QObject *parent, const char *name) : QObject(parent, name),
	notifiers(), strs()
{
	kdebugf();

// 	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "Notify"), "NotifyTab");
// 	ConfigDialog::addHGroupBox("Notify", "Notify", QT_TRANSLATE_NOOP("@default", "Notify configuration"));

	//pierwsza kolumna - nazwy
// 	ConfigDialog::addVBox("Notify", "Notify configuration", "names");
// 	ConfigDialog::addLabel("Notify", "names", 0);

	connect(gadu, SIGNAL(connectionError(Protocol *, const QString &)), this, SLOT(connectionError(Protocol *, const QString &)));
	connect(gadu, SIGNAL(chatMsgReceived1(Protocol *, UserListElements, const QString&, time_t, bool&)),
			this, SLOT(probablyNewMessage(Protocol *, UserListElements, const QString&, time_t, bool&)));
	connect(gadu, SIGNAL(chatMsgReceived2(Protocol *, UserListElements, const QString&, time_t, bool)),
			this, SLOT(newChatSlot(Protocol *, UserListElements, const QString&, time_t, bool)));
	connect(userlist, SIGNAL(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)),
		this, SLOT(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)));

// 	ConfigDialog::addCheckBox("Notify", "Notify",
// 		QT_TRANSLATE_NOOP("@default", "Notify about all users"), "NotifyAboutAll", false);
// 	ConfigDialog::addCheckBox("Notify", "Notify",
// 			QT_TRANSLATE_NOOP("@default","Notify about new messages only when window is inactive"),
// 			"NewMessageOnlyIfInactive", true, 0, 0, Advanced);
// 	ConfigDialog::addCheckBox("Notify", "Notify",
// 		QT_TRANSLATE_NOOP("@default", "Ignore changes right after connection to the server"), "NotifyIgnoreOnConnection", true,
// 		QT_TRANSLATE_NOOP("@default","This option will supersede tooltips with users' status\n changes upon establishing connection to the server"),
// 		0, Advanced);
// 	ConfigDialog::addCheckBox("Notify", "Notify",
// 		QT_TRANSLATE_NOOP("@default", "Ignore status changes from available / busy to available / busy"),
// 		"IgnoreOnlineToOnline", true, 0, 0, Advanced);

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

// 	ConfigDialog::disconnectSlot("Notify", 0, SIGNAL(clicked()), notify_slots, SLOT(_Right()), "forward");
// 	ConfigDialog::disconnectSlot("Notify", 0, SIGNAL(clicked()), notify_slots, SLOT(_Left()), "back");
// 	ConfigDialog::disconnectSlot("Notify", "available", SIGNAL(doubleClicked(QListBoxItem *)),
// 		notify_slots, SLOT(_Right2(QListBoxItem *)));
// 	ConfigDialog::disconnectSlot("Notify", "track", SIGNAL(doubleClicked(QListBoxItem *)),
// 		notify_slots, SLOT(_Left2(QListBoxItem *)));

// 	ConfigDialog::unregisterSlotOnCreateTab("Notify", notify_slots, SLOT(onCreateTabNotify()));
// 	ConfigDialog::unregisterSlotOnApplyTab("Notify", notify_slots, SLOT(onApplyTabNotify()));

// 			ConfigDialog::removeControl("Notify", "track");
// 			ConfigDialog::removeControl("Notify", "Tracked");
// 		ConfigDialog::removeControl("Notify", "listbox3");
// 			ConfigDialog::removeControl("Notify", 0, "back");
// 			ConfigDialog::removeControl("Notify", 0, "forward");
// 		ConfigDialog::removeControl("Notify", "listbox2");
// 			ConfigDialog::removeControl("Notify", "available");
// 			ConfigDialog::removeControl("Notify", "Available");
// 		ConfigDialog::removeControl("Notify", "listbox1");
// 	ConfigDialog::removeControl("Notify", "listboxy");

// 	ConfigDialog::removeControl("Notify", "Ignore status changes from available / busy to available / busy");
// 	ConfigDialog::removeControl("Notify", "Notify about all users");
// 	ConfigDialog::removeControl("Notify", "Ignore changes right after connection to the server");

	disconnect(gadu, SIGNAL(connectionError(Protocol *, const QString &)), this, SLOT(connectionError(Protocol *, const QString &)));
	disconnect(gadu, SIGNAL(chatMsgReceived1(Protocol *, UserListElements, const QString&, time_t, bool&)),
			this, SLOT(probablyNewMessage(Protocol *, UserListElements, const QString&, time_t, bool&)));
	disconnect(gadu, SIGNAL(chatMsgReceived2(Protocol *, UserListElements, const QString&, time_t, bool)),
			this, SLOT(newChatSlot(Protocol *, UserListElements, const QString&, time_t, bool)));
	disconnect(userlist, SIGNAL(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)),
		this, SLOT(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)));

	if (!notifiers.isEmpty())
	{
		kdebugm(KDEBUG_WARNING, "WARNING: not unregistered notifiers found! (%u)\n", notifiers.size());
		QValueList<QString> notifierNames = notifiers.keys();
		CONST_FOREACH(name, notifierNames)
			unregisterNotifier(*name);
	}

	//pierwsza kolumna - nazwy
// 	ConfigDialog::removeControl("Notify", 0);

// 	ConfigDialog::removeControl("Notify", "names");
// 	ConfigDialog::removeControl("Notify", "Notify configuration");
// 	ConfigDialog::removeTab("Notify");

	delete notify_slots;
	notify_slots = NULL;

	kdebugf2();
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

void Notify::newChatSlot(Protocol *protocol, UserListElements senders, const QString &msg, time_t t, bool grabbed)
{
	kdebugf();

	if (!grabbed)
		notify(new MessageNotification(MessageNotification::NewChat, senders, msg));

	kdebugf2();
}

void Notify::probablyNewMessage(Protocol *protocol, UserListElements senders, const QString &msg, time_t t, bool &grab)
{
	kdebugf();

	bool alwaysNotify = !config_file.readBoolEntry("Notify", "NewMessageOnlyIfInactive");

	Chat *chat = chat_manager->findChat(senders);
	if (chat && (alwaysNotify || !chat->isActiveWindow()))
		notify(new MessageNotification(MessageNotification::NewMessage, senders, msg));

	kdebugf2();
}

void Notify::connectionError(Protocol *protocol, const QString &message)
{
	kdebugf();

	ConnectionErrorNotification *connectionErrorNotification = new ConnectionErrorNotification(message);
	notify(connectionErrorNotification);

	kdebugf2();
}

void Notify::addConfigColumn(const QString &name, CallbackCapacity callbackCapacity)
{
	kdebugf();
	QValueList<QCString> s;
	s.append(name.utf8()); // we've got to remember all those strings, because ConfigDialog won't copy them
	s.append(name.utf8() + "_vbox");
// 	ConfigDialog::addVBox("Notify", "Notify configuration", s[1]);
// 	ConfigDialog::addLabel("Notify", s[1], s[0]);

	CONST_FOREACH(it, NotifyEvents)
	{
		QCString entry = ((*it).name + '_' + name).utf8();
		QCString wname = (name + (*it).name).utf8();
// 		ConfigDialog::addCheckBox("Notify", s[1], " ", entry, false, 0, wname);
		s.append(entry);
		s.append(wname);
	}
	strs[name] = s;
	kdebugf2();
}

void Notify::removeConfigColumn(const QString &name)
{
	kdebugf();
	const QValueList<QCString> &s = strs[name];

	QValueListConstIterator<QCString> strit = s.constBegin();
	++strit; ++strit; // omit name and name_vbox
	CONST_FOREACH(it, NotifyEvents)
	{
		++strit; // omit entry
// 		ConfigDialog::removeControl("Notify", " ", *strit); // use wname
		++strit;
	}

// 	ConfigDialog::removeControl("Notify", s[0]); // name
// 	ConfigDialog::removeControl("Notify", s[1]); // name_vbox
	strs.remove(name);
	kdebugf2();
}

void Notify::addConfigRow(const QString &name, const char *description, CallbackRequirement callbackRequirement)
{
	kdebugf();
	NotifyEvent event;
	event.name = name;
	event.description = description;
	event.callbackRequirement = callbackRequirement;
	event.wname = ("label_" + name).utf8();

	NotifyEvents.append(event);

// 	ConfigDialog::addLabel("Notify", "names", event.description, event.wname);

	CONST_FOREACH(it, notifiers)
	{
		QValueList<QCString> &s = strs[it.key()];
		QCString parent = s[1]; // name_vbox
		QCString entry = (name + '_' + it.key()).utf8();
		QCString wname = (it.key() + name).utf8();
// 		ConfigDialog::addCheckBox("Notify", parent, " ", entry, false, 0, wname);
		s.append(entry);
		s.append(wname);
	}
	kdebugf2();
}

void Notify::removeConfigRow(const QString &name)
{
	kdebugf();
	NotifyEvent event;
	FOREACH(it, NotifyEvents)// look for matching event
		if ((*it).name == name)
		{
			event = *it;
			NotifyEvents.remove(it);
			break;
		}
	if (event.name.isEmpty())
	{
		kdebugmf(KDEBUG_FUNCTION_END | KDEBUG_ERROR, "event '%s' not found\n", name.local8Bit().data());
		return;
	}

// 	ConfigDialog::removeControl("Notify", event.description, event.wname);

	if (!notifiers.isEmpty())
	{
		// find index of correct wname, because we are going to use it
		QString notifierName = notifiers.constBegin().key();
		QValueList<QCString> &s = strs[notifierName];
		int wname_idx = s.findIndex((notifierName + name).utf8());
		if (wname_idx < 1) // 0 not allowed, look below (wname_idx - 1)
		{
			kdebugmf(KDEBUG_FUNCTION_END | KDEBUG_ERROR, "event name (%s) not found!? (%d)\n", name.local8Bit().data(), wname_idx);
			return;
		}

		CONST_FOREACH(it, notifiers)
		{
			QValueList<QCString> &s = strs[it.key()]; // strings for that notifier
// 			ConfigDialog::removeControl("Notify", " ", s[wname_idx]);
			QValueList<QCString>::iterator sit = s.at(wname_idx - 1), sit2 = sit; //iterators of entry, wname+1
			++sit2; ++sit2;
			s.erase(sit, sit2);
		}
	}
	kdebugf2();
}

void Notify::registerEvent(const QString &name, const char *description, CallbackRequirement callbackRequirement)
{
	kdebugf();
	addConfigRow(name, description, callbackRequirement);
	kdebugf2();
}

void Notify::unregisterEvent(const QString &name)
{
	kdebugf();
	removeConfigRow(name);
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
	addConfigColumn(name, notifier->callbackCapacity());

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

	removeConfigColumn(name);
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
		MessageBox::wrn(tr("Unable to find notifier for %1 event").arg(notification->type()), true);

	kdebugf2();
}

Notify *notification_manager = 0;
NotifySlots *notify_slots = 0;
