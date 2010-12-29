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

#include "accounts/account.h"
#include "core/core.h"
#include "gui/windows/message-dialog.h"
#include "protocols/services/chat-service.h"
#include "protocols/protocol.h"

#include "echo-message.h"

#include "echo.h"

Echo * Echo::Instance = 0;

void Echo::createInstance()
{
	if (!Instance)
		Instance = new Echo();
}

void Echo::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

Echo * Echo::instance()
{
	return Instance;
}

Echo::Echo(QObject *parent) :
		QObject(parent)
{
	MessageDialog::show("dialog-information", tr("Kadu"), tr("Echo started"));

	triggerAllAccountsRegistered();
}

Echo::~Echo()
{
	// if we are closing the user won't notice this message anyway
	if (!Core::instance()->isClosing())
		MessageDialog::show("dialog-information", tr("Kadu"), tr("Echo stopped"));

	triggerAllAccountsUnregistered();
}

void Echo::accountRegistered(Account account)
{
	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return;

	ChatService *chatService = protocol->chatService();
	if (chatService)
		connect(chatService, SIGNAL(filterIncomingMessage(Chat, Contact, QString &, time_t, bool &)),
				this, SLOT(filterIncomingMessage(Chat, Contact, QString &, time_t, bool &)));
}

void Echo::accountUnregistered(Account account)
{
	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return;

	ChatService *chatService = protocol->chatService();
	if (chatService)
		disconnect(chatService, SIGNAL(filterIncomingMessage(Chat, Contact, QString &, time_t, bool &)),
				this, SLOT(filterIncomingMessage(Chat, Contact, QString &, time_t, bool &)));
}

void Echo::filterIncomingMessage(Chat chat, Contact sender, QString &message, time_t time, bool &ignore)
{
	Q_UNUSED(sender)
	Q_UNUSED(time)
	Q_UNUSED(ignore)

	// to prevent infinite loop in case there is also Kadu Echo at the second end
	if (message.startsWith(ECHO_MESSAGE))
		return;

	Protocol *protocol = chat.chatAccount().protocolHandler();
	if (!protocol)
		return;

	ChatService *chatService = protocol->chatService();
	if (!chatService)
		return;

	chatService->sendMessage(chat, ECHO_MESSAGE + message);
}
