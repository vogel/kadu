/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHAT_H
#define CHAT_H

#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "accounts/account.h"
#include "configuration/storable-object.h"
#include "contacts/contact-list.h"

class XmlConfigFile;

class Chat : public StorableObject
{
	Account *CurrentAccount;
	QUuid Uuid;

protected:
	virtual StoragePoint * createStoragePoint() const;

public:
	Chat(Account *parentAccount, QUuid uuid = QUuid());
	virtual ~Chat();

	QUuid uuid() const;
	Account *account() { return CurrentAccount; }
	static Chat * loadFromStorage(StoragePoint *conferenceStoragePoint);
	virtual ContactList currentContacts() { return ContactList(); }

	virtual void loadConfiguration();
	virtual void storeConfiguration();

};

#endif // CHAT_H
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHAT_H
#define CHAT_H

#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "accounts/account.h"
#include "configuration/storable-object.h"
#include "contacts/contact-list.h"

class XmlConfigFile;

class Chat : public StorableObject
{
	Account *CurrentAccount;
	QUuid Uuid;

protected:
	virtual StoragePoint * createStoragePoint() const;

public:
	Chat(Account *parentAccount, QUuid uuid = QUuid());
	virtual ~Chat();

	QUuid uuid() const;
	Account *account() { return CurrentAccount; }
	static Chat * loadFromStorage(StoragePoint *conferenceStoragePoint);
	virtual ContactList currentContacts() { return ContactList(); }

	virtual void loadConfiguration();
	virtual void storeConfiguration();

};

#endif // CHAT_H
