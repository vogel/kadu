 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDIES_MODEL_H
#define BUDDIES_MODEL_H

#include <QtCore/QAbstractListModel>
#include <QtCore/QModelIndex>

#include "accounts/account.h"
#include "accounts/accounts-aware-object.h"
#include "buddies/buddy.h"
#include "status/status.h"

#include "buddies-model-base.h"

class BuddyManager;

class BuddiesModel : public BuddiesModelBase
{
	Q_OBJECT

	BuddyManager *Manager;

private slots:
	void buddyAboutToBeAdded(Buddy &buddy);
	void buddyAdded(Buddy &buddy);
	void buddyAboutToBeRemoved(Buddy &buddy);
	void buddyRemoved(Buddy &buddy);

public:
	explicit BuddiesModel(BuddyManager *manager, QObject *parent = 0);
	~BuddiesModel();

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	
	// AbstractContactsModel implementation
	virtual Buddy buddyAt(const QModelIndex &index) const;
	virtual const QModelIndex buddyIndex(Buddy buddy) const;

};

#endif // BUDDIES_MODEL_H
