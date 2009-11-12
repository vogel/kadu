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
	QList<Contact> AllContacts;
	QList<Contact> LoadedContacts;

	ContactManager();
	virtual ~ContactManager();

	void init();
	void loadContact(Contact contact);
	void unloadContact(Contact contact);
	void tryLoadContact(Contact contact);

protected:
	virtual StoragePoint * createStoragePoint();

	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

public:
	static ContactManager * instance();
	
	void ensureLoaded(Account account);
	virtual void load();
	virtual void store();

	void addContact(Contact contact);
	void removeContact(Contact contact);
	
	unsigned int count() { return LoadedContacts.count(); }

	Contact byIndex(unsigned int index);
	Contact byUuid(const QString &uuid);

signals:
	void contactAboutToBeAdded(Contact contact);
	void contactAdded(Contact contact);
	void contactAboutToBeRemoved(Contact contact);
	void contactRemoved(Contact contact);

};

// for MOC
#include "contacts/contact.h"

#endif // CONTACT_MANAGER_H
