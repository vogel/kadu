/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "account.h"
#include "protocols_manager.h"

#include "account_manager.h"

AccountManager * AccountManager::Instance = 0;

AccountManager * AccountManager::instance()
{
	if (0 == Instance)
		Instance = new AccountManager();

	return Instance;
}

AccountManager::AccountManager()
{
}

AccountManager::~AccountManager()
{
}

Account * AccountManager::defaultAccount()
{
	return (0 == Accounts.size()) ? 0 : Accounts[0];
}

Account * AccountManager::createAccount(const QString &protocolName, AccountData *accountData)
{
	Protocol *protocol = ProtocolsManager::instance()->newInstance(protocolName);
	if (0 == protocol)
		return 0;

	return new Account(protocol, accountData);
}

void AccountManager::registerAccount(const QString &name, Account *account)
{
	// TODO: implement
	Accounts.append(account);
	emit accountRegistered(account);
}

void AccountManager::unregisterAccount(const QString &name)
{
	// TODO: implement
}

UserStatus AccountManager::status()
{
	return (0 == Accounts.size()) ? UserStatus() : Accounts[0]->currentStatus();
}
