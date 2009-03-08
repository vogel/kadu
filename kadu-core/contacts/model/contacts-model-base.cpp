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

#include "contacts-model-base.h"

ContactsModelBase::ContactsModelBase(QObject *parent)
	: QAbstractListModel(parent)
{
}

ContactsModelBase::~ContactsModelBase()
{
	triggerAllAccountsUnregistered();
}

void ContactsModelBase::accountRegistered(Account *account)
{
	connect(account, SIGNAL(contactStatusChanged(Account *, Contact, Status)),
			this, SLOT(contactStatusChanged(Account *, Contact, Status)));
}

void ContactsModelBase::accountUnregistered(Account *account)
{
	disconnect(account, SIGNAL(contactStatusChanged(Account *, Contact, Status)),
			this, SLOT(contactStatusChanged(Account *, Contact, Status)));
}

void ContactsModelBase::contactStatusChanged(Account *account, Contact contact, Status oldStatus)
{
	QModelIndex index = contactIndex(contact);

	if (index.isValid())
		emit dataChanged(index, index);
}

QFlags<Qt::ItemFlag> ContactsModelBase::flags(const QModelIndex& index) const
{
	if (index.isValid())
		return QAbstractItemModel::flags(index) | Qt::ItemIsDragEnabled;
	else
		return QAbstractItemModel::flags(index);
}

QVariant ContactsModelBase::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal)
		return QString("Column %1").arg(section);
	else
		return QString("Row %1").arg(section);
}

QVariant ContactsModelBase::data(const QModelIndex &index, int role) const
{
	Contact con = contact(index);
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

// D&D

QStringList ContactsModelBase::mimeTypes() const
{
	return ContactListMimeDataHelper::mimeTypes();
}

QMimeData * ContactsModelBase::mimeData(const QModelIndexList & indexes) const
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

