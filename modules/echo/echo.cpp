/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2003, 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2004, 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Dariusz Markowicz (darom@alari.pl)
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

#include "debug.h"
#include "gui/widgets/chat-widget.h"
//#include "gui/windows/message-dialog.h"
#include "misc/misc.h"

#include "echo.h"

Echo *echo = 0;

extern "C" KADU_EXPORT int echo_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	kdebugf();

	echo = new Echo();

	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void echo_close()
{
	kdebugf();

	delete echo;
	echo = 0;

	kdebugf2();
}

Echo::Echo() :
	QObject(0)
{
	kdebugf();
	//	MessageDialog::msg(tr("Echo started"));

	triggerAllAccountsRegistered();
	kdebugf2();
}

Echo::~Echo()
{
	kdebugf();
	//	MessageDialog::msg(tr("Echo stopped"));
	kdebugf2();
}

void Echo::accountRegistered(Account account)
{
	kdebugf();
	Protocol *protocol = account.protocolHandler();
	if (!protocol)
	{
		kdebugf2();
		return;
	}

	ChatService *chatService = protocol->chatService();
	if (chatService)
	{
		connect(chatService, SIGNAL(filterIncomingMessage(Chat, Contact, QString &, time_t, bool &)),
				this, SLOT(filterIncomingMessage(Chat, Contact, QString &, time_t, bool &)));
	}
	kdebugf2();
}

void Echo::accountUnregistered(Account account)
{
	kdebugf();
	Protocol *protocol = account.protocolHandler();
	if (!protocol)
	{
		kdebugf2();
		return;
	}

	ChatService *chatService = protocol->chatService();
	if (chatService)
	{
		disconnect(chatService, SIGNAL(filterIncomingMessage(Chat, Contact, QString &, time_t, bool &)),
				this, SLOT(filterIncomingMessage(Chat, Contact, QString &, time_t, bool &)));
	}
	kdebugf2();
}

void Echo::filterIncomingMessage(Chat chat, Contact sender, QString &message, time_t time, bool &ignore)
{
	Q_UNUSED(time)
	Q_UNUSED(ignore)
	Q_UNUSED(sender)

	kdebugf();
	if (message.left(5) == "KADU ")
	{
		kdebugf2();
		return;
	}

	Protocol *protocol = chat.chatAccount().protocolHandler();
	if (!protocol)
	{
		kdebugf2();
		return;
	}

	ChatService *chatService = protocol->chatService();
	if (!chatService)
	{
		kdebugf2();
		return;
	}

	chatService->sendMessage(chat, QString("KADU ECHO: ") + message);

	kdebugf2();
}
