/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qmap.h>
#include <qapplication.h>
#include <qstylesheet.h>

#include "window_notify.h"
#include "debug.h"
#include "config_file.h"
#include "../notify/notify.h"
#include "message_box.h"
#include "userlist.h"
#include "misc.h"

extern "C" int window_notify_init()
{
	kdebugf();
	window_notify=new WindowNotify(NULL, "window_notify");
	kdebugf2();
	return 0;
}

extern "C" void window_notify_close()
{
	kdebugf();
	delete window_notify;
	window_notify=NULL;
	kdebugf2();
}

WindowNotify::WindowNotify(QObject *parent, const char *name) : QObject(parent, name)
{
	kdebugf();

	QMap<QString, QString> s;
	s["NewChat"]=SLOT(newChat(const UinsList &, const QString &, time_t));
	s["NewMessage"]=SLOT(newMessage(const UinsList &, const QString &, time_t, bool &));
	s["ConnError"]=SLOT(connectionError(const QString &));
	s["StatusChanged"]=SLOT(userStatusChanged(const UserListElement &, const UserStatus &));
	s["toAvailable"]=SLOT(userChangedStatusToAvailable(const UserListElement &));
	s["toBusy"]=SLOT(userChangedStatusToBusy(const UserListElement &));
	s["toNotAvailable"]=SLOT(userChangedStatusToNotAvailable(const UserListElement &));
	s["Message"]=SLOT(message(const QString &, const QString &, const QMap<QString, QVariant> *, const UserListElement *));

	config_file.addVariable("Notify", "NewChat_Window", false);
	config_file.addVariable("Notify", "NewMessage_Window", false);
	config_file.addVariable("Notify", "ConnError_Window", false);
	config_file.addVariable("Notify", "ChangingStatus_Window", false);
	config_file.addVariable("Notify", "toAvailable_Window", false);
	config_file.addVariable("Notify", "toBusy_Window", false);
	config_file.addVariable("Notify", "toNotAvailable_Window", false);
	config_file.addVariable("Notify", "Message_Window", false);

	notify->registerNotifier("Window", this, s);
	kdebugf2();
}

WindowNotify::~WindowNotify()
{
	kdebugf();
	notify->unregisterNotifier("Window");
	kdebugf2();
}

void WindowNotify::newChat(const UinsList &senders, const QString& msg, time_t /*time*/)
{
	kdebugf();
	MessageBox::msg(narg(tr("Chat with <b>%1</b><br/> <small>%2</small>"), userlist.byUinValue(senders[0]).altNick(),msg));
	kdebugf2();
}

void WindowNotify::newMessage(const UinsList &senders, const QString& msg, time_t /*time*/, bool & /*grab*/)
{
	kdebugf();
	MessageBox::msg(narg(tr("New message from <b>%1</b><br/> <small>%2</small>"), userlist.byUinValue(senders[0]).altNick(), msg));
	kdebugf2();
}

void WindowNotify::connectionError(const QString &message)
{
	kdebugf();
	MessageBox::msg(tr("<b>Error:</b> %1").arg(message));
	kdebugf2();
}

void WindowNotify::userStatusChanged(const UserListElement &ule, const UserStatus &oldStatus)
{
	kdebugf();

	MessageBox::msg(narg(tr("<b>%1</b> changed status from <i>%2</i> to <i>%3</i>"),
					ule.altNick(),
					qApp->translate("@default", oldStatus.name()),
					qApp->translate("@default", ule.status().name())));
	kdebugf2();
}

void WindowNotify::userChangedStatusToAvailable(const UserListElement &ule)
{
	kdebugf();

	MessageBox::msg(narg(tr("<b>%1</b> changed status to <i>%2</i><br/> <small>%3</small>"),
					ule.altNick(),
					qApp->translate("@default", ule.status().name()),
					QStyleSheet::escape(ule.status().description())));
	kdebugf2();
}

void WindowNotify::userChangedStatusToBusy(const UserListElement &ule)
{
	kdebugf();
	userChangedStatusToAvailable(ule);
	kdebugf2();
}

void WindowNotify::userChangedStatusToNotAvailable(const UserListElement &ule)
{
	kdebugf();
	userChangedStatusToAvailable(ule);
	kdebugf2();
}

void WindowNotify::message(const QString &from, const QString &message, const QMap<QString, QVariant> * /*parameters*/, const UserListElement *ule)
{
	kdebugf();
	if (ule==NULL)
		MessageBox::msg(narg(tr("<b>From %1:</b>%2"), from, message));
	else
	{
		MessageBox::msg(narg(tr("<b>From %1: %2</b> changed status to <i>%3</i>"),
						from,
						ule->altNick(),
						qApp->translate("@default", ule->status().name())));
	}
	kdebugf2();
}

WindowNotify *window_notify=NULL;
