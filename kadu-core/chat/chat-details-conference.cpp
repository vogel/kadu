/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "buddies/buddy-set-configuration-helper.h"
#include "chat/type/chat-type-manager.h"
#include "chat/chat.h"

#include "chat-details-conference.h"

ChatDetailsConference::ChatDetailsConference(Chat *chat) :
		ChatDetails(chat)
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

	Buddies = BuddySetConfigurationHelper::loadFromConfiguration(this, "Contacts");

	chat()->refreshTitle();
}

void ChatDetailsConference::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	storeValue("Type", "Conference");

	BuddySetConfigurationHelper::saveToConfiguration(this, "Contacts", Buddies);
}

ChatType * ChatDetailsConference::type() const
{
	return ChatTypeManager::instance()->chatType("ConferenceChat");
}

QString ChatDetailsConference::name() const
{
	QStringList displays;
	foreach (Buddy buddy, Buddies)
		displays.append(buddy.display());

	displays.sort();
	return displays.join(", ");
}

void ChatDetailsConference::setBuddies(BuddySet buddies)
{
	ensureLoaded();

	Buddies = buddies;
}
