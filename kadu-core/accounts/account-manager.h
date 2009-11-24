/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ACCOUNT_MANAGER_H
#define ACCOUNT_MANAGER_H

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "accounts/account.h"
#include "protocols/protocol-factory.h"
#include "exports.h"

class AccountShared;
class Status;
class XmlConfigFile;

class KADUAPI AccountManager : public QObject, public StorableObject
{
	Q_OBJECT
	Q_DISABLE_COPY(AccountManager)

	static AccountManager *Instance;

	QList<Account> AllAccounts;
	QList<Account> RegisteredAccounts;

	AccountManager();
	virtual ~AccountManager();

private slots:
	void connectionError(Account account, const QString &server, const QString &message);
	void accountProtocolLoaded();
	void accountProtocolUnloaded();

protected:
	virtual StoragePoint * createStoragePoint();

public:
	static AccountManager * instance();

	virtual void load();
	virtual void store();

	Account defaultAccount();

	Account byIndex(unsigned int index);
	Account byUuid(const QUuid &uuid);

	unsigned int indexOf(Account account) { ensureLoaded(); return RegisteredAccounts.indexOf(account); }
	unsigned int count() { ensureLoaded(); return RegisteredAccounts.count(); }

	const QList<Account> accounts() { return RegisteredAccounts; }
	const QList<Account> byProtocolName(const QString &name);

	void registerAccount(Account account);
	void unregisterAccount(Account account);
	void deleteAccount(Account account);

	Status status();

signals:
	void accountAboutToBeRegistered(Account);
	void accountRegistered(Account);
	void accountAboutToBeUnregistered(Account);
	void accountUnregistered(Account);
	void accountAboutToBeRemoved(Account);
	void accountRemoved(Account);

};

#endif // ACCOUNT_MANAGER_H
