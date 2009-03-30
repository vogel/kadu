/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_MANAGER
#define CONTACT_MANAGER

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "configuration/storable-object.h"

#include "contact.h"
#include "contact-list.h"

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

	ContactManager() {}

	void importConfiguration(XmlConfigFile *configurationStorage);

protected:
	virtual StoragePoint * createStoragePoint();

public:
	static ContactManager * instance();

	void loadConfiguration();
	void storeConfiguration();

	ContactList contacts() const { return Contacts; }
	ContactList contacts(Account *account, bool includeAnonymous = false) const;
	void addContact(Contact contact);
	void removeContact(Contact contact);

	unsigned int count() { return Contacts.count(); }

	Contact byIndex(unsigned int index);
	int contactIndex(Contact contact) { return Contacts.indexOf(contact); }

	Contact byId(Account *account, const QString &id);
	Contact byUuid(const QString &uuid) const;
	Contact byDisplay(const QString &display) const;
//TODO 0.6.6: review
	void contactGroupRemoved(Group *group);

signals:
	void contactAboutToBeAdded(Contact &contact);
	void contactAdded(Contact &contact);
	void contactAboutToBeRemoved(Contact &contact);
	void contactRemoved(Contact &contact);

};

#endif // CONTACT_MANAGER
