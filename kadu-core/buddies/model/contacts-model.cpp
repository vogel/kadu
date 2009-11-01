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
#include "buddies/account-data/contact-account-data.h"

#include "protocols/protocol.h"

#include "contacts-model.h"

ContactsModel::ContactsModel(BuddyManager *manager, QObject *parent)
	: ContactsModelBase(parent), Manager(manager)
{
	triggerAllAccountsRegistered();

	connect(Manager, SIGNAL(buddyAboutToBeAdded(Buddy &)),
			this, SLOT(contactAboutToBeAdded(Buddy &)));
	connect(Manager, SIGNAL(buddyAdded(Buddy &)),
			this, SLOT(contactAdded(Buddy &)));
	connect(Manager, SIGNAL(buddyAboutToBeRemoved(Buddy &)),
			this, SLOT(contactAboutToBeRemoved(Buddy &)));
	connect(Manager, SIGNAL(contactRemoved(Buddy &)),
			this, SLOT(contactRemoved(Buddy &)));
}

ContactsModel::~ContactsModel()
{
	triggerAllAccountsUnregistered();

	disconnect(Manager, SIGNAL(buddyAboutToBeAdded(Buddy &)),
			this, SLOT(contactAboutToBeAdded(Buddy &)));
	disconnect(Manager, SIGNAL(buddyAdded(Buddy &)),
			this, SLOT(contactAdded(Buddy &)));
	disconnect(Manager, SIGNAL(buddyAboutToBeRemoved(Buddy &)),
			this, SLOT(contactAboutToBeRemoved(Buddy &)));
	disconnect(Manager, SIGNAL(contactRemoved(Buddy &)),
			this, SLOT(contactRemoved(Buddy &)));
}

int ContactsModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return ContactsModelBase::rowCount(parent);

	return Manager->count();
}

Buddy ContactsModel::contact(const QModelIndex &index) const
{
	QModelIndex parent = index.parent();
	return Manager->byIndex(parent.isValid() ? parent.row() : index.row());
}

const QModelIndex ContactsModel::contactIndex(Buddy contact) const
{
	return index(Manager->contactIndex(contact), 0);
}

void ContactsModel::contactAboutToBeAdded(Buddy &contact)
{
	int count = rowCount();
	beginInsertRows(QModelIndex(), count, count);
}

void ContactsModel::contactAdded(Buddy &contact)
{
	endInsertRows();
}

void ContactsModel::contactAboutToBeRemoved(Buddy &contact)
{
    	int index = contactIndex(contact).row();
	beginRemoveRows(QModelIndex(), index, index);
}

void ContactsModel::contactRemoved(Buddy &contact)
{
    	endRemoveRows();
}
