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

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Creates empty ChatDetailsSimple object.
 * @param chatData Chat object that will be decribed by this object
 *
 * Creates empty ChatDetailsSimple object assigned to chatData object.
 */
ChatDetailsSimple::ChatDetailsSimple(ChatShared *chatData) :
		ChatDetails(chatData)
{
}

ChatDetailsSimple::~ChatDetailsSimple()
{
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Loads ChatDetailsSimple object from storage.
 *
 * Loads ChatDetailsSimple object from the same storage assigned Chat object is
 * using. The only data loaded is assigned contact.
 */
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

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Stored ChatDetailsSimple object to storage.
 *
 * Stored ChatDetailsSimple object to the same storage assigned Chat object is
 * using. The only data stored is assigned contact.
 */
void ChatDetailsSimple::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	if (!CurrentContact.isNull())
		storeValue("Contact", CurrentContact.uuid().toString());
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns true if assigned contact is valid.
 * @return true if assigned contact is valid
 *
 * Returns true if assigned contact is valid. No invalid chats (without contacts)
 * will be stored thanks to this method.
 */
bool ChatDetailsSimple::shouldStore()
{
	return StorableObject::shouldStore()
			&& !CurrentContact.uuid().isNull();
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns true if assigned contact is valid.
 * @return true if assigned contact is valid
 *
 * Returns true if assigned contact is valid. No invalid chats (without contacts)
 * will be stored thanks to this method.
 */
ChatType * ChatDetailsSimple::type() const
{
	return ChatTypeManager::instance()->chatType("Simple");
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns set of contacts of this chat (just assigned contact).
 * @return set of contacts of this chat (just assigned contact)
 *
 * Returns set of contacts of this chat (just assigned contact or empty set if
 * contact is not vaid).
 */
ContactSet ChatDetailsSimple::contacts() const
{
	if (CurrentContact.isNull())
		return ContactSet();
	return ContactSet(CurrentContact);
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns name of this chat.
 * @return name of this chat
 *
 * Returns name of this chat (which is display name of assigend contact buddy).
 */
QString ChatDetailsSimple::name() const
{
	if (CurrentContact.isNull())
		return QString::null;
	return BuddyManager::instance()->byContact(CurrentContact, ActionCreateAndAdd).display();
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Assigns contact to this chat.
 * @param contact assigned contact
 *
 * Assigns contact to this chat.
 */
void ChatDetailsSimple::setContact(Contact contact)
{
	CurrentContact = contact;
}
