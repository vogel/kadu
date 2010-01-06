 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "chat/chat.h"
#include "buddies/filter/buddy-name-filter.h"
#include "buddies/buddy-list.h"
#include "contacts/contact-set.h"

#include "chat-name-filter.h"

ChatNameFilter::ChatNameFilter(QObject *parent) :
		ChatFilter(parent)
{
	Filter = new BuddyNameFilter(this);
}

bool ChatNameFilter::acceptChat(Chat chat)
{
	if (Name.isEmpty())
		return true;

	if (chat.name().contains(Name, Qt::CaseInsensitive))
		return true;
/*
	AggregateChat aggregate = dynamic_cast<AggregateChat>(chat);
	if (aggregate)
	{
		foreach (Chat subchat, aggregate->chats())
			if (acceptChat(subchat))
				return true;
		return false;
	}*/

	foreach (const Buddy &buddy, chat.contacts().toBuddySet())
		if (Filter->acceptBuddy(buddy))
			return true;

	return false;
}

void ChatNameFilter::setName(const QString& name)
{
	if (name != Name)
	{
		Name = name;
		Filter->setName(Name);
		emit filterChanged();
	}
}
