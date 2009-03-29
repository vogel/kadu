/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "accounts/account_manager.h"
#include "protocols/protocol.h"

#include "simple-chat.h"
#include "chat-manager.h"

SimpleChat::SimpleChat(Account *currentAccount, Contact contact, QUuid uuid)
	: Chat(currentAccount, uuid), CurrentContact(contact)
{
}

SimpleChat::~SimpleChat()
{
}

ContactList SimpleChat::currentContacts()
{
	ContactList c;
	c.append(CurrentContact);
	return c;
}
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "accounts/account_manager.h"
#include "protocols/protocol.h"

#include "simple-chat.h"
#include "chat-manager.h"

SimpleChat::SimpleChat(Account *currentAccount, Contact contact, QUuid uuid)
	: Chat(currentAccount, uuid), CurrentContact(contact)
{
}

SimpleChat::~SimpleChat()
{
}

ContactList SimpleChat::currentContacts()
{
	ContactList c;
	c.append(CurrentContact);
	return c;
}
