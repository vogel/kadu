/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "contacts/contact-set-configuration-helper.h"
#include "chat/type/chat-type-manager.h"
#include "chat/chat.h"

#include "chat-details-conference.h"

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Creates empty ChatDetailsConference object.
 * @param chatData Chat object that will be decribed by this object
 *
 * Creates empty ChatDetailsConference object assigned to chatData object.
 */
ChatDetailsConference::ChatDetailsConference(ChatShared *chatData) :
		ChatDetails(chatData)
{
}

ChatDetailsConference::~ChatDetailsConference()
{
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Loads ChatDetailsConference object from storage.
 *
 * Loads ChatDetailsConference object from the same storage assigned Chat object is
 * using. This loads set of contacts from 'Contacts' subnode.
 */
void ChatDetailsConference::load()
{
	if (!isValidStorage())
		return;

	ChatDetails::load();

	Contacts = ContactSetConfigurationHelper::loadFromConfiguration(this, "Contacts");
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Stores ChatDetailsConference object to storage.
 *
 * Stores ChatDetailsConference object to the same storage assigned Chat object is
 * using. This stores set of contacts into 'Contacts' subnode.
 */
void ChatDetailsConference::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	ContactSetConfigurationHelper::saveToConfiguration(this, "Contacts", Contacts);
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns true if assigned contact is valid.
 * @return true if assigned set of contact is not valid
 *
 * Returns true if assigned set of contacts is not empty. No empty chats (without contacts)
 * will be stored thanks to this method.
 */
bool ChatDetailsConference::shouldStore()
{
	return StorableObject::shouldStore() && !Contacts.isEmpty();
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns type of this chat - 'Conference'.
 * @return 'Conference' ChatType object
 *
 * Returns type of this chat - 'Conference'.
 */
ChatType * ChatDetailsConference::type() const
{
	return ChatTypeManager::instance()->chatType("Conference");
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns name of this chat.
 * @return name of this chat
 *
 * Returns name of this chat (which is display names of assigend contacts buddies
 * joined by commas).
 */
QString ChatDetailsConference::name() const
{
	QStringList displays;
	foreach (const Contact &contact, Contacts.toContactList())
		displays.append(BuddyManager::instance()->byContact(contact, ActionCreateAndAdd).display());

	displays.sort();
	return displays.join(", ");
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Assigns contact set to this chat.
 * @param contacts contact set to assign
 *
 * Assigns contact set to this chat.
 */
void ChatDetailsConference::setContacts(const ContactSet &contacts)
{
	ensureLoaded();

	Contacts = contacts;
}
