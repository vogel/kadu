/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "buddy-contacts-table-model.h"


BuddyContactsTableModel::BuddyContactsTableModel(Buddy buddy, QObject *parent) :
		QAbstractTableModel(parent), ModelBuddy(buddy)
{
}

BuddyContactsTableModel::~BuddyContactsTableModel()
{
}

int BuddyContactsTableModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;
	else
		return 2;
}

int BuddyContactsTableModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;
	else
		return 4;
}

Qt::ItemFlags BuddyContactsTableModel::flags(const QModelIndex &index) const
{
	return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

QVariant BuddyContactsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (Qt::Horizontal != orientation)
		return QVariant();

	if (Qt::DisplayRole != role)
		return QVariant();

	switch (section)
	{
		case 0: return tr("Username");
		case 1: return tr("Account");
	}

	return QVariant();
}

QVariant BuddyContactsTableModel::data(const QModelIndex &index, int role) const
{
	if (Qt::DisplayRole != role)
		return QVariant();

	return "ooo, fake data!!";
}
