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

#include "buddies/buddy.h"
#include "buddies/buddy-list.h"

#include "exports.h"

class Account;
class Group;
class XmlConfigFile;

class KADUAPI ContactManager : public QObject, public StorableObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ContactManager)

	static ContactManager * Instance;

	ContactList Contacts;

	ContactManager();
	virtual ~ContactManager();

	const Contact & byContactData(ContactData *data);
	void importConfiguration(XmlConfigFile *configurationStorage);

private slots:
	void contactDataUpdated();
	void contactAccountDataAboutToBeAdded(Account account);
	void contactAccountDataAdded(Account account);
	void contactAccountDataAboutToBeRemoved(Account account);
	void contactAccountDataRemoved(Account account);
	void contactAccountDataIdChanged(Account account, const QString &oldId);

	void groupRemoved(Group *group);

protected:
	virtual StoragePoint * createStoragePoint();

public:
	static ContactManager * instance();

	virtual void load();
	virtual void store();

	ContactList contacts();
	ContactList contacts(Account account, bool includeAnonymous = false);
	void addContact(Contact contact);
	void removeContact(Contact contact);
	void mergeContact(Contact destination, Contact source);

	unsigned int count() { return Contacts.count(); }

	Contact byIndex(unsigned int index);
	int contactIndex(Contact contact) { return Contacts.indexOf(contact); }

	Contact byId(Account account, const QString &id);
	Contact byUuid(const QString &uuid);
	Contact byDisplay(const QString &display);

	void blockUpdatedSignal(Contact &contact);
	void unblockUpdatedSignal(Contact &contact);

signals:
	void contactAboutToBeAdded(Contact &contact);
	void contactAdded(Contact &contact);
	void contactAboutToBeRemoved(Contact &contact);
	void contactRemoved(Contact &contact);

	void contactUpdated(Contact &contact);
	void contactAccountDataAboutToBeAdded(Contact &contact, Account account);
	void contactAccountDataAdded(Contact &contact, Account account);
	void contactAccountDataAboutToBeRemoved(Contact &contact, Account account);
	void contactAccountDataRemoved(Contact &contact, Account account);
	void contactAccountIdChanged(Contact &contact, Account account, const QString &oldId);

};

#endif // CONTACT_MANAGER_H
