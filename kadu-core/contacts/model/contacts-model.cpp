 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"
#include "accounts/account_manager.h"

#include "contacts/contact.h"
#include "contacts/contact-account-data.h"
#include "contacts/contact-manager.h"

#include "protocols/protocol.h"

#include "contacts-model.h"

ContactsModel::ContactsModel(ContactManager *manager, QObject *parent)
	: QAbstractListModel(parent), Manager(manager)
{
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
		return 0;

	return Manager->count();
}

QVariant ContactsModel::data(const QModelIndex &index, int role) const
{
	Contact con = contact(index);
	if (con.isNull())
		return QVariant();

	ContactAccountData *cad;
	Account *account = AccountManager::instance()->defaultAccount();

	switch (role)
	{
		case Qt::DisplayRole:
			return con.display();
		case Qt::DecorationRole:
			cad = con.accountData(account);
			if (0 == cad)
				return QVariant();
			return account->protocol()->statusPixmap(cad->status());
		default:
			return QVariant();
	}
}

QVariant ContactsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal)
		return QString("Column %1").arg(section);
	else
		return QString("Row %1").arg(section);
}

Contact ContactsModel::contact(const QModelIndex &index) const
{
	if (!index.isValid())
		return Contact::null;

	if (index.row() < 0 || index.row() >= rowCount())
		return Contact::null;

	return Manager->byIndex(index.row());
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
}

void ContactsModel::contactRemoved(Contact &contact)
{
}
