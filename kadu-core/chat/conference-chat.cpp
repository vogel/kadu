/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "chat/type/chat-type-manager.h"
#include "contacts/contact-set-configuration-helper.h"

#include "conference-chat.h"

ConferenceChat::ConferenceChat(StoragePoint *storage) :
		Chat(storage)
{
}

ConferenceChat::ConferenceChat(Account *currentAccount, ContactSet contacts, QUuid uuid) :
		Chat(currentAccount, uuid), CurrentContacts(contacts)
{
}

ConferenceChat::~ConferenceChat()
{
}

void ConferenceChat::load()
{
	if (!isValidStorage())
		return;

	Chat::load();
	CurrentContacts = ContactSetConfigurationHelper::loadFromConfiguration(this, "Contacts");
	refreshTitle();
}

void ConferenceChat::store()
{
	if (!isValidStorage())
		return;

	Chat::store();
	storeValue("Type", "Conference");
	ContactSetConfigurationHelper::saveToConfiguration(this, "Contacts", CurrentContacts);
}

ChatType ConferenceChat::type() const
{
	return ChatTypeManager::instance()->chatType("ConferenceChat");
}

QString ConferenceChat::name() const
{
	QStringList displays;
	foreach (Contact contact, CurrentContacts)
		displays.append(contact.display());

	displays.sort();
	return displays.join(", ");
}
