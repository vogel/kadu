/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "accounts/account-manager.h"
#include "accounts/account.h"

#include "buddies/buddy-list-mime-data-helper.h"
#include "buddies/buddy.h"
#include "contacts/contact.h"

#include "protocols/protocol.h"

#include "buddy-list-model.h"

BuddyListModel::BuddyListModel(QObject *parent) :
		BuddiesModelBase(parent)
{
	triggerAllAccountsRegistered();
}

BuddyListModel::~BuddyListModel()
{
}

void BuddyListModel::setBuddyList(const BuddyList &list)
{
	beginResetModel();
	List = list;
	endResetModel();
}

int BuddyListModel::rowCount(const QModelIndex &parent) const
{
	return parent.isValid()
		? BuddiesModelBase::rowCount(parent)
		: List.count();
}

int BuddyListModel::buddyIndex(const Buddy &buddy) const
{
	return List.indexOf(buddy);
}

Buddy BuddyListModel::buddyAt(int index) const
{
	if (index >= 0 && index < List.size())
		return List.at(index);
	else
		return Buddy::null;
}
