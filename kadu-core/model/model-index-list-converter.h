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

#ifndef MODEL_INDEX_LIST_CONVERTER_H
#define MODEL_INDEX_LIST_CONVERTER_H

#include <QtCore/QModelIndexList>

#include "chat/chat.h"
#include "contacts/contact-set.h"
#include "buddies/buddy-set.h"

class ModelIndexListConverter
{
	const QModelIndexList &ModelIndexList;

	BuddySet ComputedBuddySet;
	ContactSet ComputedContactSet;
	Chat ComputedChat;

	void buildBuddySet();
	void buildContactSet();
	void buildChat();

	Chat chatByPendingMessages(const QModelIndex &index) const;
	Account commonAccount() const;
	Contact contactForAccount(const QModelIndex &inde, const Account &account) const;

public:
	explicit ModelIndexListConverter(const QModelIndexList &modelIndexList);

	BuddySet buddySet() const;
	ContactSet contactSet() const;
	Chat chat() const;

};

#endif // MODEL_INDEX_LIST_CONVERTER_H
