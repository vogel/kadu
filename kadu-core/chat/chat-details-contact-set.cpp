/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "chat/chat.h"
#include "chat/type/chat-type-manager.h"
#include "contacts/contact-set-configuration-helper.h"
#include "protocols/protocol.h"

#include "chat-details-contact-set.h"

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Creates empty ChatDetailsContactSet object.
 * @param chatData Chat object that will be decribed by this object
 *
 * Creates empty ChatDetailsContactSet object assigned to chatData object.
 */
ChatDetailsContactSet::ChatDetailsContactSet(ChatShared *chatData) :
		ChatDetails(chatData)
{
	Protocol *protocol = mainData()->chatAccount().protocolHandler();

	if (protocol)
	{
		connect(protocol, SIGNAL(connected(Account)), this, SIGNAL(connected()));
		connect(protocol, SIGNAL(disconnected(Account)), this, SIGNAL(disconnected()));
	}
}

ChatDetailsContactSet::~ChatDetailsContactSet()
{
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Loads ChatDetailsContactSet object from storage.
 *
 * Loads ChatDetailsContactSet object from the same storage assigned Chat object is
 * using. This loads set of contacts from 'Contacts' subnode.
 */
void ChatDetailsContactSet::load()
{
	if (!isValidStorage())
		return;

	ChatDetails::load();

	Contacts = ContactSetConfigurationHelper::loadFromConfiguration(this, "Contacts");
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Stores ChatDetailsContactSet object to storage.
 *
 * Stores ChatDetailsContactSet object to the same storage assigned Chat object is
 * using. This stores set of contacts into 'Contacts' subnode.
 */
void ChatDetailsContactSet::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	ContactSetConfigurationHelper::saveToConfiguration(this, "Contacts", Contacts);
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns true if assigned set of contact is valid.
 * @return true if assigned set of contact is valid
 *
 * Returns true if assigned set of contacts is not empty. No empty chats (without contacts)
 * will be stored thanks to this method.
 */
bool ChatDetailsContactSet::shouldStore()
{
	ensureLoaded();

	return StorableObject::shouldStore() && !Contacts.isEmpty();
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns type of this chat - 'ContactSet'.
 * @return 'ContactSet' ChatType object
 *
 * Returns type of this chat - 'ContactSet'.
 */
ChatType * ChatDetailsContactSet::type() const
{
	return ChatTypeManager::instance()->chatType("ContactSet");
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns name of this chat.
 * @return name of this chat
 *
 * Returns name of this chat (which is display names of assigend contacts buddies
 * joined by commas).
 */
QString ChatDetailsContactSet::name() const
{
	QStringList displays;
	foreach (const Contact &contact, Contacts)
		displays.append(contact.display(true));

	displays.sort();
	return displays.join(", ");
}

bool ChatDetailsContactSet::isConnected() const
{
	return mainData()->chatAccount().protocolHandler() && mainData()->chatAccount().protocolHandler()->isConnected();
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Assigns contact set to this chat.
 * @param contacts contact set to assign
 *
 * Assigns contact set to this chat.
 */
void ChatDetailsContactSet::setContacts(const ContactSet &contacts)
{
	ensureLoaded();

	Contacts = contacts;
}
