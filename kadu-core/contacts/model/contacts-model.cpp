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

ContactsModel::ContactsModel(QObject *parent)
	: QAbstractListModel(parent)
{
	connect(ContactManager::instance(), SIGNAL(contactAboutToBeAdded(Contact &contact)),
			this, SLOT(contactAboutToBeAdded(Contact &)));
	connect(ContactManager::instance(), SIGNAL(contactAdded(Contact &)),
			this, SLOT(contactAdded(Contact &)));
	connect(ContactManager::instance(), SIGNAL(contactAboutToBeRemoved(Contact &)),
			this, SLOT(contactAboutToBeRemoved(Contact &)));
	connect(ContactManager::instance(), SIGNAL(contactRemoved(Contact &)),
			this, SLOT(contactRemoved(Contact &)));
}

ContactsModel::~ContactsModel()
{
	disconnect(ContactManager::instance(), SIGNAL(contactAboutToBeAdded(Contact &contact)),
			this, SLOT(contactAboutToBeAdded(Contact &)));
	disconnect(ContactManager::instance(), SIGNAL(contactAdded(Contact &)),
			this, SLOT(contactAdded(Contact &)));
	disconnect(ContactManager::instance(), SIGNAL(contactAboutToBeRemoved(Contact &)),
			this, SLOT(contactAboutToBeRemoved(Contact &)));
	disconnect(ContactManager::instance(), SIGNAL(contactRemoved(Contact &)),
			this, SLOT(contactRemoved(Contact &)));
}

int ContactsModel::rowCount(const QModelIndex &parent) const
{
	return ContactManager::instance()->contacts().count();
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

	if (index.row() >= rowCount())
		return Contact::null;

	return ContactManager::instance()->contacts().at(index.row());
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
