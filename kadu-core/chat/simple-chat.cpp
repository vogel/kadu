/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account-manager.h"
#include "chat/type/chat-type-manager.h"
#include "contacts/contact.h"
#include "contacts/contact-manager.h"
#include "buddies/buddy-manager.h"
#include "protocols/protocol.h"

#include "simple-chat.h"
#include "chat-manager.h"

SimpleChat::SimpleChat(StoragePoint *storage) :
		Chat(storage), CurrentContactAccountData(0)
{
}

SimpleChat::SimpleChat(Account currentAccount, Contact *cad, QUuid uuid) :
		Chat(currentAccount, uuid), CurrentContactAccountData(cad)
{
}

SimpleChat::~SimpleChat()
{
}

void SimpleChat::load()
{
	if (!isValidStorage())
		return;

	Chat::load();

	QString cadUuid = loadValue<QString>("ContactAccountData");
	if (cadUuid.isNull())
	{
		Buddy buddy = BuddyManager::instance()->byUuid(loadValue<QString>("Contact"));
		CurrentContactAccountData = buddy.contact(account());
		removeValue("Contact");
	}
	else
	{
		ContactManager::instance()->ensureLoaded(account());
		CurrentContactAccountData = ContactManager::instance()->byUuid(cadUuid);
	}

	refreshTitle();
}

void SimpleChat::store()
{
	if (!isValidStorage())
		return;

	Chat::store();
	storeValue("Type", "Simple");

	if (CurrentContactAccountData)
		storeValue("ContactAccountData", CurrentContactAccountData->uuid().toString());
}

ChatType SimpleChat::type() const
{
	return ChatTypeManager::instance()->chatType("SimpleChat");
}

BuddySet SimpleChat::buddies() const
{
	if (!CurrentContactAccountData)
		return BuddySet();
	return BuddySet(CurrentContactAccountData->buddy());
}

QString SimpleChat::name() const
{
	if (!CurrentContactAccountData)
		return QString::null;
	return CurrentContactAccountData->buddy().display();
}
