/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDY_CONTACTS_TABLE_MODEL_H
#define BUDDY_CONTACTS_TABLE_MODEL_H

#include <QtCore/QAbstractTableModel>

#include "buddies/buddy.h"
#include "gui/widgets/buddy-contacts-table-item.h"

class BuddyContactsTableModel : public QAbstractTableModel
{
	Q_OBJECT

	Buddy ModelBuddy;
	QList<BuddyContactsTableItem> Contacts;

	void contactsFromBuddy();
	void buddyFromContacts();
	void performItemAction(const BuddyContactsTableItem &item);
	void performItemActionEdit(const BuddyContactsTableItem &item);
	void performItemActionAdd(const BuddyContactsTableItem &item);
	void performItemActionDetach(const BuddyContactsTableItem &item);
	void performItemActionRemove(const BuddyContactsTableItem &item);

public:
	explicit BuddyContactsTableModel(Buddy buddy, QObject *parent = 0);
	virtual ~BuddyContactsTableModel();

	void save();

	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	virtual Qt::ItemFlags flags(const QModelIndex &index) const;

	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

};

#endif // BUDDY_CONTACTS_TABLE_MODEL_H
