 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACTS_MODEL_BASE_H
#define CONTACTS_MODEL_BASE_H

#include <QtCore/QAbstractItemModel>

#include "accounts/account.h"
#include "accounts/accounts-aware-object.h"
#include "buddies/buddy.h"
#include "status/status.h"

#include "abstract-contacts-model.h"

class QModelIndex;

class ContactsModelBase : public QAbstractItemModel, public AbstractContactsModel, public AccountsAwareObject
{
	Q_OBJECT

	ContactAccountData * contactDefaultAccountData(const QModelIndex &index) const;
	ContactAccountData * contactAccountData(const QModelIndex &index, int accountIndex) const;

	QVariant data(Buddy contact, int role) const;
	QVariant data(ContactAccountData *cad, int role, bool useDisplay = true) const;

private slots:
	void contactStatusChanged(Account account, Buddy contact, Status oldStatus);

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

public:
	explicit ContactsModelBase(QObject *parent = 0);
	virtual ~ContactsModelBase();

	virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;

	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QFlags<Qt::ItemFlag> flags(const QModelIndex &index) const;

	virtual QModelIndex parent(const QModelIndex &child) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual QVariant data(const QModelIndex &index, int role) const;

	// D&D
	virtual QStringList mimeTypes() const;
	virtual QMimeData * mimeData(const QModelIndexList & indexes) const;

};

#endif // CONTACTS_MODEL_BASE_H
