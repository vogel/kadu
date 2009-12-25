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
	contactsFromBuddy();
}

BuddyContactsTableModel::~BuddyContactsTableModel()
{
}

void BuddyContactsTableModel::contactsFromBuddy()
{
	Contacts.clear();
	foreach (Contact contact, ModelBuddy.contacts())
		Contacts.append(BuddyContactsTableItem(contact));
}

int BuddyContactsTableModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;

	return 2;
}

int BuddyContactsTableModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;

	return Contacts.count();
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
	if (Qt::DisplayRole != role && Qt::EditRole != role)
		return QVariant();

	if (index.row() < 0 || index.row() >= Contacts.size())
		return QVariant();

	BuddyContactsTableItem item = Contacts.at(index.row());
	switch (index.column())
	{
		case 0: return item.id();
		case 1: return item.itemAccount().name();
	}

	return QVariant();
}

bool BuddyContactsTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (index.row() < 0 || index.row() >= Contacts.size())
		return false;

	BuddyContactsTableItem item = Contacts.at(index.row());
	switch (index.column())
	{
		case 0:
			item.setId(value.toString());
			break;

		case 1: // TODO
			break;
	}

	Contacts.replace(index.row(), item);

	return true;
}
