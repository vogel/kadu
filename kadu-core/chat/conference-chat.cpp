/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "accounts/account_manager.h"
// #include "protocols/protocol.h"

#include "conference-chat.h"

ConferenceChat::ConferenceChat(Account *currentAccount, ContactList contacts, QUuid uuid)
	: Chat(currentAccount, uuid), CurrentContacts(contacts)
{
}

ConferenceChat::~ConferenceChat()
{
}

