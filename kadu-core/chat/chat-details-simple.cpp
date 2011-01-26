/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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
		CurrentContact = ContactManager::instance()->byUuid(cadUuid);
		if (CurrentContact.isNull())
		{
			// TODO 0.6.6 how it works?
			Buddy buddy = BuddyManager::instance()->byUuid(cadUuid);
			QList<Contact> contactList = buddy.contacts(mainData()->chatAccount());
			if (!contactList.isEmpty())
				CurrentContact = contactList.at(0);
		}
	}
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Stores ChatDetailsSimple object to storage.
 *
 * Stores ChatDetailsSimple object to the same storage assigned Chat object is
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
 * @short Returns type of this chat - 'Simple'.
 * @return 'Simple' ChatType object
 *
 * Returns type of this chat - 'Simple'.
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
		return QString();
	return BuddyManager::instance()->byContact(CurrentContact, ActionCreateAndAdd).display();
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Assigns contact to this chat.
 * @param contact assigned contact
 *
 * Assigns contact to this chat.
 */
void ChatDetailsSimple::setContact(const Contact &contact)
{
	CurrentContact = contact;
}
