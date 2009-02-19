/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "account.h"
#include "account_data.h"
#include "accounts_aware_object.h"
#include "config_file.h"
#include "debug.h"
#include "protocols/protocol.h"
#include "protocols/protocol_factory.h"
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

void AccountManager::loadConfiguration(XmlConfigFile *configurationStorage, const QString &name)
{
	if (name.isEmpty())
		return;

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

		// TODO hasAccountUUID(uuid) => return

		Account *account = new Account(uuid);

		if (account->loadConfiguration(configurationStorage, accountElement))
		{
			if (account->protocol()->protocolFactory()->name() == name)
				registerAccount(account);
			else
				delete account;
		}
		else
			delete account;
	}
}

void AccountManager::storeConfiguration(XmlConfigFile *configurationStorage, const QString &name)
{
	QDomElement accountsNode = configurationStorage->getNode("Accounts");

	foreach (Account *account, Accounts)
	{
		if (account->protocol()->protocolFactory()->name() == name || name.isNull())
		{
			QDomElement accountNode = configurationStorage->getUuidNode(accountsNode, "Account", account->uuid(), XmlConfigFile::ModeCreate);
			account->storeConfiguration(configurationStorage, accountNode);
			if (!name.isNull())
				unregisterAccount(account);
		}
	}
}

Account * AccountManager::defaultAccount() const
{
	return byIndex(0);
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

Account * AccountManager::byIndex(unsigned int index) const
{
	if (index < 0 || index >= count())
		return 0;

	return Accounts.at(index);
}

Account * AccountManager::byUuid(const QUuid &uuid) const
{
	foreach (Account *account, Accounts)
		if (uuid == account->uuid())
			return account;

	return 0;
}
const QList<Account *> AccountManager::byProtocolName(const QString &name) const
{
	QList<Account *> list;
	foreach (Account *account, Accounts)
	{
		if (account->protocol()->protocolFactory()->name() == name)
			list.append(account);
	}
	return list;
}

void AccountManager::registerAccount(Account *account)
{
	emit accountAboutToBeRegistered(account);
	Accounts << account;
	emit accountRegistered(account);
	AccountsAwareObject::notifyAccountRegistered(account);
}

void AccountManager::unregisterAccount(Account *account)
{
	AccountsAwareObject::notifyAccountUnregistered(account);
	emit accountAboutToBeUnregistered(account);
	Accounts.removeAll(account);
	emit accountUnregistered(account);
}

Status AccountManager::status() const
{
	Account *account = defaultAccount();
	return account
		? account->currentStatus()
		: Status();
}
