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

#include "configuration/storable-object.h"
#include "exports.h"

class Contact;
class ContactShared;

class KADUAPI ContactManager : public QObject, public StorableObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ContactManager)

	static ContactManager * Instance;

	QList<Account> LoadedAccounts;
	QList<Contact> AllContacts;
	QList<Contact> LoadedContacts;

	ContactManager();
	virtual ~ContactManager();

private slots:
	void contactProtocolLoaded();
	void contactProtocolUnloaded();

protected:
	virtual StoragePoint * createStoragePoint();

public:
	static ContactManager * instance();

	virtual void load();
	virtual void store();

	void addContact(Contact contact);
	void removeContact(Contact contact);
	void changeContactId(Contact contact, const QString &oldId);

	unsigned int count() { return LoadedContacts.count(); }

	Contact byIndex(unsigned int index);
	Contact byId(Account account, const QString &id);
	QList<Contact> contacts(Account account);
	Contact byUuid(const QString &uuid);
	Contact byContactShared(ContactShared *data);

signals:
	void contactAboutToBeAdded(Contact contact);
	void contactAdded(Contact contact);
	void contactAboutToBeRemoved(Contact contact);
	void contactRemoved(Contact contact);
	// TODO emit somewhere
	void contactIdChanged(Contact contact, const QString &oldId);

};

// for MOC
#include "contacts/contact.h"

#endif // CONTACT_MANAGER_H
