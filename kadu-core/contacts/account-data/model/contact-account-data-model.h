 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_ACCOUNT_DATA_MODEL
#define CONTACT_ACCOUNT_DATA_MODEL

#include <QtCore/QAbstractListModel>
#include <QtCore/QModelIndex>

#include "contacts/contact.h"

class Account;
class ContactAccountData;

class ContactAccountDataModel : public QAbstractListModel
{
	Q_OBJECT
	Contact SourceContact;

private slots:
	void contactAccountDataAboutToBeAdded(ContactAccountData *data);
	void contactAccountDataAdded(ContactAccountData *data);
	void contactAccountDataAboutToBeRemoved(ContactAccountData *data);
	void contactAccountDataRemoved(ContactAccountData *data);

public:
	ContactAccountDataModel(Contact contact, QObject *parent = 0);
	virtual ~ContactAccountDataModel();

	virtual int columnCount(const QModelIndex &parent) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QVariant data(const QModelIndex &index, int role) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	ContactAccountData * accountData(const QModelIndex &index) const;
	int accountDataIndex(ContactAccountData *data);
	QModelIndex accountDataModelIndex(ContactAccountData *data);
};

#endif // CONTACT_ACCOUNT_DATA_MODEL
