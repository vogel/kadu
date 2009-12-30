/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "buddies/buddy-manager.h"
#include "contacts/contact-manager.h"
#include "model/roles.h"
#include "protocols/protocol.h"

#include "buddy-contacts-table-model.h"

BuddyContactsTableModel::BuddyContactsTableModel(Buddy buddy, QObject *parent) :
		QAbstractTableModel(parent), ModelBuddy(buddy)
{
	contactsFromBuddy();
}

BuddyContactsTableModel::~BuddyContactsTableModel()
{
}

void BuddyContactsTableModel::save()
{
	buddyFromContacts();
}

void BuddyContactsTableModel::contactsFromBuddy()
{
	Contacts.clear();
	foreach (Contact contact, ModelBuddy.contacts())
		Contacts.append(BuddyContactsTableItem(contact));
}

void BuddyContactsTableModel::buddyFromContacts()
{
	foreach (BuddyContactsTableItem item, Contacts)
		performItemAction(item);
}

void BuddyContactsTableModel::performItemAction(const BuddyContactsTableItem &item)
{
	switch (item.action())
	{
		case BuddyContactsTableItem::ItemEdit:
			performItemActionEdit(item);
			break;

		case BuddyContactsTableItem::ItemAdd:
			performItemActionAdd(item);
			break;

		case BuddyContactsTableItem::ItemDetach:
			performItemActionDetach(item);
			break;

		case BuddyContactsTableItem::ItemRemove:
			performItemActionRemove(item);
			break;
	}
}

void BuddyContactsTableModel::performItemActionEdit(const BuddyContactsTableItem &item)
{
	Contact contact = item.itemContact();
	if (!contact)
		return;

	if (contact.contactAccount() != item.itemAccount())
	{
		// allow protocol handles to handle that
		ContactManager::instance()->removeItem(contact);
		contact.setContactAccount(item.itemAccount());
		contact.setId(item.id());
		ContactManager::instance()->addItem(contact);
	}
	else
		contact.setId(item.id());
}

void BuddyContactsTableModel::performItemActionAdd(const BuddyContactsTableItem &item)
{
	Contact contact = ContactManager::instance()->byId(item.itemAccount(), item.id(), true);
	contact.setOwnerBuddy(ModelBuddy);
}

void BuddyContactsTableModel::performItemActionDetach(const BuddyContactsTableItem &item)
{
	Contact contact = item.itemContact();
	if (!contact)
		return;

	QString display = item.detachedBuddyName();
	if (display.isEmpty())
		return;

	Buddy newBuddy = BuddyManager::instance()->byDisplay(display, true);
	contact.setOwnerBuddy(newBuddy);
}

void BuddyContactsTableModel::performItemActionRemove(const BuddyContactsTableItem &item)
{
	// save in configuration, but do not use
	Contact contact = item.itemContact();
	contact.setOwnerBuddy(Buddy::null);
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
	if (index.row() < 0 || index.row() >= Contacts.size())
		return QVariant();

	BuddyContactsTableItem item = Contacts.at(index.row());
	switch (index.column())
	{
		case 0:
			if (Qt::DisplayRole != role && Qt::EditRole != role)
				return QVariant();
			return item.id();
		case 1:
		{
			switch (role)
			{
				case Qt::DisplayRole:
				case Qt::EditRole:
					return item.itemAccount().name();
				case Qt::DecorationRole:
					return item.itemAccount().protocolHandler()
							? item.itemAccount().protocolHandler()->icon()
							: QVariant();
				case AccountRole:
					return QVariant::fromValue<Account>(item.itemAccount());
			}

			return QVariant();
		}
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
			if (Qt::EditRole == role)
				item.setId(value.toString());
			break;

		case 1:
			if (AccountRole == role)
				item.setItemAccount(qvariant_cast<Account>(value));
			break;
	}

	Contacts.replace(index.row(), item);

	return true;
}
