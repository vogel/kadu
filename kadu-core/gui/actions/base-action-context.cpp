/*
 * %kadu copyright begin%
 * Copyright 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "base-action-context.h"

BaseActionContext::BaseActionContext() :
		CurrentStatusContainer(0)
{
	connect(&MyChangeNotifier, SIGNAL(changed()), this, SIGNAL(changed()));
}

BaseActionContext::~BaseActionContext()
{
}

ChangeNotifier & BaseActionContext::changeNotifier()
{
	return MyChangeNotifier;
}

ContactSet BaseActionContext::contacts()
{
	return Contacts;
}

void BaseActionContext::setContacts(const ContactSet &contacts)
{
	if (Contacts != contacts)
	{
		Contacts = contacts;
		MyChangeNotifier.notify();
	}
}

BuddySet BaseActionContext::buddies()
{
	return Buddies;
}

void BaseActionContext::setBuddies(const BuddySet &buddies)
{
	if (Buddies != buddies)
	{
		Buddies = buddies;
		MyChangeNotifier.notify();
	}
}

Chat BaseActionContext::chat()
{
	return CurrentChat;
}

void BaseActionContext::setChat(const Chat &chat)
{
	if (CurrentChat != chat)
	{
		CurrentChat = chat;
		MyChangeNotifier.notify();
	}
}

StatusContainer * BaseActionContext::statusContainer()
{
	return CurrentStatusContainer;
}

void BaseActionContext::setStatusContainer(StatusContainer *statusContainer)
{
	if (CurrentStatusContainer != statusContainer)
	{
		CurrentStatusContainer = statusContainer;
		MyChangeNotifier.notify();
	}
}

RoleSet BaseActionContext::roles()
{
	return Roles;
}

void BaseActionContext::setRoles(const RoleSet &roles)
{
	if (Roles != roles)
	{
		Roles = roles;
		MyChangeNotifier.notify();
	}
}

#include "moc_base-action-context.cpp"
