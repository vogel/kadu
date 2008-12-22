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
#include "protocols/protocol.h"
#include "protocols/protocols_manager.h"
#include "xml_config_file.h"

#include "account_manager.h"

AccountManager * AccountManager::Instance = 0;

KADUAPI AccountManager * AccountManager::instance()
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

		QUuid uuid(accountElement.attribute("uuid"));
		if (uuid.isNull())
			uuid = QUuid::createUuid();

		Account *account = new Account(uuid);

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
	return (0 == Accounts.values().size()) ? 0 : Accounts.values()[0];
}

Account * AccountManager::createAccount(const QString &protocolName, AccountData *accountData)
{
	Protocol *protocol = ProtocolsManager::instance()->newInstance(protocolName);
	if (0 == protocol)
		return 0;

	Account *result = new Account(QUuid::createUuid(), protocol, accountData);
	protocol->setAccount(result);

	return result;
}

Account * AccountManager::account(const QUuid &uuid)
{
	if (Accounts.contains(uuid))
		return Accounts[uuid];

	return 0;
}

void AccountManager::registerAccount(Account *account)
{
	Accounts[account->uuid()] = account;
	emit accountRegistered(account);
}

void AccountManager::unregisterAccount(Account *account)
{
	Accounts.remove(account->uuid());
	emit accountUnregistered(account);
}

UserStatus AccountManager::status()
{
	Account *account = defaultAccount();
	return account
		? account->currentStatus()
		: UserStatus();
}
