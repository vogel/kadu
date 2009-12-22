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
#include "storage/manager.h"
#include "exports.h"

class AccountShared;
class Status;
class XmlConfigFile;

class KADUAPI AccountManager : public QObject, public Manager<Account>
{
	Q_OBJECT
	Q_DISABLE_COPY(AccountManager)

	static AccountManager *Instance;

	AccountManager();
	virtual ~AccountManager();

private slots:
	void connectionError(Account account, const QString &server, const QString &message);

protected:
	virtual void itemAdded(Account item);
	virtual void itemRemoved(Account item);

	virtual void itemAboutToBeRegistered(Account item);
	virtual void itemRegistered(Account item);
	virtual void itemAboutToBeUnregisterd(Account item);
	virtual void itemUnregistered(Account item);

public:
	static AccountManager * instance();

	virtual QString storageNodeName() { return QLatin1String("Accounts"); }
	virtual QString storageNodeItemName() { return QLatin1String("Account"); }

	// TODO: hide it someway...
	void detailsLoaded(Account account);
	void detailsUnloaded(Account account);

	Account defaultAccount();

	const QList<Account> byProtocolName(const QString &name);

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
