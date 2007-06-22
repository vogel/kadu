/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "echo.h"

//#include "message_box.h"
#include "debug.h"
#include "misc.h"

extern "C" int echo_init()
{
	echo = new Echo();
	return 0;
}

extern "C" void echo_close()
{
	delete echo;
	echo = NULL;
}

Echo::Echo() : QObject(NULL, "echo")
{
//	MessageBox::msg(tr("Echo started"));
	connect(gadu, SIGNAL(messageReceived(Protocol *, UserListElements, const QString &, time_t)),
			this, SLOT(messageReceived(Protocol *, UserListElements, const QString &, time_t)));
}

Echo::~Echo()
{
	disconnect(gadu, SIGNAL(messageReceived(Protocol *, UserListElements, const QString &, time_t)),
			this, SLOT(messageReceived(Protocol *, UserListElements, const QString &, time_t)));
//	MessageBox::msg(tr("Echo stopped"));
}

void Echo::messageReceived(Protocol *protocol, UserListElements senders, const QString& msg, time_t time)
{
	kdebugf();
	if (msg.left(5) != "KADU ")
	{
		QString resp = QString("KADU ECHO: ") + msg;
		gadu->sendMessage(senders, resp);
	}
	kdebugf2();
}

Echo* echo;
