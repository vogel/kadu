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

#include "exports.h"

class Account;
class AccountData;
class Status;
class XmlConfigFile;

class KADUAPI AccountManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(AccountManager)

	static AccountManager *Instance;

	AccountManager();
	virtual ~AccountManager();

	QList<Account *> Accounts;

public:
	static AccountManager * instance();

	//TODO: 0.6.6
	void loadConfiguration(XmlConfigFile *configurationStorage, const QString &name = QString::null);
	//TODO: 0.6.6
	void storeConfiguration(XmlConfigFile *configurationStorage, const QString &name = QString::null);

	Account * defaultAccount() const;

	Account * byIndex(unsigned int index) const;
	Account * byUuid(const QUuid &uuid) const;

	unsigned int indexOf(Account *account) const { return Accounts.indexOf(account); }
	unsigned int count() const { return Accounts.count(); }

	const QList<Account *> accounts() const { return Accounts; }
	const QList<Account *> byProtocolName(const QString &name) const;

	void registerAccount(Account *account);
	void unregisterAccount(Account *account);

	Status status() const;

signals:
	void accountAboutToBeRegistered(Account *);
	void accountRegistered(Account *);
	void accountAboutToBeUnregistered(Account *);
	void accountUnregistered(Account *);

};

#endif // ACCOUNT_MANAGER_H
