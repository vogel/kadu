/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "account.h"
#include "config_file.h"
#include "protocols_manager.h"
#include "xml_config_file.h"

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

void AccountManager::loadConfiguration(XmlConfigFile *configurationStorage)
{
	QDomElement accountsNode = configurationStorage->getNode("Accounts", XmlConfigFile::ModeFind);
	if (accountsNode.isNull())
		return;

	QDomNodeList accountNodes = configurationStorage->getNodes(accountsNode, "Account");
	int count = accountNodes.count();
	for (int i = 0; i < count; i++)
	{
		QDomElement accountElement = accountNodes.item(i).toElement();
		if (accountElement.isNull())
			continue;

		QString accountName = accountElement.attribute("name");
		Account *account = new Account(accountName);

		if (account->loadConfiguration(configurationStorage, accountElement))
			registerAccount(account);
		else
			delete account;
	}
}

void AccountManager::storeConfiguration(XmlConfigFile *configurationStorage)
{
	QDomElement accountsNode = configurationStorage->getNode("Accounts");

	foreach (Account *account, Accounts)
	{
		QDomElement accountNode = configurationStorage->getNamedNode(accountsNode, "Account", account->name(), XmlConfigFile::ModeCreate);
		account->storeConfiguration(configurationStorage, accountNode);
	}
}

Account * AccountManager::defaultAccount()
{
	return (0 == Accounts.size()) ? 0 : Accounts[0];
}

Account * AccountManager::createAccount(const QString &name, const QString &protocolName, AccountData *accountData)
{
	Protocol *protocol = ProtocolsManager::instance()->newInstance(protocolName);
	if (0 == protocol)
		return 0;

	return new Account(name, protocol, accountData);
}

void AccountManager::registerAccount(Account *account)
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
