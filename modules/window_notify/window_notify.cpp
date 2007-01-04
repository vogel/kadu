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

/**
 * @ingroup window_notify
 * @{
 */
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

WindowNotify::WindowNotify(QObject *parent, const char *name) : Notifier(parent, name)
{
	kdebugf();

	QMap<QString, QString> s;
	s["NewChat"]=SLOT(newChat(Protocol *, UserListElements, const QString &, time_t));
	s["NewMessage"]=SLOT(newMessage(Protocol *, UserListElements, const QString &, time_t, bool &));
	s["ConnError"]=SLOT(connectionError(Protocol *, const QString &));
	s["toAvailable"]=SLOT(userChangedStatusToAvailable(const QString &, UserListElement));
	s["toBusy"]=SLOT(userChangedStatusToBusy(const QString &, UserListElement));
	s["toInvisible"]=SLOT(userChangedStatusToInvisible(const QString &, UserListElement));
	s["toNotAvailable"]=SLOT(userChangedStatusToNotAvailable(const QString &, UserListElement));

	s["StatusChanged"]=SLOT(userStatusChanged(UserListElement, QString, const UserStatus &));
	s["Message"]=SLOT(message(const QString &, const QString &, const QMap<QString, QVariant> *, const UserListElement *));

	config_file.addVariable("Notify", "NewChat_Window", false);
	config_file.addVariable("Notify", "NewMessage_Window", false);
	config_file.addVariable("Notify", "ConnError_Window", false);
	config_file.addVariable("Notify", "ChangingStatus_Window", false);
	config_file.addVariable("Notify", "toAvailable_Window", false);
	config_file.addVariable("Notify", "toBusy_Window", false);
	config_file.addVariable("Notify", "toInvisible_Window", false);
	config_file.addVariable("Notify", "toNotAvailable_Window", false);
	config_file.addVariable("Notify", "Message_Window", false);

	notify->registerNotifier(QT_TRANSLATE_NOOP("@default", "Window"), this, s);
	kdebugf2();
}

WindowNotify::~WindowNotify()
{
	kdebugf();
	notify->unregisterNotifier("Window");
	kdebugf2();
}

void WindowNotify::newChat(Protocol * /*protocol*/, UserListElements senders, const QString &msg, time_t /*t*/)
{
	kdebugf();
	MessageBox::msg(narg(tr("Chat with <b>%1</b><br/> <small>%2</small>"),
			senders[0].altNick(),msg));
	kdebugf2();
}

void WindowNotify::newMessage(Protocol * /*protocol*/, UserListElements senders, const QString &msg, time_t /*t*/, bool &/*grab*/)
{
	kdebugf();
	MessageBox::msg(narg(tr("New message from <b>%1</b><br/> <small>%2</small>"),
			senders[0].altNick(), msg));
	kdebugf2();
}

void WindowNotify::connectionError(Protocol *, const QString &message)
{
	kdebugf();
	MessageBox::msg(tr("<b>Error:</b> %1").arg(message));
	kdebugf2();
}

void WindowNotify::userStatusChanged(UserListElement ule, QString /*protocolName*/, const UserStatus &oldStatus)
{
	kdebugf();

	MessageBox::msg(narg(tr("<b>%1</b> changed status from <i>%2</i> to <i>%3</i>"),
					ule.altNick(),
					qApp->translate("@default", oldStatus.name().ascii()),
					qApp->translate("@default", ule.status("Gadu").name().ascii())));
	kdebugf2();
}

void WindowNotify::userChangedStatusToAvailable(const QString &/*protocolName*/, UserListElement ule)
{
	kdebugf();

	MessageBox::msg(narg(tr("<b>%1</b> changed status to <i>%2</i><br/> <small>%3</small>"),
					ule.altNick(),
					qApp->translate("@default", ule.status("Gadu").name().ascii()),
					QStyleSheet::escape(ule.status("Gadu").description())));
	kdebugf2();
}

void WindowNotify::userChangedStatusToBusy(const QString &protocolName, UserListElement ule)
{
	kdebugf();
	userChangedStatusToAvailable(protocolName, ule);
	kdebugf2();
}

void WindowNotify::userChangedStatusToInvisible(const QString &protocolName, UserListElement ule)
{
	kdebugf();
	userChangedStatusToAvailable(protocolName, ule);
	kdebugf2();
}

void WindowNotify::userChangedStatusToNotAvailable(const QString &protocolName, UserListElement ule)
{
	kdebugf();
	userChangedStatusToAvailable(protocolName, ule);
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
						qApp->translate("@default", ule->status("Gadu").name().ascii())));
	}
	kdebugf2();
}

void WindowNotify::externalEvent(const QString &/*notifyType*/, const QString &msg, const UserListElements &ules)
{
	kdebugf();

	if (ules.count() > 0)
		MessageBox::msg(ules.altNicks().join(",") + ": " + msg);
	else
		MessageBox::msg(msg);

	kdebugf2();
}

WindowNotify *window_notify=NULL;

/** @} */

