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

#include "status.h"

class Account;
class AccountData;
class XmlConfigFile;

class KADUAPI AccountManager : public QObject
{
	Q_OBJECT

	static AccountManager *Instance;

	AccountManager();
	~AccountManager();

	QMap<QUuid, Account *> Accounts;

public:
	static AccountManager * instance();

	//TODO: 0.6.6
	void loadConfiguration(XmlConfigFile *configurationStorage, const QString &name = QString::null);
	//TODO: 0.6.6
	void storeConfiguration(XmlConfigFile *configurationStorage, const QString &name = QString::null);

	Account * defaultAccount();
	Account * createAccount(const QString &protocolName, AccountData *accountData);
	Account * account(const QUuid &uuid);

	const QList<Account *> accounts() { return Accounts.values(); }
	const QList<Account *> byProtocolName(const QString &name);

	void registerAccount(Account *account);
	void unregisterAccount(Account *account);

	UserStatus status();

signals:
	void accountAboutToBeRegistered(Account *);
	void accountRegistered(Account *);
	void accountAboutToBeUnregistered(Account *);
	void accountUnregistered(Account *);

};

#endif // ACCOUNT_MANAGER_H
