/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef MODEL_INDEX_LIST_CONVERTER_H
#define MODEL_INDEX_LIST_CONVERTER_H

#include <QtCore/QModelIndexList>

#include "buddies/buddy-set.h"
#include "chat/chat.h"
#include "contacts/contact-set.h"
#include "model/roles.h"

class ModelIndexListConverter
{
	const QModelIndexList &ModelIndexList;

	RoleSet Roles;
	BuddySet Buddies;
	ContactSet Contacts;
	Chat ComputedChat;

	void buildRoles();
	void buildBuddies();
	void buildContacts();
	void buildChat();

	Chat chatFromIndex(const QModelIndex &index) const;
	Chat chatFromBuddies() const;
	Chat chatFromContacts(const Account &account) const;

	Account commonAccount() const;
	Contact contactForAccount(const QModelIndex &inde, const Account &account) const;

public:
	explicit ModelIndexListConverter(const QModelIndexList &modelIndexList);

	RoleSet roles() const;
	BuddySet buddies() const;
	ContactSet contacts() const;
	Chat chat() const;

};

#endif // MODEL_INDEX_LIST_CONVERTER_H
