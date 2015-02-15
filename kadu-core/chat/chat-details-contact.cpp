/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "chat/chat.h"
#include "chat/type/chat-type-manager.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "protocols/protocol.h"

#include "chat-details-contact.h"

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Creates empty ChatDetailsContact object.
 * @param chatData Chat object that will be decribed by this object
 *
 * Creates empty ChatDetailsContact object assigned to chatData object.
 */
ChatDetailsContact::ChatDetailsContact(ChatShared *chatData) :
		ChatDetails(chatData)
{
	Protocol *protocol = mainData()->chatAccount().protocolHandler();

	if (protocol)
	{
		connect(protocol, SIGNAL(connected(Account)), this, SIGNAL(connected()));
		connect(protocol, SIGNAL(disconnected(Account)), this, SIGNAL(disconnected()));
	}
}

ChatDetailsContact::~ChatDetailsContact()
{
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Loads ChatDetailsContact object from storage.
 *
 * Loads ChatDetailsContact object from the same storage assigned Chat object is
 * using. The only data loaded is assigned contact.
 */
void ChatDetailsContact::load()
{
	if (!isValidStorage())
		return;

	ChatDetails::load();

	QString cadUuid = loadValue<QString>("Contact");

	CurrentContact = ContactManager::instance()->byUuid(cadUuid);
	if (!CurrentContact)
	{
		// import from old 0.6.6 releases
		Buddy buddy = BuddyManager::instance()->byUuid(cadUuid);
		if (buddy)
		{
			QVector<Contact> contactList = buddy.contacts(mainData()->chatAccount());
			if (!contactList.isEmpty())
				CurrentContact = contactList.at(0);
		}
	}
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Stores ChatDetailsContact object to storage.
 *
 * Stores ChatDetailsContact object to the same storage assigned Chat object is
 * using. The only data stored is assigned contact.
 */
void ChatDetailsContact::store()
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
bool ChatDetailsContact::shouldStore()
{
	ensureLoaded();

	return StorableObject::shouldStore()
			&& !CurrentContact.uuid().isNull();
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns type of this chat - 'Contact'.
 * @return 'Contact' ChatType object
 *
 * Returns type of this chat - 'Contact'.
 */
ChatType * ChatDetailsContact::type() const
{
	return ChatTypeManager::instance()->chatType("Contact");
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns set of contacts of this chat (just assigned contact).
 * @return set of contacts of this chat (just assigned contact)
 *
 * Returns set of contacts of this chat (just assigned contact or empty set if
 * contact is not vaid).
 */
ContactSet ChatDetailsContact::contacts() const
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
QString ChatDetailsContact::name() const
{
	return CurrentContact.display(true);
}

bool ChatDetailsContact::isConnected() const
{
	return mainData()->chatAccount().protocolHandler() && mainData()->chatAccount().protocolHandler()->isConnected();
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Assigns contact to this chat.
 * @param contact assigned contact
 *
 * Assigns contact to this chat.
 */
void ChatDetailsContact::setContact(const Contact &contact)
{
	CurrentContact = contact;
}

Contact ChatDetailsContact::contact()
{
	return CurrentContact;
}

#include "moc_chat-details-contact.cpp"
