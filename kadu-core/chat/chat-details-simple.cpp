/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "buddies/buddy-manager.h"
#include "buddies/buddy-set.h"
#include "buddies/buddy-shared.h"
#include "chat/type/chat-type-manager.h"
#include "chat/chat.h"
#include "chat/chat-shared.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"

#include "chat-details-simple.h"

ChatDetailsSimple::ChatDetailsSimple(ChatShared *chatData) :
		ChatDetails(chatData), CurrentContact(Contact::null)
{
}

ChatDetailsSimple::~ChatDetailsSimple()
{
}

void ChatDetailsSimple::load()
{
	if (!isValidStorage())
		return;

	ChatDetails::load();

	QString cadUuid = loadValue<QString>("Contact");
	if (cadUuid.isNull())
	{
		CurrentContact = ContactManager::instance()->byUuid(loadValue<QString>("ContactAccountData"));
		removeValue("ContactAccountData");
	}
	else
	{
		CurrentContact = ContactManager::instance()->byUuid(cadUuid, false);
		if (CurrentContact.isNull())
		{
			// TODO 0.6.6 how it work?
			Buddy buddy = BuddyManager::instance()->byUuid(cadUuid);
			QList<Contact> contactList = buddy.contacts(mainData()->chatAccount());
			if (!contactList.isEmpty())
				CurrentContact = contactList[0];
		}
	}

	mainData()->refreshTitle();
}

void ChatDetailsSimple::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	if (!CurrentContact.isNull())
		storeValue("Contact", CurrentContact.uuid().toString());
}

ChatType * ChatDetailsSimple::type() const
{
	return ChatTypeManager::instance()->chatType("Simple");
}

ContactSet ChatDetailsSimple::contacts() const
{
	if (CurrentContact.isNull())
		return ContactSet();
	return ContactSet(CurrentContact);
}

QString ChatDetailsSimple::name() const
{
	if (CurrentContact.isNull())
		return QString::null;
	return CurrentContact.ownerBuddy().display();
}

void ChatDetailsSimple::setContact(Contact contact)
{
	CurrentContact = contact;
}
