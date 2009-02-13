 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACTS_LIST_MODEL
#define CONTACTS_LIST_MODEL

#include <QtCore/QAbstractListModel>
#include <QtCore/QModelIndex>

#include "accounts/accounts_aware_object.h"

#include "contacts/contact.h"
#include "contacts/contact-list.h"

#include "protocols/status.h"

#include "contacts-model-base.h"

class ContactListModel : public ContactsModelBase
{
	Q_OBJECT

	ContactList List;

public:
	explicit ContactListModel(ContactList list, QObject *parent = 0);

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	// AbstractContactsModel implementation
	virtual Contact contact(const QModelIndex& index) const;
	virtual const QModelIndex contactIndex(Contact contact) const;

};

#endif // CONTACTS_LIST_MODEL
