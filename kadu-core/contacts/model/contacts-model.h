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
#include "contacts/contact.h"
#include "protocols/status.h"

#include "abstract-contacts-model.h"

const int ContactRoles = 1000;
const int ContactRole = ContactRoles;

class ContactManager;

class ContactsModel : public QAbstractListModel, public AbstractContactsModel
{
	Q_OBJECT

	ContactManager *Manager;

private slots:
	void contactAboutToBeAdded(Contact &contact);
	void contactAdded(Contact &contact);
	void contactAboutToBeRemoved(Contact &contact);
	void contactRemoved(Contact &contact);

	void accountRegistered(Account *account);
	void accountUnregistered(Account *account);

	void contactStatusChanged(Account *account, Contact contact, Status oldStatus);

public:
	explicit ContactsModel(ContactManager *manager, QObject *parent = 0);
	virtual ~ContactsModel();

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	// IContactsModel implementation
	virtual const QModelIndex contactIndex(Contact contact) const;

};

#endif // CONTACTS_MODEL
