 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"
#include "contacts/account-data/contact-account-data.h"
#include "contacts/account-data/contact-account-data-manager.h"
#include "model/roles.h"
#include "protocols/protocol.h"

#include "contact-account-data-model.h"

ContactAccountDataModel::ContactAccountDataModel(Contact contact, QObject *parent)
	: QAbstractListModel(parent), SourceContact(contact)
{
	connect(ContactAccountDataManager::instance(), SIGNAL(contactAccountDataAboutToBeAdded(ContactAccountData *)),
			this, SLOT(contactAccountDataAboutToBeAdded(ContactAccountData *)));
	connect(ContactAccountDataManager::instance(), SIGNAL(contactAccountDataAdded(ContactAccountData *)),
			this, SLOT(contactAccountDataAdded(ContactAccountData *)));
	connect(ContactAccountDataManager::instance(), SIGNAL(contactAccountDataAboutToBeRemoved(ContactAccountData *)),
			this, SLOT(contactAccountDataAboutToBeRemoved(ContactAccountData *)));
	connect(ContactAccountDataManager::instance(), SIGNAL(contactAccountDataRemoved(ContactAccountData *)),
			this, SLOT(contactAccountDataRemoved(ContactAccountData *)));
}

ContactAccountDataModel::~ContactAccountDataModel()
{
	disconnect(ContactAccountDataManager::instance(), SIGNAL(contactAccountDataAboutToBeAdded(ContactAccountData *)),
			this, SLOT(contactAccountDataAboutToBeAdded(ContactAccountData *)));
	disconnect(ContactAccountDataManager::instance(), SIGNAL(contactAccountDataAdded(ContactAccountData *)),
			this, SLOT(contactAccountDataAdded(ContactAccountData *)));
	disconnect(ContactAccountDataManager::instance(), SIGNAL(contactAccountDataAboutToBeRemoved(ContactAccountData *)),
			this, SLOT(contactAccountDataAboutToBeRemoved(ContactAccountData *)));
	disconnect(ContactAccountDataManager::instance(), SIGNAL(contactAccountDataRemoved(ContactAccountData *)),
			this, SLOT(contactAccountDataRemoved(ContactAccountData *)));
}

int ContactAccountDataModel::columnCount(const QModelIndex &parent) const
{
	return 2;
}

int ContactAccountDataModel::rowCount(const QModelIndex &parent) const
{
	return SourceContact.accountDatas().count();
}

QVariant ContactAccountDataModel::data(const QModelIndex &index, int role) const
{
	ContactAccountData *data = accountData(index);
	if (0 == data)
		return QVariant();

	switch (role)
	{
		case Qt::DisplayRole:
			if (index.column() == 0) // long or shor name?
				return data->id();
			else
				return QString("%1 (%2)").arg(data->id(), data->account().name());

		case Qt::DecorationRole:
			return data->account().protocolHandler()->icon();

		case AccountDataRole:
			return QVariant::fromValue<ContactAccountData *>(data);

		default:
			return QVariant();
	}
}

QVariant ContactAccountDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal)
		return QString("Column %1").arg(section);
	else
		return QString("Row %1").arg(section);
}

ContactAccountData * ContactAccountDataModel::accountData(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;

	if (index.row() < 0 || index.row() >= rowCount())
		return 0;

	return SourceContact.accountDatas().at(index.row());
}

int ContactAccountDataModel::accountDataIndex(ContactAccountData *data)
{
	return SourceContact.accountDatas().indexOf(data);
}

QModelIndex ContactAccountDataModel::accountDataModelIndex(ContactAccountData *data)
{
	return createIndex(accountDataIndex(data), 0, 0);
}

void ContactAccountDataModel::contactAccountDataAboutToBeAdded(ContactAccountData *data)
{
	int count = rowCount();
	beginInsertRows(QModelIndex(), count, count);
}

void ContactAccountDataModel::contactAccountDataAdded(ContactAccountData *data)
{
	endInsertRows();
}

void ContactAccountDataModel::contactAccountDataAboutToBeRemoved(ContactAccountData *data)
{
	int index = accountDataIndex(data);
	beginRemoveRows(QModelIndex(), index, index);
}

void ContactAccountDataModel::contactAccountDataRemoved(ContactAccountData *data)
{
	endRemoveRows();
}
