/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2004, 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "echo.h"

//#include "message_box.h"
#include "debug.h"
#include "misc/misc.h"

extern "C" KADU_EXPORT int echo_init(bool firstLoad)
{
	echo = new Echo();
	return 0;
}

extern "C" KADU_EXPORT void echo_close()
{
	delete echo;
	echo = NULL;
}

Echo::Echo() : QObject(NULL, "echo")
{
//	MessageDialog::msg(tr("Echo started"));
	connect(gadu, SIGNAL(messageReceived(Account *, UserListElements, const QString &, time_t)),
			this, SLOT(messageReceived(Account *, UserListElements, const QString &, time_t)));
}

Echo::~Echo()
{
	disconnect(gadu, SIGNAL(messageReceived(Account *, UserListElements, const QString &, time_t)),
			this, SLOT(messageReceived(Account *, UserListElements, const QString &, time_t)));
//	MessageDialog::msg(tr("Echo stopped"));
}

void Echo::messageReceived(Account *protocol, UserListElements senders, const QString& msg, time_t time)
{
	kdebugf();
	if (msg.left(5) != "KADU ")
	{
		protocol->sendMessage(senders, QString("KADU ECHO: ") + msg);
	}
	kdebugf2();
}

Echo* echo;
