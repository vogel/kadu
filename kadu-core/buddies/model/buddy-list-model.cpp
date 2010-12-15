/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account.h"
#include "accounts/account-manager.h"

#include "buddies/buddy.h"
#include "buddies/buddy-list-mime-data-helper.h"
#include "contacts/contact.h"

#include "protocols/protocol.h"

#include "buddy-list-model.h"

BuddyListModel::BuddyListModel(const BuddyList &list, QObject *parent) :
		BuddiesModelBase(parent), List(list)
{
	triggerAllAccountsRegistered();
}

BuddyListModel::~BuddyListModel()
{
}

int BuddyListModel::rowCount(const QModelIndex &parent) const
{
	return parent.isValid()
		? BuddiesModelBase::rowCount(parent)
		: List.count();
}

Buddy BuddyListModel::buddyAt(const QModelIndex &index) const
{
	QModelIndex parent = index.parent();
	return parent.isValid()
		? List.at(parent.row())
		: index.isValid()
			? List.at(index.row())
			: Buddy::null;
}

QModelIndex BuddyListModel::indexForValue(const QVariant &value) const
{
	return index(List.indexOf(value.value<Buddy>()), 0);
}
