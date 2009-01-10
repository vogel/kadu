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

#include "protocols/protocol.h"

#include "accounts-model.h"

AccountsModel::AccountsModel(QObject *parent)
	: QAbstractListModel(parent)
{
}

int AccountsModel::rowCount(const QModelIndex &parent) const
{
	return AccountManager::instance()->accounts().count();
}

QVariant AccountsModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (index.row() >= rowCount())
		return QVariant();

	Account *account = AccountManager::instance()->accounts().at(index.row());

	switch (role)
	{
		case Qt::DisplayRole:
			return account->name();
		case Qt::DecorationRole:
			return account->protocol()->icon();
		default:
			return QVariant();
	}
}

QVariant AccountsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal)
		return QString("Column %1").arg(section);
	else
		return QString("Row %1").arg(section);
}
