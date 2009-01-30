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
#include "accounts/accounts_aware_object.h"
#include "contacts/contact.h"
#include "protocols/status.h"

#include "abstract-contacts-model.h"

const int ContactRoles = 1000;
const int ContactRole = ContactRoles;

class ContactManager;

class ContactsModel : public QAbstractListModel, public AbstractContactsModel, public AccountsAwareObject
{
	Q_OBJECT

	ContactManager *Manager;

private slots:
	void contactAboutToBeAdded(Contact &contact);
	void contactAdded(Contact &contact);
	void contactAboutToBeRemoved(Contact &contact);
	void contactRemoved(Contact &contact);

	void contactStatusChanged(Account *account, Contact contact, Status oldStatus);

protected:
	virtual void accountRegistered(Account *account);
	virtual void accountUnregistered(Account *account);

public:
	explicit ContactsModel(ContactManager *manager, QObject *parent = 0);
	virtual ~ContactsModel();

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QFlags<Qt::ItemFlag> flags(const QModelIndex &index) const;

	virtual QVariant data(const QModelIndex &index, int role) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	// D&D
	virtual QStringList mimeTypes() const;
	virtual QMimeData* mimeData(const QList<QModelIndex> indexes) const;

	// IContactsModel implementation
	virtual const QModelIndex contactIndex(Contact contact) const;

};

#endif // CONTACTS_MODEL
