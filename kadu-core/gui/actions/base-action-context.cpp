/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
		ChangedSignalBlocked(false), Changed(false), CurrentStatusContainer(0)
{
}

BaseActionContext::~BaseActionContext()
{
}

void BaseActionContext::blockChangedSignal()
{
	ChangedSignalBlocked = true;
}

void BaseActionContext::unblockChangedSignal()
{
	if (Changed)
		emit changed();

	ChangedSignalBlocked = false;
	Changed = false;
}

void BaseActionContext::dataUpdated()
{
	if (ChangedSignalBlocked)
		Changed = true;
	else
		emit changed();
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
		dataUpdated();
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
		dataUpdated();
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
		dataUpdated();
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
		dataUpdated();
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
		dataUpdated();
	}
}
