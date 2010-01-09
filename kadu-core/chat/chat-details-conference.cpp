/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "buddies/buddy-manager.h"
#include "contacts/contact-set-configuration-helper.h"
#include "chat/type/chat-type-manager.h"
#include "chat/chat.h"

#include "chat-details-conference.h"

ChatDetailsConference::ChatDetailsConference(ChatShared *chatData) :
		ChatDetails(chatData)
{
}

ChatDetailsConference::~ChatDetailsConference()
{
}

void ChatDetailsConference::load()
{
	if (!isValidStorage())
		return;

	ChatDetails::load();

	Contacts = ContactSetConfigurationHelper::loadFromConfiguration(this, "Contacts", mainData()->chatAccount());

	mainData()->refreshTitle();
}

void ChatDetailsConference::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	ContactSetConfigurationHelper::saveToConfiguration(this, "Contacts", Contacts);
}

bool ChatDetailsConference::shouldStore()
{
	return StorableObject::shouldStore() && !Contacts.isEmpty();
}

ChatType * ChatDetailsConference::type() const
{
	return ChatTypeManager::instance()->chatType("Conference");
}

QString ChatDetailsConference::name() const
{
	QStringList displays;
	foreach (const Contact &contact, Contacts.toContactList())
		displays.append(BuddyManager::instance()->byContact(contact, ActionCreateAndAdd).display());

	displays.sort();
	return displays.join(", ");
}

void ChatDetailsConference::setContacts(ContactSet contacts)
{
	ensureLoaded();

	Contacts = contacts;
}
