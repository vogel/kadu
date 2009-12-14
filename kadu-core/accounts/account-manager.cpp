/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"
#include "accounts/account-shared.h"
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
		Instance = new AccountManager();

	return Instance;
}

AccountManager::AccountManager()
{
	ConfigurationManager::instance()->registerStorableObject(this);
}

AccountManager::~AccountManager()
{
	ConfigurationManager::instance()->unregisterStorableObject(this);
}

void AccountManager::itemAdded(Account item)
{
	if (item.data())
		item.data()->ensureLoaded();
	AccountsAwareObject::notifyAccountAdded(item);
}

void AccountManager::itemRemoved(Account item)
{
	AccountsAwareObject::notifyAccountRemoved(item);
}

void AccountManager::itemAboutToBeRegistered(Account item)
{
	emit accountAboutToBeRegistered(item);
}

void AccountManager::itemRegisterd(Account item)
{
	AccountsAwareObject::notifyAccountRegistered(item);
	connect(item.protocolHandler(), SIGNAL(connectionError(Account, const QString &, const QString &)),
			this, SLOT(connectionError(Account, const QString &, const QString &)));

	emit accountRegistered(item);
}

void AccountManager::itemAboutToBeUnregisterd(Account item)
{
	AccountsAwareObject::notifyAccountUnregistered(item);
	disconnect(item.protocolHandler(), SIGNAL(connectionError(Account, const QString &, const QString &)),
			this, SLOT(connectionError(Account, const QString &, const QString &)));

	emit accountAboutToBeUnregistered(item);
}

void AccountManager::itemUnregistered(Account item)
{
	emit accountUnregistered(item);
}

void AccountManager::detailsLoaded(Account account)
{
	if (!account.isNull())
		registerItem(account);
}

void AccountManager::detailsUnloaded(Account account)
{
	if (!account.isNull())
		unregisterItem(account);
}

Account AccountManager::defaultAccount()
{
	ensureLoaded();
	return byIndex(0);
}

const QList<Account> AccountManager::byProtocolName(const QString &name)
{
	ensureLoaded();

	QList<Account> list;
	foreach (Account account, allItems())
		if (account.protocolName() == name)
			list.append(account);

	return list;
}

Status AccountManager::status()
{
	Account account = defaultAccount();
	return !account.isNull()
			? account.statusContainer()->status()
			: Status();
}

void AccountManager::connectionError(Account account, const QString &server, const QString &message)
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
