/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ACCOUNT_MANAGER_H
#define ACCOUNT_MANAGER_H

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "accounts/account.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol.h"
#include "storage/manager.h"
#include "exports.h"

class Status;
class ConfigurationApi;

class KADUAPI AccountManager : public QObject, public Manager<Account>
{
	Q_OBJECT
	Q_DISABLE_COPY(AccountManager)

	static AccountManager *Instance;

	AccountManager();
	virtual ~AccountManager();

	void init();

private slots:
	void passwordProvided(const QVariant &data, const QString &password, bool permament);

	void accountDataUpdated();
	void connectionError(Account account, const QString &server, const QString &message);

protected:
	virtual void itemAdded(Account item);
	virtual void itemRemoved(Account item);

	virtual void itemAboutToBeRegistered(Account item);
	virtual void itemRegistered(Account item);
	virtual void itemAboutToBeUnregisterd(Account item);
	virtual void itemUnregistered(Account item);

	virtual void loaded();

public:
	static AccountManager * instance();

	template<template <class> class Container>
	static Account bestAccount(const Container<Account> &accounts)
	{
		Account result;
		if (accounts.isEmpty())
			return result;

		foreach (const Account &account, accounts)
			if (account.details() && account.data())
			{
				// TODO: hack
				bool newConnected = account.data()->protocolHandler() && account.data()->protocolHandler()->isConnected();
				bool oldConnected = false;
				if (result)
					oldConnected = result.data()->protocolHandler() && result.data()->protocolHandler()->isConnected();

				if (!result || (newConnected && !oldConnected)  || (account.protocolName() == "gadu" && result.protocolName() != "gadu"))
				{
					result = account;
					if (newConnected && result.protocolName() == "gadu")
						break;
				}
			}

		return result;
	}

	virtual QString storageNodeName() { return QLatin1String("Accounts"); }
	virtual QString storageNodeItemName() { return QLatin1String("Account"); }

	Account defaultAccount();
	Account bestAccount();

	const QVector<Account> byIdentity(Identity identity);
	Account byId(const QString &protocolName, const QString &id);
	const QVector<Account> byProtocolName(const QString &name);

	void removeAccountAndBuddies(Account account);

public slots:
	void providePassword(Account account);

signals:
	void accountAboutToBeRegistered(Account);
	void accountRegistered(Account);
	void accountAboutToBeUnregistered(Account);
	void accountUnregistered(Account);

	void accountAboutToBeRemoved(Account);
	void accountRemoved(Account);

	void accountUpdated(Account);

};

#endif // ACCOUNT_MANAGER_H
