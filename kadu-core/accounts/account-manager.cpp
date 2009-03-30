/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"
#include "accounts/accounts-aware-object.h"
#include "notify/notification-manager.h"
#include "protocols/connection-error-notification.h"
#include "protocols/protocol.h"
#include "protocols/protocol_factory.h"
#include "protocols/protocols_manager.h"
#include "config_file.h"
#include "debug.h"
#include "xml_config_file.h"

#include "account-manager.h"

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

StoragePoint * AccountManager::createStoragePoint()
{
	return new StoragePoint(xml_config_file, xml_config_file->getNode("Accounts"));
}

void AccountManager::loadConfiguration(const QString &protocolName)
{
	if (protocolName.isEmpty())
		return;

	if (!isValidStorage())
		return;

	QDomElement accountsNode = storage()->point();
	if (accountsNode.isNull())
		return;

	QDomNodeList accountNodes = storage()->storage()->getNodes(accountsNode, "Account");
	int count = accountNodes.count();
	for (int i = 0; i < count; i++)
	{
		QDomElement accountElement = accountNodes.item(i).toElement();
		if (accountElement.isNull())
			continue;

		// TODO hasAccountUUID(uuid) => return
		QString accountProtocolName = storage()->storage()->getTextNode(accountElement, "Protocol");
		if (accountProtocolName.isEmpty() || accountProtocolName != protocolName)
			continue;

		ProtocolFactory *protocolFactory = ProtocolsManager::instance()->protocolFactory(protocolName);
		if (!protocolFactory)
			continue;

		StoragePoint *storagePoint = new StoragePoint(storage()->storage(), accountElement);

		Account *account = protocolFactory->loadAccount(storagePoint);
		registerAccount(account);
	}
}

void AccountManager::storeConfiguration(const QString &protocolName)
{
	if (!isValidStorage())
		return;

	QDomElement accountsNode = storage()->point();

	foreach (Account *account, Accounts)
	{
		if (protocolName.isNull() || account->protocol()->protocolFactory()->name() == protocolName)
		{
			account->storeConfiguration();
			if (!protocolName.isNull())
				unregisterAccount(account);
		}
	}
}

Account * AccountManager::defaultAccount() const
{
	return byIndex(0);
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

	connect(account->protocol(), SIGNAL(connectionError(Account *, const QString &, const QString &)),
			this, SLOT(connectionError(Account *, const QString &, const QString &)));
}

void AccountManager::unregisterAccount(Account *account)
{
	disconnect(account->protocol(), SIGNAL(connectionError(Account *, const QString &, const QString &)),
			this, SLOT(connectionError(Account *, const QString &, const QString &)));

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

void AccountManager::connectionError(Account *account, const QString &server, const QString &message)
{
	kdebugf();

	if (!ConnectionErrorNotification::activeError(account, message))
	{
		ConnectionErrorNotification *connectionErrorNotification = new ConnectionErrorNotification(account,
				server, message);
		NotificationManager::instance()->notify(connectionErrorNotification);
	}

	kdebugf2();
}
