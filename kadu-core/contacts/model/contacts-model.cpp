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
#include "contacts/contact-list-mime-data-helper.h"
#include "contacts/contact-manager.h"

#include "protocols/protocol.h"

#include "contacts-model.h"

ContactsModel::ContactsModel(ContactManager *manager, QObject *parent)
	: QAbstractListModel(parent), Manager(manager)
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

void ContactsModel::accountRegistered(Account *account)
{
	connect(account, SIGNAL(contactStatusChanged(Account *, Contact, Status)),
			this, SLOT(contactStatusChanged(Account *, Contact, Status)));
}

void ContactsModel::accountUnregistered(Account *account)
{
	disconnect(account, SIGNAL(contactStatusChanged(Account *, Contact, Status)),
			this, SLOT(contactStatusChanged(Account *, Contact, Status)));
}

void ContactsModel::contactStatusChanged(Account *account, Contact contact, Status oldStatus)
{
	QModelIndex index = contactIndex(contact);

	if (index.isValid())
		emit dataChanged(index, index);
}

int ContactsModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;

	return Manager->count();
}

QFlags<Qt::ItemFlag> ContactsModel::flags(const QModelIndex& index) const
{
	if (index.isValid())
		return QAbstractItemModel::flags(index) | Qt::ItemIsDragEnabled;
	else
		return QAbstractItemModel::flags(index);
}

QVariant ContactsModel::data(const QModelIndex &index, int role) const
{
	Contact con = Manager->byIndex(index.row());
	if (con.isNull())
		return QVariant();

	ContactAccountData *cad;

	Account *account = con.prefferedAccount();
	if (!account)
		account = AccountManager::instance()->defaultAccount();

	switch (role)
	{
		case Qt::DisplayRole:
			return con.display();
		case Qt::DecorationRole:
			cad = con.accountData(account);
			if (0 == cad)
				return QVariant();
			// TODO generic icon
			return account && account->protocol()
				? account->protocol()->statusPixmap(cad->status())
				: QVariant();
		case ContactRole:
			return QVariant::fromValue(con);
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

// D&D

QStringList ContactsModel::mimeTypes() const
{
	return ContactListMimeDataHelper::mimeTypes();
}

QMimeData * ContactsModel::mimeData(const QList<QModelIndex> indexes) const
{
	ContactList list;
	foreach (QModelIndex index, indexes)
	{
		QVariant conVariant = index.data(ContactRole);;
		if (!conVariant.canConvert<Contact>())
			continue;
		Contact con = conVariant.value<Contact>();
		if (con.isNull())
			continue;
		list << con;
	}

	return ContactListMimeDataHelper::toMimeData(list);
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
