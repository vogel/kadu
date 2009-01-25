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

class Account;
class XmlConfigFile;

class ContactManager : public QObject, public StorableObject
{
	Q_OBJECT

	static ContactManager * Instance;

	ContactList Contacts;

	ContactManager() {}
	ContactManager(const ContactManager &) {}

	void importConfiguration(XmlConfigFile *configurationStorage);

protected:
	virtual StoragePoint * createStoragePoint() const;

public:
	static ContactManager * instance();

	void loadConfiguration(XmlConfigFile *configurationStorage);
	void storeConfiguration(XmlConfigFile *configurationStorage);

	ContactList contacts() const { return Contacts; }
	ContactList contacts(Account *account, bool includeAnonymous = false) const;
	void addContact(Contact contact);

	unsigned int count() { return Contacts.count(); }

	Contact byIndex(unsigned int index);
	int contactIndex(Contact contact) { return Contacts.indexOf(contact); }

	Contact byId(Account *account, const QString &id);
	Contact byUuid(const QString &uuid) const;
	Contact byDisplay(const QString &display) const;

signals:
	void contactAboutToBeAdded(Contact &contact);
	void contactAdded(Contact &contact);
	void contactAboutToBeRemoved(Contact &contact);
	void contactBeRemoved(Contact &contact);

};

#endif // CONTACT_MANAGER
