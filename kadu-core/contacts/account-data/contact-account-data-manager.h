/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_ACCOUNT_DATA_MANAGER_H
#define CONTACT_ACCOUNT_DATA_MANAGER_H

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "accounts/accounts-aware-object.h"
#include "configuration/storable-object.h"
#include "exports.h"

class ContactAccountData;

class KADUAPI ContactAccountDataManager : public QObject, public StorableObject, AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ContactAccountDataManager)

	static ContactAccountDataManager * Instance;
	
	QList<Account *> LoadedAccounts;
	QList<ContactAccountData *> ContactAccountDataList;

	ContactAccountDataManager();
	virtual ~ContactAccountDataManager();

	void init();

	void load(Account *account);
	void store(Account *account);

protected:
	virtual StoragePoint * createStoragePoint();

	virtual void accountRegistered(Account *account);
	virtual void accountUnregistered(Account *account);

public:
	static ContactAccountDataManager * instance();
	
	void ensureLoaded(Account *account);
	virtual void load();
	virtual void store();

	void addContactAccountData(ContactAccountData *cad);
	void removeContactAccountData(ContactAccountData *cad);
	
	unsigned int count() { return ContactAccountDataList.count(); }

	ContactAccountData * byIndex(unsigned int index);
	ContactAccountData * byUuid(const QString &uuid);

signals:
	void contactAccountDataAboutToBeAdded(ContactAccountData *cad);
	void contactAccountDataAdded(ContactAccountData *cad);
	void contactAccountDataAboutToBeRemoved(ContactAccountData *cad);
	void contactAccountDataRemoved(ContactAccountData *cad);

};

#endif // CONTACT_ACCOUNT_DATA_MANAGER_H
