 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "chat/aggregate-chat.h"
#include "chat/chat.h"
#include "contacts/filter/contact-name-filter.h"
#include "contacts/contact-list.h"

#include "chat-name-filter.h"

ChatNameFilter::ChatNameFilter(QObject *parent) :
		ChatFilter(parent)
{
	Filter = new ContactNameFilter(this);
}

bool ChatNameFilter::acceptChat(Chat *chat)
{
	if (Name.isEmpty())
		return true;

	if (chat->name().contains(Name, Qt::CaseInsensitive))
		return true;

	AggregateChat *aggregate = dynamic_cast<AggregateChat *>(chat);
	if (aggregate)
	{
		foreach (Chat *subchat, aggregate->chats())
			if (acceptChat(subchat))
				return true;
		return false;
	}

	ContactList contacts = chat->contacts().toContactList();
	foreach (Contact contact, contacts)
		if (Filter->acceptContact(contact))
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
