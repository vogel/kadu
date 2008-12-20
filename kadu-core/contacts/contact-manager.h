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

#include "contact.h"

class Account;
class XmlConfigFile;

class ContactManager : public QObject
{
	Q_OBJECT

	static ContactManager * Instance;

	QMap<QUuid, Contact> Contacts;

	ContactManager() {}
	ContactManager(const ContactManager &) {}

	void importConfiguration(XmlConfigFile *configurationStorage);

public:
	static ContactManager * instance();

	void loadConfiguration(XmlConfigFile *configurationStorage);
	void storeConfiguration(XmlConfigFile *configurationStorage);

	QList<Contact> contacts() { return Contacts.values(); }
	void addContact(Contact contact);

	Contact getContactById(Account *account, const QString &id);

signals:
	void contactAdded(Contact &contact);

};

#endif // CONTACT_MANAGER
