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
	connect(AccountManager::instance(), SIGNAL(accountAboutToBeRegistered(Account *)),
			this, SLOT(accountAboutToBeRegistered(Account *)));
	connect(AccountManager::instance(), SIGNAL(accountRegistered(Account *)),
			this, SLOT(accountRegistered(Account *)));
	connect(AccountManager::instance(), SIGNAL(accountAboutToBeUnregistered(Account *)),
			this, SLOT(accountAboutToBeUnregistered(Account *)));
	connect(AccountManager::instance(), SIGNAL(accountUnregistered(Account *)),
			this, SLOT(accountUnregistered(Account *)));
}

AccountsModel::~AccountsModel()
{
	disconnect(AccountManager::instance(), SIGNAL(accountAboutToBeRegistered(Account *)),
			this, SLOT(accountAboutToBeRegistered(Account *)));
	disconnect(AccountManager::instance(), SIGNAL(accountRegistered(Account *)),
			this, SLOT(accountRegistered(Account *)));
	disconnect(AccountManager::instance(), SIGNAL(accountAboutToBeUnregistered(Account *)),
			this, SLOT(accountAboutToBeUnregistered(Account *)));
	disconnect(AccountManager::instance(), SIGNAL(accountUnregistered(Account *)),
			this, SLOT(accountUnregistered(Account *)));
}

int AccountsModel::rowCount(const QModelIndex &parent) const
{
	return AccountManager::instance()->accounts().count();
}

QVariant AccountsModel::data(const QModelIndex &index, int role) const
{
	Account *acc = account(index);
	if (0 == acc)
		return QVariant();

	switch (role)
	{
		case Qt::DisplayRole:
			return acc->name();
		case Qt::DecorationRole:
			return acc->protocol()->icon();
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

Account * AccountsModel::account(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;

	if (index.row() >= rowCount())
		return 0;

	return AccountManager::instance()->accounts().at(index.row());
}

int AccountsModel::accountIndex(Account *account)
{
	return AccountManager::instance()->accounts().indexOf(account);
}

void AccountsModel::accountAboutToBeRegistered(Account *account)
{
	int count = rowCount();
	beginInsertRows(QModelIndex(), count, count);
}

void AccountsModel::accountRegistered(Account *account)
{
	endInsertRows();
}

void AccountsModel::accountAboutToBeUnregistered(Account *account)
{
	int index = accountIndex(account);
	beginRemoveRows(QModelIndex(), index, index);
}

void AccountsModel::accountUnregistered(Account *account)
{
	endRemoveRows();
}
