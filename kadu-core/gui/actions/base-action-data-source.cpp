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
		HasContactSelected(false)
{
}

BaseActionDataSource::~BaseActionDataSource()
{
}

ContactSet BaseActionDataSource::contacts()
{
	return Contacts;
}

void BaseActionDataSource::setContacts(const ContactSet &contacts)
{
	Contacts = contacts;
}

BuddySet BaseActionDataSource::buddies()
{
	return Buddies;
}

void BaseActionDataSource::setBuddies(const BuddySet &buddies)
{
	Buddies = buddies;
}

Chat BaseActionDataSource::chat()
{
	return CurrentChat;
}

void BaseActionDataSource::setChat(const Chat &chat)
{
	CurrentChat = chat;
}

StatusContainer * BaseActionDataSource::statusContainer()
{
	return CurrentStatusContainer;
}

void BaseActionDataSource::setStatusContainer(StatusContainer *statusContainer)
{
	CurrentStatusContainer = statusContainer;
}

bool BaseActionDataSource::hasContactSelected()
{
	return HasContactSelected;
}

void BaseActionDataSource::setHasContactSelected(bool hasContactSelected)
{
	HasContactSelected = hasContactSelected;
}
