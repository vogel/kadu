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
		Chat(storage), CurrentContact(Contact::null)
{
}

SimpleChat::SimpleChat(Account currentAccount, Contact contact, QUuid uuid) :
		Chat(currentAccount, uuid), CurrentContact(contact)
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

	QString cadUuid = loadValue<QString>("Contact");
	if (cadUuid.isNull())
	{
		Buddy buddy = BuddyManager::instance()->byUuid(loadValue<QString>("Contact"));
		CurrentContact = buddy.contact(account());
		removeValue("Contact");
	}
	else
		CurrentContact = ContactManager::instance()->byUuid(cadUuid);

	refreshTitle();
}

void SimpleChat::store()
{
	if (!isValidStorage())
		return;

	Chat::store();
	storeValue("Type", "Simple");

	if (!CurrentContact.isNull())
		storeValue("Contact", CurrentContact.uuid().toString());
}

ChatType SimpleChat::type() const
{
	return ChatTypeManager::instance()->chatType("SimpleChat");
}

BuddySet SimpleChat::buddies() const
{
	if (CurrentContact.isNull())
		return BuddySet();
	return BuddySet(CurrentContact.ownerBuddy());
}

QString SimpleChat::name() const
{
	if (CurrentContact.isNull())
		return QString::null;
	return CurrentContact.ownerBuddy().display();
}
