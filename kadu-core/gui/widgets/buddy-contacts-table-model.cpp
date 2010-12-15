/*
 * %kadu copyright begin%
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "buddies/buddy-manager.h"
#include "contacts/contact-manager.h"
#include "gui/widgets/buddy-contacts-table-item.h"
#include "model/roles.h"
#include "protocols/protocol.h"

#include "buddy-contacts-table-model.h"

BuddyContactsTableModel::BuddyContactsTableModel(Buddy buddy, QObject *parent) :
		QAbstractTableModel(parent), ModelBuddy(buddy), CurrentMaxPriority(-1)
{
	contactsFromBuddy();
}

BuddyContactsTableModel::~BuddyContactsTableModel()
{
}

bool BuddyContactsTableModel::isValid()
{
	foreach (BuddyContactsTableItem *item, Contacts)
		if (!item->isValid())
			return false;

	return true;
}

void BuddyContactsTableModel::save()
{
	buddyFromContacts();

	beginRemoveRows(QModelIndex(), 0, Contacts.count() - 1);
	qDeleteAll<>(Contacts);
	Contacts.clear();
	endRemoveRows();

	contactsFromBuddy();
}

BuddyContactsTableItem * BuddyContactsTableModel::item(int row)
{
	if (row >= 0 && row < Contacts.count())
		return Contacts[row];
	else
		return 0;
}

void BuddyContactsTableModel::contactsFromBuddy()
{
	ModelBuddy.normalizePriorities();
	if (ModelBuddy.contacts().isEmpty())
		CurrentMaxPriority = -1;
	else
		CurrentMaxPriority = ModelBuddy.contacts().last().priority();

	Contacts.clear();
	foreach (const Contact &contact, ModelBuddy.contacts())
		addItem(new BuddyContactsTableItem(contact, this));
}

void BuddyContactsTableModel::buddyFromContacts()
{
	foreach (BuddyContactsTableItem *item, Contacts)
		performItemAction(item);

	ModelBuddy.sortContacts();
	ModelBuddy.normalizePriorities();
}

void BuddyContactsTableModel::performItemAction(BuddyContactsTableItem *item)
{
	switch (item->action())
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

void BuddyContactsTableModel::performItemActionEdit(BuddyContactsTableItem *item)
{
	Contact contact = item->itemContact();
	if (!contact)
		return;

	contact.setPriority(item->itemContactPriority());

	if (contact.contactAccount() != item->itemAccount())
	{
		// allow protocol handles to handle that
		ContactManager::instance()->removeItem(contact);
		contact.setContactAccount(item->itemAccount());
		contact.setId(item->id());
		ContactManager::instance()->addItem(contact);
	}
	else
		contact.setId(item->id());
}

void BuddyContactsTableModel::performItemActionAdd(BuddyContactsTableItem *item)
{
	Contact contact = ContactManager::instance()->byId(item->itemAccount(), item->id(), ActionCreateAndAdd);
	contact.setOwnerBuddy(ModelBuddy);
	contact.setPriority(item->itemContactPriority());
}

void BuddyContactsTableModel::performItemActionDetach(BuddyContactsTableItem *item)
{
	Contact contact = item->itemContact();
	if (!contact)
		return;

	QString display = item->detachedBuddyName();
	if (display.isEmpty())
		return;

	Buddy newBuddy = BuddyManager::instance()->byDisplay(display, ActionCreateAndAdd);
	newBuddy.setAnonymous(false);
	contact.setOwnerBuddy(newBuddy);
}

void BuddyContactsTableModel::performItemActionRemove(BuddyContactsTableItem *item)
{
	// save in configuration, but do not use
	Contact contact = item->itemContact();
	contact.setOwnerBuddy(Buddy::null);
}

void BuddyContactsTableModel::addItem(BuddyContactsTableItem *item)
{
	beginInsertRows(QModelIndex(), Contacts.count(), Contacts.count());

	connect(item, SIGNAL(updated(BuddyContactsTableItem*)),
			this, SLOT(itemUpdated(BuddyContactsTableItem*)));
	Contacts.append(item);

	endInsertRows();
}

void BuddyContactsTableModel::itemUpdated(BuddyContactsTableItem *item)
{
	int index = Contacts.indexOf(item);
	if (index != -1)
	{
		emit dataChanged(createIndex(index, 0), createIndex(index, 2));
		emit validChanged();
	}
}

int BuddyContactsTableModel::columnCount(const QModelIndex &parent) const
{
	return parent.isValid() ? 0 : 2;
}

int BuddyContactsTableModel::rowCount(const QModelIndex &parent) const
{
	return parent.isValid() ? 0 : Contacts.count();
}

bool BuddyContactsTableModel::insertRows(int row, int count, const QModelIndex& parent)
{
	Q_UNUSED(row)
	Q_UNUSED(parent)

	for (int i = 0; i < count; i++)
	{
		CurrentMaxPriority++;

		BuddyContactsTableItem *item = new BuddyContactsTableItem(this);
		item->setAction(BuddyContactsTableItem::ItemAdd);
		item->setItemContactPriority(CurrentMaxPriority);
		addItem(item);
	}

	return true;
}

bool BuddyContactsTableModel::removeRows(int row, int count, const QModelIndex& parent)
{
	beginRemoveRows(parent, row, row + count - 1);

	for (int i = 0; i < count; i++)
		delete Contacts.takeAt(row);

	endRemoveRows();

	return true;
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

	BuddyContactsTableItem *item = Contacts.at(index.row());
	switch (role)
	{
		case BuddyContactsTableItemRole:
			return QVariant::fromValue<BuddyContactsTableItem *>(item);

		case Qt::BackgroundColorRole:
			return item->isValid()
					? QVariant()
					: QColor(255, 0, 0, 25);
	}

	switch (index.column())
	{
		case 0:
			if (Qt::DisplayRole != role && Qt::EditRole != role)
				return QVariant();
			return item->id();
		case 1:
		{
			switch (role)
			{
				case Qt::DisplayRole:
				case Qt::EditRole:
					return item->itemAccount().accountIdentity().name();
				case Qt::DecorationRole:
					return item->itemAccount().protocolHandler()
							? item->itemAccount().protocolHandler()->icon()
							: QIcon();
				case AccountRole:
					return QVariant::fromValue<Account>(item->itemAccount());
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

	BuddyContactsTableItem *item = Contacts.at(index.row());
	switch (index.column())
	{
		case 0:
			if (Qt::EditRole == role)
				item->setId(value.toString());
			break;

		case 1:
			if (AccountRole == role)
				item->setItemAccount(value.value<Account>());
			break;
	}

	return true;
}
