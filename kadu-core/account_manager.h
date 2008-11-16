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

#include <QtCore/QList>
#include <QtCore/QObject>

#include "status.h"

class Account;
class AccountData;

class AccountManager : public QObject
{
	Q_OBJECT

	static AccountManager *Instance;

	AccountManager();
	~AccountManager();

	QList<Account *> Accounts;

public:
	static AccountManager * instance();

	Account * defaultAccount();
	Account * createAccount(const QString &protocolName, AccountData *accountData);

	const QList<Account *> accounts() { return Accounts; }

	void registerAccount(const QString &name, Account *account);
	void unregisterAccount(const QString &name);

	UserStatus status();

signals:
	void accountRegistered(Account *);
	void accountUnregistered(Account *);

};

#endif // ACCOUNT_MANAGER_H
