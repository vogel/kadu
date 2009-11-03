 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDY_ACCOUNT_DATA_MODEL_H
#define BUDDY_ACCOUNT_DATA_MODEL_H

#include <QtCore/QAbstractListModel>
#include <QtCore/QModelIndex>

#include "buddies/buddy.h"

class Account;
class ContactAccountData;

class BuddyAccountDataModel : public QAbstractListModel
{
	Q_OBJECT

	Buddy SourceBuddy;

private slots:
	void contactAccountDataAboutToBeAdded(ContactAccountData *data);
	void contactAccountDataAdded(ContactAccountData *data);
	void contactAccountDataAboutToBeRemoved(ContactAccountData *data);
	void contactAccountDataRemoved(ContactAccountData *data);

public:
	BuddyAccountDataModel(Buddy contact, QObject *parent = 0);
	virtual ~BuddyAccountDataModel();

	virtual int columnCount(const QModelIndex &parent) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QVariant data(const QModelIndex &index, int role) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	ContactAccountData * accountData(const QModelIndex &index) const;
	int accountDataIndex(ContactAccountData *data);
	QModelIndex accountDataModelIndex(ContactAccountData *data);

};

#endif // BUDDY_ACCOUNT_DATA_MODEL_H
