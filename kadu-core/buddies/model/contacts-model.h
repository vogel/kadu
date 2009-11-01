 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACTS_MODEL
#define CONTACTS_MODEL

#include <QtCore/QAbstractListModel>
#include <QtCore/QModelIndex>

#include "accounts/account.h"
#include "accounts/accounts-aware-object.h"
#include "buddies/buddy.h"
#include "status/status.h"

#include "contacts-model-base.h"

class BuddyManager;

class ContactsModel : public ContactsModelBase
{
	Q_OBJECT

	BuddyManager *Manager;

private slots:
	void contactAboutToBeAdded(Buddy &contact);
	void contactAdded(Buddy &contact);
	void contactAboutToBeRemoved(Buddy &contact);
	void contactRemoved(Buddy &contact);

public:
	explicit ContactsModel(BuddyManager *manager, QObject *parent = 0);
	~ContactsModel();

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	
	// AbstractContactsModel implementation
	virtual Buddy contact(const QModelIndex &index) const;
	virtual const QModelIndex contactIndex(Buddy contact) const;

};

#endif // CONTACTS_MODEL
