 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDIES_MODEL_BASE_H
#define BUDDIES_MODEL_BASE_H

#include <QtCore/QAbstractItemModel>

#include "accounts/account.h"
#include "accounts/accounts-aware-object.h"
#include "buddies/buddy.h"
#include "status/status.h"

#include "abstract-buddies-model.h"

class QModelIndex;

class BuddiesModelBase : public QAbstractItemModel, public AbstractBuddiesModel, public AccountsAwareObject
{
	Q_OBJECT

	Contact * buddyDefaultAccountData(const QModelIndex &index) const;
	Contact * buddyAccountData(const QModelIndex &index, int accountIndex) const;

	QVariant data(Buddy buddy, int role) const;
	QVariant data(Contact *cad, int role, bool useDisplay = true) const;

private slots:
	void buddyStatusChanged(Account account, Buddy buddy, Status oldStatus);

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

public:
	explicit BuddiesModelBase(QObject *parent = 0);
	virtual ~BuddiesModelBase();

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

#endif // BUDDIES_MODEL_BASE_H
