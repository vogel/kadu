/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef BUDDY_CONTACTS_TABLE_MODEL_H
#define BUDDY_CONTACTS_TABLE_MODEL_H

#include <QtCore/QAbstractTableModel>

#include "buddies/buddy.h"

class BuddyContactsTableItem;
class ConfigurationValueStateNotifier;
class SimpleConfigurationValueStateNotifier;

class BuddyContactsTableModel : public QAbstractTableModel
{
	Q_OBJECT

	Buddy ModelBuddy;
	QList<BuddyContactsTableItem *> Contacts;
	SimpleConfigurationValueStateNotifier *StateNotifier;

	int CurrentMaxPriority;

	void contactsFromBuddy();
	void buddyFromContacts();
	void performItemAction(BuddyContactsTableItem *);
	void performItemActionEdit(BuddyContactsTableItem *item);
	void performItemActionAdd(BuddyContactsTableItem *);
	void performItemActionDetach(BuddyContactsTableItem *);
	void performItemActionRemove(BuddyContactsTableItem *);

	void addItem(BuddyContactsTableItem *item, bool emitRowsInserted = true);

	void sendAuthorization(const Contact &contact);

	bool isValid() const;
	void updateStateNotifier();

private slots:
	void itemUpdated(BuddyContactsTableItem *item);

public:
	explicit BuddyContactsTableModel(Buddy buddy, QObject *parent = 0);
	virtual ~BuddyContactsTableModel();

	const ConfigurationValueStateNotifier * valueStateNotifier() const;

	void save();

	BuddyContactsTableItem * item(int row);

	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	// NOTE: insertRows() always appends, ignores `row' argument.
	virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
	virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

	virtual Qt::ItemFlags flags(const QModelIndex &index) const;

	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

};

#endif // BUDDY_CONTACTS_TABLE_MODEL_H
