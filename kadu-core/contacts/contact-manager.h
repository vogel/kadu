/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_MANAGER_H
#define CONTACT_MANAGER_H

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "accounts/accounts-aware-object.h"
#include "configuration/storable-object.h"
#include "exports.h"

class Contact;

class KADUAPI ContactManager : public QObject, public StorableObject, AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ContactManager)

	static ContactManager * Instance;

	QList<Account> LoadedAccounts;
	QList<Contact *> ContactList;

	ContactManager();
	virtual ~ContactManager();

	void init();

	void load(Account account);
	void store(Account account);

protected:
	virtual StoragePoint * createStoragePoint();

	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

public:
	static ContactManager * instance();
	
	void ensureLoaded(Account account);
	virtual void load();
	virtual void store();

	void addContact(Contact *cad);
	void removeContact(Contact *cad);
	
	unsigned int count() { return ContactList.count(); }

	Contact * byIndex(unsigned int index);
	Contact * byUuid(const QString &uuid);

signals:
	void contactAboutToBeAdded(Contact *cad);
	void contactAdded(Contact *cad);
	void contactAboutToBeRemoved(Contact *cad);
	void contactRemoved(Contact *cad);

};

#endif // CONTACT_MANAGER_H
