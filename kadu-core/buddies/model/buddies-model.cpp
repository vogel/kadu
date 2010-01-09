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
#include "buddies/buddy-manager.h"
#include "contacts/contact.h"

#include "protocols/protocol.h"

#include "buddies-model.h"

BuddiesModel::BuddiesModel(BuddyManager *manager, QObject *parent)
	: BuddiesModelBase(parent), Manager(manager)
{
	triggerAllAccountsRegistered();

	connect(Manager, SIGNAL(buddyAboutToBeAdded(Buddy &)),
			this, SLOT(buddyAboutToBeAdded(Buddy &)));
	connect(Manager, SIGNAL(buddyAdded(Buddy &)),
			this, SLOT(buddyAdded(Buddy &)));
	connect(Manager, SIGNAL(buddyAboutToBeRemoved(Buddy &)),
			this, SLOT(buddyAboutToBeRemoved(Buddy &)));
			connect(Manager, SIGNAL(buddyRemoved(Buddy &)),
			this, SLOT(buddyRemoved(Buddy &)));
}

BuddiesModel::~BuddiesModel()
{
	triggerAllAccountsUnregistered();

	disconnect(Manager, SIGNAL(buddyAboutToBeAdded(Buddy &)),
			this, SLOT(buddyAboutToBeAdded(Buddy &)));
	disconnect(Manager, SIGNAL(buddyAdded(Buddy &)),
			this, SLOT(buddyAdded(Buddy &)));
	disconnect(Manager, SIGNAL(buddyAboutToBeRemoved(Buddy &)),
			this, SLOT(buddyAboutToBeRemoved(Buddy &)));
	disconnect(Manager, SIGNAL(buddyRemoved(Buddy &)),
			this, SLOT(buddyRemoved(Buddy &)));
}

int BuddiesModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return BuddiesModelBase::rowCount(parent);

	return Manager->count();
}

Buddy BuddiesModel::buddyAt(const QModelIndex &index) const
{
	QModelIndex parent = index.parent();
	return Manager->byIndex(parent.isValid() ? parent.row() : index.row());
}

const QModelIndex BuddiesModel::buddyIndex(Buddy buddy) const
{
	return index(Manager->indexOf(buddy), 0);
}

void BuddiesModel::buddyAboutToBeAdded(Buddy &buddy)
{
	int count = rowCount();
	beginInsertRows(QModelIndex(), count, count);
}

void BuddiesModel::buddyAdded(Buddy &buddy)
{
	endInsertRows();
}

void BuddiesModel::buddyAboutToBeRemoved(Buddy &buddy)
{
	int index = buddyIndex(buddy).row();
	beginRemoveRows(QModelIndex(), index, index);
}

void BuddiesModel::buddyRemoved(Buddy &buddy)
{
	endRemoveRows();
}
