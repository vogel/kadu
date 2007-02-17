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
#include "config_dialog.h"
#include "config_file.h"
#include "debug.h"
#include "kadu.h"
#include "misc.h"
#include "notify.h"
#include "notify_slots.h"
#include "message_box.h"
#include "userbox.h"

extern "C" int notify_init()
{
	kdebugf();
	notify = new Notify(NULL, "notify");

	kdebugf2();
	return 0;
}

extern "C" void notify_close()
{
	kdebugf();
	delete notify;
	notify = NULL;
	kdebugf2();
}

Notify::Notify(QObject *parent, const char *name) : QObject(parent, name),
	notifiers(), notifySignals(), eventNames(), notifyEvents(), strs()
{
	kdebugf();
	eventNames<<"ConnError"<<"NewChat"<<"NewMessage"<<"StatusChanged"<<"toAvailable"<<
				"toBusy"<<"toInvisible"<<"toNotAvailable"<<"UserBoxChangeToolTip"<<"Message";

	notifySignals["NewChat"]=			QString(SIGNAL(newChat(Protocol *, UserListElements, const QString &, time_t)));
	notifySignals["NewMessage"]=		QString(SIGNAL(newMessage(Protocol *, UserListElements, const QString &, time_t, bool &)));
	notifySignals["ConnError"]=			QString(SIGNAL(connectionError(Protocol *, const QString &)));
	notifySignals["StatusChanged"]=		QString(SIGNAL(userStatusChanged(UserListElement, QString, const UserStatus &)));
	notifySignals["toAvailable"]=		QString(SIGNAL(userChangedStatusToAvailable(const QString &, UserListElement)));
	notifySignals["toBusy"]=			QString(SIGNAL(userChangedStatusToBusy(const QString &, UserListElement)));
	notifySignals["toInvisible"]=		QString(SIGNAL(userChangedStatusToInvisible(const QString &, UserListElement)));
	notifySignals["toNotAvailable"]=	QString(SIGNAL(userChangedStatusToNotAvailable(const QString &, UserListElement)));
	notifySignals["UserBoxChangeToolTip"]=QString(SIGNAL(userBoxChangeToolTip(const QPoint &, UserListElement, bool)));
	notifySignals["Message"]=			QString(SIGNAL(message(const QString &, const QString &, const QMap<QString, QVariant> *, const UserListElement *)));

	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "Notify"), "NotifyTab");
	ConfigDialog::addHGroupBox("Notify", "Notify", QT_TRANSLATE_NOOP("@default", "Notify configuration"));

	//pierwsza kolumna - nazwy
	ConfigDialog::addVBox("Notify", "Notify configuration", "names");
	ConfigDialog::addLabel("Notify", "names", 0);
	ConfigDialog::addLabel("Notify", "names", QT_TRANSLATE_NOOP("@default", "Connection error"));
	ConfigDialog::addLabel("Notify", "names", QT_TRANSLATE_NOOP("@default", "New chat"));
	ConfigDialog::addLabel("Notify", "names", QT_TRANSLATE_NOOP("@default", "New message"));
	ConfigDialog::addLabel("Notify", "names", QT_TRANSLATE_NOOP("@default", "User is changing status"));
	ConfigDialog::addLabel("Notify", "names", QT_TRANSLATE_NOOP("@default", "User changed status to \"Available\""));
	ConfigDialog::addLabel("Notify", "names", QT_TRANSLATE_NOOP("@default", "User changed status to \"Busy\""));
	ConfigDialog::addLabel("Notify", "names", QT_TRANSLATE_NOOP("@default", "User changed status to \"Invisible\""));
	ConfigDialog::addLabel("Notify", "names", QT_TRANSLATE_NOOP("@default", "User changed status to \"Not available\""));
	ConfigDialog::addLabel("Notify", "names", QT_TRANSLATE_NOOP("@default", "Mouse over user"));
	ConfigDialog::addLabel("Notify", "names", QT_TRANSLATE_NOOP("@default", "Other message"));

	connect(gadu, SIGNAL(connectionError(Protocol *, const QString &)), this, notifySignals["ConnError"].ascii());
	connect(gadu, SIGNAL(chatMsgReceived1(Protocol *, UserListElements, const QString&, time_t, bool&)),
			this, SLOT(probablyNewMessage(Protocol *, UserListElements, const QString&, time_t, bool&)));
	connect(gadu, SIGNAL(chatMsgReceived2(Protocol *, UserListElements, const QString&, time_t, bool)),
			this, SLOT(newChatSlot(Protocol *, UserListElements, const QString&, time_t, bool)));
	connect(userlist, SIGNAL(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)),
		this, SLOT(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)));
	connect(kadu->userbox(), SIGNAL(changeToolTip(const QPoint &, UserListElement, bool)),
			this, SIGNAL(userBoxChangeToolTip(const QPoint &, UserListElement, bool)));

	notify_slots=new NotifySlots(NULL, "notify_slots");

	ConfigDialog::addCheckBox("Notify", "Notify",
		QT_TRANSLATE_NOOP("@default", "Notify about all users"), "NotifyAboutAll", false);
	ConfigDialog::addCheckBox("Notify", "Notify",
		QT_TRANSLATE_NOOP("@default", "Ignore changes right after connection to the server"), "NotifyIgnoreOnConnection", true,
		QT_TRANSLATE_NOOP("@default","This option will supersede tooltips with users' status\n changes upon establishing connection to the server"),
		0, Advanced);
	ConfigDialog::addCheckBox("Notify", "Notify",
		QT_TRANSLATE_NOOP("@default", "Ignore status changes from available / busy to available / busy"),
		"IgnoreOnlineToOnline", true, 0, 0, Advanced);

	ConfigDialog::addGrid("Notify", "Notify" ,"listboxy",3);

		ConfigDialog::addGrid("Notify", "listboxy", "listbox1", 1);
			ConfigDialog::addLabel("Notify", "listbox1", QT_TRANSLATE_NOOP("@default", "Available"));
			ConfigDialog::addListBox("Notify", "listbox1","available");

		ConfigDialog::addGrid("Notify", "listboxy", "listbox2", 1);
			ConfigDialog::addPushButton("Notify", "listbox2", 0, "AddToNotifyList", 0, "forward");
			ConfigDialog::addPushButton("Notify", "listbox2", 0, "RemoveFromNotifyList", 0, "back");

		ConfigDialog::addGrid("Notify", "listboxy", "listbox3", 1);
			ConfigDialog::addLabel("Notify", "listbox3", QT_TRANSLATE_NOOP("@default", "Tracked"));
			ConfigDialog::addListBox("Notify", "listbox3", "track");

	ConfigDialog::connectSlot("Notify", 0, SIGNAL(clicked()), notify_slots, SLOT(_Right()), "forward");
	ConfigDialog::connectSlot("Notify", 0, SIGNAL(clicked()), notify_slots, SLOT(_Left()), "back");
	ConfigDialog::connectSlot("Notify", "available", SIGNAL(doubleClicked(QListBoxItem *)),
		notify_slots, SLOT(_Right2(QListBoxItem *)));
	ConfigDialog::connectSlot("Notify", "track", SIGNAL(doubleClicked(QListBoxItem *)),
		notify_slots, SLOT(_Left2(QListBoxItem *)));

	ConfigDialog::registerSlotOnCreateTab("Notify", notify_slots, SLOT(onCreateTabNotify()));
	ConfigDialog::registerSlotOnApplyTab("Notify", notify_slots, SLOT(onApplyTabNotify()));
	ConfigDialog::registerSlotOnApplyTab("Notify", this, SLOT(updateConnections()));

	kdebugf2();
}

Notify::~Notify()
{
	kdebugf();

	ConfigDialog::disconnectSlot("Notify", 0, SIGNAL(clicked()), notify_slots, SLOT(_Right()), "forward");
	ConfigDialog::disconnectSlot("Notify", 0, SIGNAL(clicked()), notify_slots, SLOT(_Left()), "back");
	ConfigDialog::disconnectSlot("Notify", "available", SIGNAL(doubleClicked(QListBoxItem *)),
		notify_slots, SLOT(_Right2(QListBoxItem *)));
	ConfigDialog::disconnectSlot("Notify", "track", SIGNAL(doubleClicked(QListBoxItem *)),
		notify_slots, SLOT(_Left2(QListBoxItem *)));

	ConfigDialog::unregisterSlotOnCreateTab("Notify", notify_slots, SLOT(onCreateTabNotify()));
	ConfigDialog::unregisterSlotOnApplyTab("Notify", notify_slots, SLOT(onApplyTabNotify()));
	ConfigDialog::unregisterSlotOnApplyTab("Notify", this, SLOT(updateConnections()));

			ConfigDialog::removeControl("Notify", "track");
			ConfigDialog::removeControl("Notify", "Tracked");
		ConfigDialog::removeControl("Notify", "listbox3");
			ConfigDialog::removeControl("Notify", 0, "back");
			ConfigDialog::removeControl("Notify", 0, "forward");
		ConfigDialog::removeControl("Notify", "listbox2");
			ConfigDialog::removeControl("Notify", "available");
			ConfigDialog::removeControl("Notify", "Available");
		ConfigDialog::removeControl("Notify", "listbox1");
	ConfigDialog::removeControl("Notify", "listboxy");

	ConfigDialog::removeControl("Notify", "Ignore status changes from available / busy to available / busy");
	ConfigDialog::removeControl("Notify", "Notify about all users");
	ConfigDialog::removeControl("Notify", "Ignore changes right after connection to the server");

	disconnect(gadu, SIGNAL(connectionError(Protocol *, const QString &)), this, notifySignals["ConnError"].ascii());
	disconnect(gadu, SIGNAL(chatMsgReceived1(Protocol *, UserListElements, const QString&, time_t, bool&)),
			this, SLOT(probablyNewMessage(Protocol *, UserListElements, const QString&, time_t, bool&)));
	disconnect(gadu, SIGNAL(chatMsgReceived2(Protocol *, UserListElements, const QString&, time_t, bool)),
			this, SLOT(newChatSlot(Protocol *, UserListElements, const QString&, time_t, bool)));
	disconnect(userlist, SIGNAL(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)),
		this, SLOT(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)));
	disconnect(kadu->userbox(), SIGNAL(changeToolTip(const QPoint &, UserListElement, bool)),
			this, SIGNAL(userBoxChangeToolTip(const QPoint &, UserListElement, bool)));

	if (!notifiers.isEmpty())
	{
		kdebugm(KDEBUG_WARNING, "WARNING: not unregistered notifiers found! (%lu)\n", notifiers.size());
		QValueList<QString> notifierNames = notifiers.keys();
		CONST_FOREACH(name, notifierNames)
			unregisterNotifier(*name);
	}

	//pierwsza kolumna - nazwy
	ConfigDialog::removeControl("Notify", 0);
	ConfigDialog::removeControl("Notify", "Connection error");
	ConfigDialog::removeControl("Notify", "New chat");
	ConfigDialog::removeControl("Notify", "New message");
	ConfigDialog::removeControl("Notify", "User is changing status");
	ConfigDialog::removeControl("Notify", "User changed status to \"Available\"");
	ConfigDialog::removeControl("Notify", "User changed status to \"Busy\"");
	ConfigDialog::removeControl("Notify", "User changed status to \"Invisible\"");
	ConfigDialog::removeControl("Notify", "User changed status to \"Not available\"");
	ConfigDialog::removeControl("Notify", "Mouse over user");
	ConfigDialog::removeControl("Notify", "Other message");

	ConfigDialog::removeControl("Notify", "names");
	ConfigDialog::removeControl("Notify", "Notify configuration");
	ConfigDialog::removeTab("Notify");

	notifySignals.clear();

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

	emit userStatusChanged(elem, protocolName, oldStatus);

	switch (elem.status("Gadu").status())
	{
		case Online:	emit userChangedStatusToAvailable(protocolName, elem); break;
		case Busy:		emit userChangedStatusToBusy(protocolName, elem); break;
		case Invisible:	emit userChangedStatusToInvisible(protocolName, elem); break;
		case Offline:	emit userChangedStatusToNotAvailable(protocolName, elem); break;
		default:
			;//jeszcze jest status "blokowany", który nie jest tu obs³ugiwany
	}

	kdebugf2();
}

void Notify::newChatSlot(Protocol *protocol, UserListElements senders, const QString &msg, time_t t, bool grabbed)
{
	if (!grabbed)
	{
		kdebugf();//sygnatury trochê siê ró¿ni±, wiêc ten slot musi byæ...
		emit newChat(protocol, senders, msg, t);
		kdebugf2();
	}
}

void Notify::probablyNewMessage(Protocol *protocol, UserListElements senders, const QString &msg, time_t t, bool &grab)
{
	kdebugf();
	Chat* chat = chat_manager->findChat(senders);
	if (chat != NULL)
		emit newMessage(protocol, senders, msg, t, grab);
	kdebugf2();
}

void Notify::addConfigColumn(const QString &name, const QMap<QString, QString> &notifierSlots, CallbackCapacity callbackCapacity)
{
	kdebugf();
	QValueList<QCString> s;
	s.append(name.utf8()); // we've got to remember all those strings, because ConfigDialog won't copy them
	s.append(name.utf8() + "_vbox");
	ConfigDialog::addVBox("Notify", "Notify configuration", s[1]);
	ConfigDialog::addLabel("Notify", s[1], s[0]);

	CONST_FOREACH(it, eventNames)
	{
		QCString entry = ((*it) + '_' + name).utf8();
		QCString wname = (name + (*it)).utf8();
		ConfigDialog::addCheckBox("Notify", s[1], " ", entry, false, 0, wname);
		if (!notifierSlots.contains(*it))
			notify_slots->registerDisabledControl(wname);
		s.append(entry);
		s.append(wname);
	}
	CONST_FOREACH(it, notifyEvents)
	{
		QCString entry = ((*it).name + '_' + name).utf8();
		QCString wname = (name + (*it).name).utf8();
		ConfigDialog::addCheckBox("Notify", s[1], " ", entry, false, 0, wname);
		if ((callbackCapacity == CallbackNotSupported) && ((*it).callbackRequirement == CallbackRequired))
			notify_slots->registerDisabledControl(wname);
		s.append(entry);
		s.append(wname);
	}
	strs[name] = s;
	kdebugf2();
}

void Notify::removeConfigColumn(const QString &name, const QMap<QString, QPair<QString, bool> > &notifierSlots)
{
	kdebugf();
	const QValueList<QCString> &s = strs[name];

	QValueListConstIterator<QCString> strit = s.constBegin();
	++strit; ++strit; // omit name and name_vbox
	CONST_FOREACH(it, eventNames)
	{
		++strit; // omit entry
		ConfigDialog::removeControl("Notify", " ", *strit); // use wname
		if (!notifierSlots.contains(*it))
			notify_slots->unregisterDisabledControl(*strit);
		++strit;
	}
	CONST_FOREACH(it, notifyEvents)
	{
		++strit; // omit entry
		ConfigDialog::removeControl("Notify", " ", *strit); // use wname
		++strit;
	}

	ConfigDialog::removeControl("Notify", s[0]); // name
	ConfigDialog::removeControl("Notify", s[1]); // name_vbox
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

	notifyEvents.append(event);

	ConfigDialog::addLabel("Notify", "names", event.description, event.wname);

	CONST_FOREACH(it, notifiers)
	{
		QValueList<QCString> &s = strs[it.key()];
		QCString parent = s[1]; // name_vbox
		QCString entry = (name + '_' + it.key()).utf8();
		QCString wname = (it.key() + name).utf8();
		ConfigDialog::addCheckBox("Notify", parent, " ", entry, false, 0, wname);
		if ((callbackRequirement == CallbackRequired) && ((*it).notifier->callbackCapacity() == CallbackNotSupported))
			notify_slots->registerDisabledControl(wname);
		s.append(entry);
		s.append(wname);
	}
	kdebugf2();
}

void Notify::removeConfigRow(const QString &name)
{
	kdebugf();
	NotifyEvent event;
	FOREACH(it, notifyEvents)// look for matching event
		if ((*it).name == name)
		{
			event = *it;
			notifyEvents.remove(it);
			break;
		}
	if (event.name.isEmpty())
	{
		kdebugmf(KDEBUG_FUNCTION_END | KDEBUG_ERROR, "event '%s' not found\n", name.local8Bit().data());
		return;
	}

	ConfigDialog::removeControl("Notify", event.description, event.wname);

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
			ConfigDialog::removeControl("Notify", " ", s[wname_idx]);
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

void Notify::updateConnections()
{
	kdebugf();

	FOREACH(i, notifiers)
	{
		QString notifierName = i.key();
		NotifierSlots &notifier = i.data();
		FOREACH(j, notifier.notifierSlots)
		{
			QString signalName = j.key();
			QPair<QString, bool> &connection = j.data();
			if (config_file.readBoolEntry("Notify", signalName + '_' + notifierName) != connection.second)
			{
				if (connection.second)
					disconnect(this, notifySignals[signalName].ascii(), notifier.notifier, connection.first.ascii());
				else
					connect(this, notifySignals[signalName].ascii(), notifier.notifier, connection.first.ascii());
				connection.second = !connection.second;
			}
		}
	}

	kdebugf2();
}

void Notify::registerNotifier(const QString &name, Notifier *notifier,
							const QMap<QString, QString> &notifierSlots)
{
	kdebugf();
	if (notifiers.contains(name))
	{
		kdebugm(KDEBUG_WARNING, "WARNING: '%s' already exists in notifiers! "
		"strange... unregistering old Notifier\n", name.local8Bit().data());

		unregisterNotifier(name);
	}
	notifiers[name] = NotifierSlots(notifier, notifierSlots);

	CONST_FOREACH(i, notifySignals)
		if (config_file.readBoolEntry("Notify", i.key() + '_' + name) && notifierSlots.contains(i.key()))
			connectSlot(name, i.key());
	addConfigColumn(name, notifierSlots, notifier->callbackCapacity());
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
	NotifierSlots notifier = notifiers[name];
	removeConfigColumn(name, notifier.notifierSlots);

	CONST_FOREACH(i, notifySignals)
		if (config_file.readBoolEntry("Notify", i.key() + '_' + name) && notifier.notifierSlots.contains(i.key()))
			disconnectSlot(name, i.key());
	notifiers.remove(name);
	kdebugf2();
}

void Notify::connectSlot(const QString &notifierName, const QString &slotName)
{
	kdebugf();
	NotifierSlots &notifier = notifiers[notifierName];
	if (notifier.notifierSlots[slotName].second == false)
	{
		connect(this, notifySignals[slotName].ascii(), notifier.notifier, notifier.notifierSlots[slotName].first.ascii());
		notifier.notifierSlots[slotName].second = true;
	}
	else
		kdebugm(KDEBUG_WARNING, "WARNING: slot already connected ('%s' '%s')!\n", notifierName.local8Bit().data(), slotName.local8Bit().data());
	kdebugf2();
}

void Notify::disconnectSlot(const QString &notifierName, const QString &slotName)
{
	kdebugf();
	NotifierSlots &notifier = notifiers[notifierName];
	if (notifier.notifierSlots[slotName].second == true)
	{
		disconnect(this, notifySignals[slotName].ascii(), notifier.notifier, notifier.notifierSlots[slotName].first.ascii());
		notifier.notifierSlots[slotName].second = false;
	}
	else
		kdebugm(KDEBUG_WARNING, "WARNING: slot not connected ('%s' '%s')!\n", notifierName.local8Bit().data(), slotName.local8Bit().data());
	kdebugf2();
}

void Notify::emitMessage(const QString &from, const QString &to, const QString &msg, const QMap<QString, QVariant> *parameters, const UserListElement *ule)
{
	kdebugf();
	if (to.isEmpty())
		emit message(from, msg, parameters, ule);
	else if (notifiers.contains(to))
		if (notifiers[to].notifierSlots.contains("Message"))
		{
			connect(this, SIGNAL(privateMessage(const QString &, const QString &, const QMap<QString, QVariant> *, const UserListElement *)),
					notifiers[to].notifier, notifiers[to].notifierSlots["Message"].first.ascii());
			emit privateMessage(from, msg, parameters, ule);
			disconnect(this, SIGNAL(privateMessage(const QString &, const QString &, const QMap<QString, QVariant> *, const UserListElement *)),
					notifiers[to].notifier, notifiers[to].notifierSlots["Message"].first.ascii());
		}
	kdebugf2();
}

QStringList Notify::notifiersList() const
{
	return QStringList(notifiers.keys());
}

const QValueList<Notify::NotifyEvent> &Notify::externalNotifyTypes()
{
	return notifyEvents;
}

Notify::NotifierSlots::NotifierSlots() : notifier(NULL), notifierSlots()
{
}

Notify::NotifierSlots::NotifierSlots(Notifier *n, const QMap<QString, QString> &notifierSlots) : notifier(n), notifierSlots()
{
	kdebugf();
	CONST_FOREACH(i, notifierSlots)
		this->notifierSlots[i.key()] = qMakePair(i.data(), false);
	kdebugf2();
}

void Notify::notify(Notification *notification)
{
	kdebugf();
	QString notifyType = notification->type();
	bool foundNotifier = false;
	bool foundNotifierWithCallbackSupported = notification->getCallbacks().count() == 0;

	CONST_FOREACH(i, notifiers)
		if (config_file.readBoolEntry("Notify", notifyType + '_' + i.key()))
		{
			(*i).notifier->externalEvent(notification);
			foundNotifier = true;
			foundNotifierWithCallbackSupported = true;
		}

	if (!foundNotifierWithCallbackSupported)
		CONST_FOREACH(i, notifiers)
		{
			if ((*i).notifier->callbackCapacity() == CallbackSupported)
			{
				(*i).notifier->externalEvent(notification);
				foundNotifier = true;
				foundNotifierWithCallbackSupported = true;
				break;
			}
		}

	if (!foundNotifier)
		notification->callbackDiscard();

	if (!foundNotifierWithCallbackSupported)
		MessageBox::wrn(tr("Unable to find notifier for %1 event").arg(notification->type()), true);

	kdebugf2();
}

Notify *notify = 0;
NotifySlots *notify_slots = 0;
