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

#include "base-action-data-source.h"

BaseActionDataSource::BaseActionDataSource() :
		ChangedSignalBlocked(false), Changed(false), CurrentStatusContainer(0)
{
}

BaseActionDataSource::~BaseActionDataSource()
{
}

void BaseActionDataSource::blockChangedSignal()
{
	ChangedSignalBlocked = true;
}

void BaseActionDataSource::unblockChangedSignal()
{
	if (Changed)
		emit changed();

	ChangedSignalBlocked = false;
	Changed = false;
}

void BaseActionDataSource::dataUpdated()
{
	if (ChangedSignalBlocked)
		Changed = true;
	else
		emit changed();
}

ContactSet BaseActionDataSource::contacts()
{
	return Contacts;
}

void BaseActionDataSource::setContacts(const ContactSet &contacts)
{
	if (Contacts != contacts)
	{
		Contacts = contacts;
		dataUpdated();
	}
}

BuddySet BaseActionDataSource::buddies()
{
	return Buddies;
}

void BaseActionDataSource::setBuddies(const BuddySet &buddies)
{
	if (Buddies != buddies)
	{
		Buddies = buddies;
		dataUpdated();
	}
}

Chat BaseActionDataSource::chat()
{
	return CurrentChat;
}

void BaseActionDataSource::setChat(const Chat &chat)
{
	if (CurrentChat != chat)
	{
		CurrentChat = chat;
		dataUpdated();
	}
}

StatusContainer * BaseActionDataSource::statusContainer()
{
	return CurrentStatusContainer;
}

void BaseActionDataSource::setStatusContainer(StatusContainer *statusContainer)
{
	if (CurrentStatusContainer != statusContainer)
	{
		CurrentStatusContainer = statusContainer;
		dataUpdated();
	}
}

RoleSet BaseActionDataSource::roles()
{
	return Roles;
}

void BaseActionDataSource::setRoles(const RoleSet &roles)
{
	if (Roles != roles)
	{
		Roles = roles;
		dataUpdated();
	}
}
