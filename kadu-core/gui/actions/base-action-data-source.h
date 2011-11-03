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

#ifndef BASE_ACTION_DATA_SOURCE_H
#define BASE_ACTION_DATA_SOURCE_H

#include "buddies/buddy-set.h"
#include "chat/chat.h"
#include "contacts/contact-set.h"

#include "gui/actions/action-data-source.h"

class BaseActionDataSource : public ActionDataSource
{
	ContactSet Contacts;
	BuddySet Buddies;
	Chat CurrentChat;
	StatusContainer *CurrentStatusContainer;
	bool HasContactSelected;

public:
	BaseActionDataSource();
	virtual ~BaseActionDataSource();

	virtual ContactSet contacts();
	void setContacts(const ContactSet &contacts);

	virtual BuddySet buddies();
	void setBuddies(const BuddySet &buddies);

	virtual Chat chat();
	void setChat(const Chat &chat);

	virtual StatusContainer * statusContainer();
	void setStatusContainer(StatusContainer *statusContainer);

	virtual bool hasContactSelected();
	void setHasContactSelected(bool hasContactSelected);

};

#endif // BASE_ACTION_DATA_SOURCE_H
