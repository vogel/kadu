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
#include "configuration/configuration-file.h"
#include "configuration/configuration-manager.h"
#include "configuration/xml-configuration-file.h"
#include "core/core.h"
#include "notify/notification-manager.h"
#include "protocols/connection-error-notification.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocols-manager.h"
#include "debug.h"

#include "account-manager.h"

AccountManager * AccountManager::Instance = 0;

KADUAPI AccountManager * AccountManager::instance()
{
	if (0 == Instance)
	{
		Instance = new AccountManager();
		Instance->init();
	}
	return Instance;
}

AccountManager::AccountManager() :
		StorableObject(true)
{
	Core::instance()->configuration()->registerStorableObject(this);
}

AccountManager::~AccountManager()
{
	foreach (ProtocolFactory *factory, ProtocolsManager::instance()->protocolFactories())
		protocolFactoryUnregistered(factory);

	disconnect(ProtocolsManager::instance(), SIGNAL(protocolFactoryRegistered(ProtocolFactory *)),
			this, SLOT(protocolFactoryRegistered(ProtocolFactory *)));
	disconnect(ProtocolsManager::instance(), SIGNAL(protocolFactoryUnregistered(ProtocolFactory *)),
			this, SLOT(protocolFactoryUnregistered(ProtocolFactory *)));

	Core::instance()->configuration()->unregisterStorableObject(this);
}

void AccountManager::init()
{
	connect(ProtocolsManager::instance(), SIGNAL(protocolFactoryRegistered(ProtocolFactory *)),
			this, SLOT(protocolFactoryRegistered(ProtocolFactory *)));
	connect(ProtocolsManager::instance(), SIGNAL(protocolFactoryUnregistered(ProtocolFactory *)),
			this, SLOT(protocolFactoryUnregistered(ProtocolFactory *)));

	foreach (ProtocolFactory *factory, ProtocolsManager::instance()->protocolFactories())
		protocolFactoryRegistered(factory);
}

StoragePoint * AccountManager::createStoragePoint()
{
	return new StoragePoint(xml_config_file, xml_config_file->getNode("Accounts"));
}

void AccountManager::load()
{
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

		StoragePoint *storagePoint = new StoragePoint(storage()->storage(), accountElement);
		Account *account = Account::loadFromStorage(storagePoint);
		NonRegisteredAccounts.append(account);

		ProtocolFactory *factory = ProtocolsManager::instance()->byName(account->protocolName());
		if (factory)
		{
			account->loadProtocol(factory);
			registerAccount(account);
		}
	}
}

void AccountManager::store()
{
	if (!isValidStorage())
		return;
	
	foreach (Account *account, Accounts)
		account->store();
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

void AccountManager::deleteAccount(Account *account)
{
	emit accountAboutToBeRemoved(account);
	unregisterAccount(account);
	account->removeFromStorage();
	emit accountRemoved(account);
}

Status AccountManager::status() const
{
	Account *account = defaultAccount();
	return account
		? account->status()
		: Status();
}

void AccountManager::protocolFactoryRegistered(ProtocolFactory *factory)
{
	printf("pf registered: %s\n", qPrintable(factory->name()));

	if (!isValidStorage())
		return;

	QString factoryProtocolName = factory->name();

	foreach (Account *account, NonRegisteredAccounts)
	{
		if (account->protocolName() == factoryProtocolName)
		{
			account->loadProtocol(factory);
			registerAccount(account);
			printf("registered account\n");
		}
	}
}

void AccountManager::protocolFactoryUnregistered(ProtocolFactory *factory)
{
	foreach (Account *account, Accounts)
		if (account->protocol()->protocolFactory() == factory)
		{
			unregisterAccount(account);
			account->unloadProtocol();
		}
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
