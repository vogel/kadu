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

ContactsModel::ContactsModel(ContactManager *manager, QObject *parent)
	: ContactsModelBase(parent), Manager(manager)
{
	triggerAllAccountsRegistered();

	connect(Manager, SIGNAL(contactAboutToBeAdded(Contact &)),
			this, SLOT(contactAboutToBeAdded(Contact &)));
	connect(Manager, SIGNAL(contactAdded(Contact &)),
			this, SLOT(contactAdded(Contact &)));
	connect(Manager, SIGNAL(contactAboutToBeRemoved(Contact &)),
			this, SLOT(contactAboutToBeRemoved(Contact &)));
	connect(Manager, SIGNAL(contactRemoved(Contact &)),
			this, SLOT(contactRemoved(Contact &)));
}

ContactsModel::~ContactsModel()
{
	triggerAllAccountsUnregistered();

	disconnect(Manager, SIGNAL(contactAboutToBeAdded(Contact &)),
			this, SLOT(contactAboutToBeAdded(Contact &)));
	disconnect(Manager, SIGNAL(contactAdded(Contact &)),
			this, SLOT(contactAdded(Contact &)));
	disconnect(Manager, SIGNAL(contactAboutToBeRemoved(Contact &)),
			this, SLOT(contactAboutToBeRemoved(Contact &)));
	disconnect(Manager, SIGNAL(contactRemoved(Contact &)),
			this, SLOT(contactRemoved(Contact &)));
}

int ContactsModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return ContactsModelBase::rowCount(parent);

	return Manager->count();
}

Contact ContactsModel::contact(const QModelIndex &index) const
{
	QModelIndex parent = index.parent();
	return Manager->byIndex(parent.isValid() ? parent.row() : index.row());
}

const QModelIndex ContactsModel::contactIndex(Contact contact) const
{
	return index(Manager->contactIndex(contact), 0);
}

void ContactsModel::contactAboutToBeAdded(Contact &contact)
{
	int count = rowCount();
	beginInsertRows(QModelIndex(), count, count);
}

void ContactsModel::contactAdded(Contact &contact)
{
	endInsertRows();
}

void ContactsModel::contactAboutToBeRemoved(Contact &contact)
{
    	int index = contactIndex(contact).row();
	beginRemoveRows(QModelIndex(), index, index);
}

void ContactsModel::contactRemoved(Contact &contact)
{
    	endRemoveRows();
}
