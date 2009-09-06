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
#include "contacts/avatar.h"
#include "contacts/avatar-manager.h"
#include "contacts/contact.h"
#include "contacts/contact-account-data.h"
#include "contacts/contact-list-mime-data-helper.h"
#include "contacts/contact-manager.h"
#include "protocols/protocol.h"

#include "contacts-model-base.h"

ContactsModelBase::ContactsModelBase(QObject *parent) :
		QAbstractItemModel(parent)
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

QModelIndex ContactsModelBase::index(int row, int column, const QModelIndex &parent) const
{
	return createIndex(row, column, parent.isValid() ? parent.row() : -1);
}

int ContactsModelBase::columnCount(const QModelIndex &parent) const
{
	return 1;
}

int ContactsModelBase::rowCount(const QModelIndex &parentIndex) const
{
	if (!parentIndex.isValid() || parent(parentIndex).isValid())
		return 0;

	Contact con = contact(parentIndex);
	return con.accountDatas().size();
}

QFlags<Qt::ItemFlag> ContactsModelBase::flags(const QModelIndex& index) const
{
	if (index.isValid())
		return QAbstractItemModel::flags(index) | Qt::ItemIsDragEnabled;
	else
		return QAbstractItemModel::flags(index);
}

QModelIndex ContactsModelBase::parent(const QModelIndex &child) const
{
	if (-1 == child.internalId())
		return QModelIndex();
	else
		return index(child.internalId(), 0, QModelIndex());
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

ContactAccountData * ContactsModelBase::contactDefaultAccountData(const QModelIndex &index) const
{
	Contact con = contact(index);
	if (con.isNull())
		return 0;

	Account *account = con.prefferedAccount();
	if (!account)
		account = AccountManager::instance()->defaultAccount();
	
	return con.accountData(account);
}

ContactAccountData * ContactsModelBase::contactAccountData(const QModelIndex &index, int accountIndex) const
{
	Contact con = contact(index);
	if (con.isNull())
		return 0;

	QList<ContactAccountData *> accountDatas = con.accountDatas();
	if (accountDatas.size() <= accountIndex)
		return 0;

	return accountDatas[accountIndex];
}

QVariant ContactsModelBase::data(ContactAccountData *cad, int role, bool useDisplay) const
{
	if (!cad)
		return QVariant();

	switch (role)
	{
		case Qt::DisplayRole:
			return useDisplay
				? cad->contact().display()
				: QString("%1: %2").arg(cad->account()->name()).arg(cad->id());
		case Qt::DecorationRole:
			if (0 == cad)
				return QVariant();
			// TODO generic icon
			return cad->account()
				? cad->account()->statusPixmap(cad->status())
				: QVariant();
		case ContactRole:
			return QVariant::fromValue(cad->contact());
		case DescriptionRole:
			//TODO 0.6.6:
			//	ContactKaduData *ckd = contact.moduleData<ContactKaduData>(true);
			//	if (!ckd)
			//		return QString::null;
			//	if (ckd->hideDescription())
			//	{
				//		delete ckd;
				//		return QString::null;
				//	}
				//	delete ckd;
				//
				return cad->status().description();
		case StatusRole:
			return QVariant::fromValue(cad->status());
		case AccountRole:
			return QVariant::fromValue(cad->account());
		case AvatarRole:
			// TODO: 0.6.6 move it
			if (cad->avatar().pixmap().isNull())
				AvatarManager::instance()->updateAvatar(cad);
			return QVariant::fromValue(cad->avatar().pixmap());
		default:
			return QVariant();
}
}

QVariant ContactsModelBase::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	QModelIndex parentIndex = parent(index);
	if (!parentIndex.isValid())
		return data(contactDefaultAccountData(index), role, true);
	else
		return data(contactAccountData(parentIndex, index.row()), role, false);
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
