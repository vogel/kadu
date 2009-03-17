/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "gui/windows/kadu-window.h"
#include "protocols/protocol.h"
#include "protocols/protocol_factory.h"
#include "protocols/services/chat-service.h"

#include "config_file.h"
#include "kadu.h"

#include "core.h"

Core * Core::Instance = 0;

Core * Core::instance()
{
	if (!Instance)
		Instance = new Core();

	return Instance;
}

Core::Core()
{
	Myself.setDisplay(config_file.readEntry("General", "Nick"));

	triggerAllAccountsRegistered();
}

Core::~Core()
{
	triggerAllAccountsUnregistered();
}

void Core::accountRegistered(Account *account)
{
	Protocol *protocol = account->protocol();

	ChatService *chatService = protocol->chatService();
	if (chatService)
		connect(chatService, SIGNAL(messageReceived(Account *, Contact, ContactList, const QString &, time_t)),
			this, SIGNAL(messageReceived(Account *, Contact, ContactList, const QString &, time_t)));

	connect(protocol, SIGNAL(connecting(Account *)), this, SIGNAL(connecting()));
	connect(protocol, SIGNAL(connected(Account *)), this, SIGNAL(connected()));
	connect(protocol, SIGNAL(disconnected(Account *)), this, SIGNAL(disconnected()));
	connect(protocol, SIGNAL(statusChanged(Account *, Status)),
			this, SLOT(statusChanged(Account *, Status)));

	ContactAccountData *contactAccountData = protocol->protocolFactory()->
			newContactAccountData(Myself, account, account->id());
	Myself.addAccountData(contactAccountData);
}

void Core::accountUnregistered(Account *account)
{
	Protocol *protocol = account->protocol();

	ChatService *chatService = protocol->chatService();
	if (chatService)
		disconnect(chatService, SIGNAL(messageReceived(Account *, Contact, ContactList, const QString &, time_t)),
			this, SIGNAL(messageReceived(Account *, Contact, ContactList, const QString &, time_t)));

	disconnect(protocol, SIGNAL(connecting(Account *)), this, SIGNAL(connecting()));
	disconnect(protocol, SIGNAL(connected(Account *)), this, SIGNAL(connected()));
	disconnect(protocol, SIGNAL(disconnected(Account *)), this, SIGNAL(disconnected()));
	disconnect(protocol, SIGNAL(statusChanged(Account *, Status)),
			this, SLOT(statusChanged(Account *, Status)));

	Myself.removeAccountData(account);
}

void Core::createGui()
{
	new Kadu(0);
	(new KaduWindow(0))->show();
}
