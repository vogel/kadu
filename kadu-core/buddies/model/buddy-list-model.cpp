 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"
#include "accounts/account-manager.h"

#include "buddies/buddy.h"
#include "buddies/buddy-list-mime-data-helper.h"
#include "buddies/account-data/contact-account-data.h"

#include "protocols/protocol.h"

#include "buddy-list-model.h"

BuddyListModel::BuddyListModel(BuddyList list, QObject *parent)
	: BuddiesModelBase(parent), List(list)
{
	triggerAllAccountsRegistered();
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

const QModelIndex BuddyListModel::buddyIndex(Buddy buddy) const
{
	return index(List.indexOf(buddy), 0);
}
