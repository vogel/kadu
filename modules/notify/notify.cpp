/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "notify.h"
#include "debug.h"
#include "config_file.h"
#include "notify_slots.h"
#include "config_dialog.h"
#include "kadu.h"
#include "chat.h"

extern "C" int notify_init()
{
	kdebugf();
	notify=new Notify();
	
	
	kdebugf2();
	return 0;
}

extern "C" void notify_close()
{
	kdebugf();
	delete notify;
	notify=NULL;
	kdebugf2();
}

Notify::Notify()
{
	kdebugf();
	notifySignals["NewChat"]=			QString(SIGNAL(newChat(UinsList, const QString &, time_t)));
	notifySignals["NewMessage"]=		QString(SIGNAL(newMessage(UinsList, const QString &, time_t, bool &)));
	notifySignals["ConnError"]=			QString(SIGNAL(connectionError(const QString &)));
	notifySignals["ChangingStatus"]=	QString(SIGNAL(userChangingStatus(const UinType, const unsigned int, const unsigned int)));
	notifySignals["toAvailable"]=		QString(SIGNAL(userChangedStatusToAvailable(const UserListElement &)));
	notifySignals["toBusy"]=			QString(SIGNAL(userChangedStatusToBusy(const UserListElement &)));
	notifySignals["toNotAvailable"]=	QString(SIGNAL(userChangedStatusToNotAvailable(const UserListElement &)));
	notifySignals["Message"]=			QString(SIGNAL(message(const QString &, const QString &, const QMap<QString, QVariant> *, const UserListElement *)));

	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "Notify"));
	ConfigDialog::addHGroupBox("Notify", "Notify", QT_TRANSLATE_NOOP("@default", "Notify configuration"));
	
	//pierwsza kolumna - nazwy
	ConfigDialog::addVBox("Notify", "Notify configuration", "names");
	ConfigDialog::addLabel("Notify", "names", "");
	ConfigDialog::addLabel("Notify", "names", QT_TRANSLATE_NOOP("@default", "Connection error"));
	ConfigDialog::addLabel("Notify", "names", QT_TRANSLATE_NOOP("@default", "New chat"));
	ConfigDialog::addLabel("Notify", "names", QT_TRANSLATE_NOOP("@default", "New message"));
	ConfigDialog::addLabel("Notify", "names", QT_TRANSLATE_NOOP("@default", "User is changing status"));
	ConfigDialog::addLabel("Notify", "names", QT_TRANSLATE_NOOP("@default", "User changed status to \"Available\""));
	ConfigDialog::addLabel("Notify", "names", QT_TRANSLATE_NOOP("@default", "User changed status to \"Busy\""));
	ConfigDialog::addLabel("Notify", "names", QT_TRANSLATE_NOOP("@default", "User changed status to \"Not available\""));
	ConfigDialog::addLabel("Notify", "names", QT_TRANSLATE_NOOP("@default", "Other message"));

	connect(kadu, SIGNAL(connectionError(const QString &)), this, notifySignals["ConnError"]);
	connect(gadu, SIGNAL(chatMsgReceived1(UinsList, const QString&, time_t,bool&)), this, SLOT(probablyNewMessage(UinsList, const QString&, time_t, bool&)));
	connect(gadu, SIGNAL(chatMsgReceived2(UinsList, const QString&, time_t)), this, SLOT(probablyNewChat(UinsList, const QString&, time_t)));

	connect(&userlist, SIGNAL(changingStatus(const UinType, const unsigned int, const unsigned int, bool)),
		this, SLOT(changingStatus(const UinType, const unsigned int, const unsigned int, bool)));
	connect(&userlist, SIGNAL(statusModified(UserListElement *, bool)),
		this, SLOT(changedStatus(UserListElement *, bool)));

	notify_slots=new NotifySlots();

	ConfigDialog::addCheckBox("Notify", "Notify",
		QT_TRANSLATE_NOOP("@default", "Ignore changes on connection to server"), "NotifyIgnoreOnConnection", true);
	ConfigDialog::addCheckBox("Notify", "Notify",
		QT_TRANSLATE_NOOP("@default", "Notify when users become available"), "NotifyStatusChange", false);
	ConfigDialog::addCheckBox("Notify", "Notify",
		QT_TRANSLATE_NOOP("@default", "Notify about all users"), "NotifyAboutAll", false);

	ConfigDialog::addGrid("Notify", "Notify" ,"listboxy",3);

		ConfigDialog::addGrid("Notify", "listboxy", "listbox1", 1);
			ConfigDialog::addLabel("Notify", "listbox1", QT_TRANSLATE_NOOP("@default", "Available"));
			ConfigDialog::addListBox("Notify", "listbox1","available");

		ConfigDialog::addGrid("Notify", "listboxy", "listbox2", 1);
			ConfigDialog::addPushButton("Notify", "listbox2", "", "AddToNotifyList","","forward");
			ConfigDialog::addPushButton("Notify", "listbox2", "", "RemoveFromNotifyList","","back");

		ConfigDialog::addGrid("Notify", "listboxy", "listbox3", 1);
			ConfigDialog::addLabel("Notify", "listbox3", QT_TRANSLATE_NOOP("@default", "Tracked"));
			ConfigDialog::addListBox("Notify", "listbox3", "track");

	ConfigDialog::connectSlot("Notify", "", SIGNAL(clicked()), notify_slots, SLOT(_Right()), "forward");
	ConfigDialog::connectSlot("Notify", "", SIGNAL(clicked()), notify_slots, SLOT(_Left()), "back");
	ConfigDialog::connectSlot("Notify", "available", SIGNAL(doubleClicked(QListBoxItem *)),
		notify_slots, SLOT(_Right2(QListBoxItem *)));
	ConfigDialog::connectSlot("Notify", "track", SIGNAL(doubleClicked(QListBoxItem *)),
		notify_slots, SLOT(_Left2(QListBoxItem *)));

	ConfigDialog::registerSlotOnCreate(notify_slots, SLOT(onCreateConfigDialog()));
	ConfigDialog::registerSlotOnApply(notify_slots, SLOT(onApplyConfigDialog()));
	ConfigDialog::registerSlotOnApply(this, SLOT(updateConnections()));

	kdebugf2();
}

Notify::~Notify()
{
	kdebugf();

	ConfigDialog::disconnectSlot("Notify", "", SIGNAL(clicked()), notify_slots, SLOT(_Right()), "forward");
	ConfigDialog::disconnectSlot("Notify", "", SIGNAL(clicked()), notify_slots, SLOT(_Left()), "back");
	ConfigDialog::disconnectSlot("Notify", "available", SIGNAL(doubleClicked(QListBoxItem *)),
		notify_slots, SLOT(_Right2(QListBoxItem *)));
	ConfigDialog::disconnectSlot("Notify", "track", SIGNAL(doubleClicked(QListBoxItem *)),
		notify_slots, SLOT(_Left2(QListBoxItem *)));

	ConfigDialog::unregisterSlotOnCreate(notify_slots, SLOT(onCreateConfigDialog()));
	ConfigDialog::unregisterSlotOnApply(notify_slots, SLOT(onApplyConfigDialog()));
	ConfigDialog::unregisterSlotOnApply(this, SLOT(updateConnections()));

	delete notify_slots;
	notify_slots=NULL;

			ConfigDialog::removeControl("Notify", "track");
			ConfigDialog::removeControl("Notify", "Tracked");
		ConfigDialog::removeControl("Notify", "listbox3");
			ConfigDialog::removeControl("Notify", "", "back");
			ConfigDialog::removeControl("Notify", "", "forward");
		ConfigDialog::removeControl("Notify", "listbox2");
			ConfigDialog::removeControl("Notify", "available");
			ConfigDialog::removeControl("Notify", "Available");
		ConfigDialog::removeControl("Notify", "listbox1");
	ConfigDialog::removeControl("Notify", "listboxy");

	ConfigDialog::removeControl("Notify", "Notify about all users");
	ConfigDialog::removeControl("Notify", "Notify when users become available");
	ConfigDialog::removeControl("Notify", "Ignore changes on connection to server");

	disconnect(kadu, SIGNAL(connectionError(const QString &)), this, notifySignals["ConnError"]);
	disconnect(gadu, SIGNAL(chatMsgReceived1(UinsList, const QString&, time_t,bool&)), this, SLOT(probablyNewMessage(UinsList, const QString&, time_t, bool&)));
	disconnect(gadu, SIGNAL(chatMsgReceived2(UinsList, const QString&, time_t)), this, SLOT(probablyNewChat(UinsList, const QString&, time_t)));
	disconnect(&userlist, SIGNAL(changingStatus(const UinType, const unsigned int, const unsigned int, bool)),
		this, SLOT(changingStatus(const UinType, const unsigned int, const unsigned int, bool)));
	disconnect(&userlist, SIGNAL(statusModified(UserListElement *, bool)),
		this, SLOT(changedStatus(UserListElement *, bool)));

	if (notifiers.size()>0)
	{
		kdebugm(KDEBUG_WARNING, "WARNING: not unregistered notifiers found! (%d)\n", notifiers.size());
		QValueList<QString> notifierNames=notifiers.keys();
		for (QValueList<QString>::iterator i=notifierNames.begin(); i!=notifierNames.end(); i++)
			unregisterNotifier(*i);
	}
	
	//pierwsza kolumna - nazwy
	ConfigDialog::removeControl("Notify", "");
	ConfigDialog::removeControl("Notify", "Connection error");
	ConfigDialog::removeControl("Notify", "New chat");
	ConfigDialog::removeControl("Notify", "New message");
	ConfigDialog::removeControl("Notify", "User is changing status");
	ConfigDialog::removeControl("Notify", "User changed status to \"Available\"");
	ConfigDialog::removeControl("Notify", "User changed status to \"Busy\"");
	ConfigDialog::removeControl("Notify", "User changed status to \"Not available\"");
	ConfigDialog::removeControl("Notify", "Other message");

	ConfigDialog::removeControl("Notify", "names");
	ConfigDialog::removeControl("Notify", "Notify configuration");
	ConfigDialog::removeTab("Notify");

	notifySignals.clear();
	kdebugf2();
}

void Notify::changingStatus(const UinType uin, const unsigned int oldstatus, const unsigned int status, bool onConnection)
{
	kdebugf();

	if (!config_file.readBoolEntry("Notify","NotifyStatusChange"))
	{
		kdebugf2();
		return;
	}
	
	if (onConnection && config_file.readBoolEntry("Notify", "NotifyIgnoreOnConnection"))
	{
		kdebugf2();
		return;
	}

	if (userlist.containsUin(uin))
	{
		if (!userlist.byUin(uin).notify && !config_file.readBoolEntry("Notify","NotifyAboutAll"))
		{
			kdebugf2();
			return;
		}
	}
	else if (!config_file.readBoolEntry("Notify","NotifyAboutAll"))
	{
		kdebugf2();
		return;
	}

	if (oldstatus==0 || status==0)
	{
		kdebugm(KDEBUG_ERROR, "ERROR: oldstatus==0 or status==0 (oldstatus=%d status=%d)\n",  oldstatus, status);
		return;
	}
	emit userChangingStatus(uin, oldstatus, status);
	
	kdebugf2();
}

void Notify::changedStatus(UserListElement *ule, bool onConnection)
{
	kdebugf();
	if (!config_file.readBoolEntry("Notify","NotifyStatusChange"))
	{
		kdebugf2();
		return;
	}

	if (onConnection && config_file.readBoolEntry("Notify", "NotifyIgnoreOnConnection"))
	{
		kdebugf2();
		return;
	}

	if (!ule->notify && !config_file.readBoolEntry("Notify","NotifyAboutAll"))
	{
		kdebugf2();
		return;
	}
	else if (!config_file.readBoolEntry("Notify","NotifyAboutAll"))
	{
		kdebugf2();
		return;
	}

	if (ule->status==0)
	{
		kdebugm(KDEBUG_ERROR, "ERROR: status==0 (%s)\n", ule->altnick.local8Bit().data());
		return;
	}
	int status2=statusGGToStatusNr(ule->status);
//	kdebugm(KDEBUG_INFO, "ule->status: %d statusNr:%d %s\n", ule->status, status2, ule->altnick.local8Bit().data());
	if (status2==0 || status2==1)
		emit userChangedStatusToAvailable(*ule);
	else if (status2==2 || status2==3)
		emit userChangedStatusToBusy(*ule);
	else if (status2>=4 && status2<=7)
		emit userChangedStatusToNotAvailable(*ule);
	else
		;//jeszcze jest status "blokowany", który nie jest tu obs³ugiwany
	kdebugf2();
}

void Notify::probablyNewChat(UinsList senders, const QString& msg, time_t time)
{
	kdebugf();
	Chat* chat=chat_manager->findChatByUins(senders);
	if (chat==NULL)
		emit newChat(senders, msg, time);
	kdebugf2();
}

void Notify::probablyNewMessage(UinsList senders, const QString& msg, time_t time, bool &grab)
{
	kdebugf();
	Chat* chat=chat_manager->findChatByUins(senders);
	if (chat!=NULL)
		emit newMessage(senders, msg, time, grab);
	kdebugf2();
}

void Notify::addConfigColumn(const QString &name, const QMap<QString, QString> &notifierSlots)
{
	kdebugf();
	ConfigDialog::addVBox("Notify", "Notify configuration", name+"_vbox");
	ConfigDialog::addLabel("Notify", name+"_vbox", name);
	
	QStringList t;
	t<<"ConnError"<<"NewChat"<<"NewMessage"<<"ChangingStatus"<<"toAvailable"<<"toBusy"<<"toNotAvailable"<<"Message";
	
	int i=1;
	for (QStringList::iterator it=t.begin(); it!=t.end(); it++, i++)
	{
		ConfigDialog::addCheckBox("Notify", name+"_vbox", " ", (*it)+"_"+name, false, "", name+QString::number(i));
		if (!notifierSlots.contains(*it))
			notify_slots->registerDisabledControl(name+QString::number(i));
	}
	kdebugf2();
}

void Notify::removeConfigColumn(const QString &name, const QMap<QString, QPair<QString, bool> > &notifierSlots)
{
	kdebugf();

	QStringList t;
	t<<"ConnError"<<"NewChat"<<"NewMessage"<<"ChangingStatus"<<"toAvailable"<<"toBusy"<<"toNotAvailable"<<"Message";
	
	int i=1;
	for (QStringList::iterator it=t.begin(); it!=t.end(); it++, i++)
	{
		ConfigDialog::removeControl("Notify", " ", name+QString::number(i));
		if (!notifierSlots.contains(*it))
			notify_slots->unregisterDisabledControl(name+QString::number(i));
	}

	ConfigDialog::removeControl("Notify", name);
	ConfigDialog::removeControl("Notify", name+"_vbox");
	kdebugf2();
}

void Notify::updateConnections()
{
	kdebugf();
	
	for(QMap<QString, Notifier>::iterator i=notifiers.begin(); i!=notifiers.end(); i++)
	{
		QString notifierName=i.key();
		Notifier &notifier=i.data();
		for(QMap<QString, QPair<QString, bool> >::iterator j=notifier.notifierSlots.begin(); j!=notifier.notifierSlots.end(); j++)
		{
			QString signalName=j.key();
			QPair<QString, bool> &connection=j.data();
			if (config_file.readBoolEntry("Notify", signalName+"_"+notifierName)!=connection.second)
			{
				if (connection.second)
					disconnect(this, notifySignals[signalName], notifier.notifier, connection.first);
				else
					connect(this, notifySignals[signalName], notifier.notifier, connection.first);
				connection.second=!connection.second;
			}
		}
	}
	
	kdebugf2();
}

void Notify::registerNotifier(const QString &name, QObject *notifier,
							const QMap<QString, QString> &notifierSlots)
{
	kdebugf();
	if (notifiers.contains(name))
	{
		kdebugm(KDEBUG_WARNING, "WARNING: '%s' already exists in notifiers! "
		"strange... unregistering old Notifier\n", name.local8Bit().data());
		
		unregisterNotifier(name);
	}
	notifiers[name]=Notifier(notifier, notifierSlots);
	
	for (QMap<QString, QString>::iterator i=notifySignals.begin(); i!=notifySignals.end(); i++)
		if (config_file.readBoolEntry("Notify", i.key()+"_"+name) && notifierSlots.contains(i.key()))
			connectSlot(name, i.key());
	addConfigColumn(name, notifierSlots);
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
	Notifier notifier=notifiers[name];
	removeConfigColumn(name, notifier.notifierSlots);
	for (QMap<QString, QString>::iterator i=notifySignals.begin(); i!=notifySignals.end(); i++)
		if (config_file.readBoolEntry("Notify", i.key()+"_"+name) && notifier.notifierSlots.contains(i.key()))
			disconnectSlot(name, i.key());
	notifiers.remove(name);
	kdebugf2();
}

void Notify::connectSlot(const QString &notifierName, const QString &slotName)
{
	kdebugf();
	Notifier &notifier=notifiers[notifierName];
	if (notifier.notifierSlots[slotName].second==false)
	{
		connect(this, notifySignals[slotName], notifier.notifier, notifier.notifierSlots[slotName].first);
		notifier.notifierSlots[slotName].second=true;
	}
	else
		kdebugm(KDEBUG_WARNING, "WARNING: slot already connected ('%s' '%s')!\n", notifierName.local8Bit().data(), slotName.local8Bit().data());
	kdebugf2();
}

void Notify::disconnectSlot(const QString &notifierName, const QString &slotName)
{
	kdebugf();
	Notifier &notifier=notifiers[notifierName];
	if (notifier.notifierSlots[slotName].second==true)
	{
		disconnect(this, notifySignals[slotName], notifier.notifier, notifier.notifierSlots[slotName].first);
		notifier.notifierSlots[slotName].second=false;
	}
	else
		kdebugm(KDEBUG_WARNING, "WARNING: slot not connected ('%s' '%s')!\n", notifierName.local8Bit().data(), slotName.local8Bit().data());
	kdebugf2();
}

void Notify::emitMessage(const QString &from, const QString &to, const QString &msg, const QMap<QString, QVariant> *parameters, const UserListElement *ule)
{
	kdebugf();
	if (to==QString::null)
		emit message(from, msg, parameters, ule);
	else if (notifiers.contains(to))
		if (notifiers[to].notifierSlots.contains("Message"))
		{
			connect(this, SIGNAL(privateMessage(const QString &, const QString &, const QMap<QString, QVariant> *, const UserListElement *)),
					notifiers[to].notifier, notifiers[to].notifierSlots["Message"].first);
			emit privateMessage(from, msg, parameters, ule);
			disconnect(this, SIGNAL(privateMessage(const QString &, const QString &, const QMap<QString, QVariant> *, const UserListElement *)),
					notifiers[to].notifier, notifiers[to].notifierSlots["Message"].first);
		}
	kdebugf2();
}

Notify::Notifier::Notifier() : notifier(NULL)
{
}

Notify::Notifier::Notifier(QObject *o, const QMap<QString, QString> &notifierSlots) : notifier(o)
{
	kdebugf();
	for (QMap<QString, QString>::const_iterator i=notifierSlots.begin(); i!=notifierSlots.end(); i++)
		this->notifierSlots[i.key()]=qMakePair(i.data(), false);
	kdebugf2();
}

Notify *notify=NULL;
NotifySlots *notify_slots=NULL;
