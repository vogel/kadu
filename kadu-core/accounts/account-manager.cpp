/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include "accounts/account.h"
#include "accounts/account-shared.h"
#include "accounts/accounts-aware-object.h"
#include "buddies/buddy-manager.h"
#include "configuration/configuration-file.h"
#include "configuration/configuration-manager.h"
#include "configuration/xml-configuration-file.h"
#include "contacts/contact-manager.h"
#include "core/core.h"
#include "gui/windows/password-window.h"
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

	// needed for QueuedConnection
	qRegisterMetaType<Account>("Account");
}

AccountManager::~AccountManager()
{
	ConfigurationManager::instance()->unregisterStorableObject(this);
}

void AccountManager::itemAdded(Account item)
{
	QMutexLocker(&mutex());

	if (item.data())
		item.data()->ensureLoaded();
	AccountsAwareObject::notifyAccountAdded(item);
}

void AccountManager::itemAboutToBeRemoved(Account item)
{
	QMutexLocker(&mutex());

	Manager<Account>::itemAboutToBeRemoved(item);

	item.setAccountIdentity(Identity::null);
}

void AccountManager::itemRemoved(Account item)
{
	QMutexLocker(&mutex());

	AccountsAwareObject::notifyAccountRemoved(item);
}

void AccountManager::itemAboutToBeRegistered(Account item)
{
	QMutexLocker(&mutex());

	connect(item, SIGNAL(updated()), this, SLOT(accountDataUpdated()));
	emit accountAboutToBeRegistered(item);
}

void AccountManager::itemRegistered(Account item)
{
	QMutexLocker(&mutex());

	AccountsAwareObject::notifyAccountRegistered(item);

	/* NOTE: We need QueuedConnection here so when the protocol emits the signal, it can cleanup
	 * itself before we do something (e.g., reset connection data after invalidPassword, so when
	 * we try to log in after entering new password, a new connection can be estabilished instead
	 * of giving up because of already existing connection).
	 */
	connect(item.protocolHandler(), SIGNAL(connectionError(Account, const QString &, const QString &)),
			this, SLOT(connectionError(Account, const QString &, const QString &)), Qt::QueuedConnection);
	connect(item.protocolHandler(), SIGNAL(invalidPassword(Account)),
			this, SLOT(invalidPassword(Account)), Qt::QueuedConnection);

	emit accountRegistered(item);
}

void AccountManager::itemAboutToBeUnregisterd(Account item)
{
	QMutexLocker(&mutex());

	AccountsAwareObject::notifyAccountUnregistered(item);
	disconnect(item.protocolHandler(), SIGNAL(connectionError(Account, const QString &, const QString &)),
			this, SLOT(connectionError(Account, const QString &, const QString &)));
	disconnect(item.protocolHandler(), SIGNAL(invalidPassword(Account)),
			this, SLOT(invalidPassword(Account)));

	emit accountAboutToBeUnregistered(item);
}

void AccountManager::itemUnregistered(Account item)
{
	QMutexLocker(&mutex());

	disconnect(item, SIGNAL(updated()), this, SLOT(accountDataUpdated()));
	emit accountUnregistered(item);
}

void AccountManager::detailsLoaded(Account account)
{
	QMutexLocker(&mutex());

	if (!account.isNull())
		registerItem(account);
}

void AccountManager::detailsUnloaded(Account account)
{
	QMutexLocker(&mutex());

	if (!account.isNull())
		unregisterItem(account);
}

Account AccountManager::defaultAccount()
{
	QMutexLocker(&mutex());

	ensureLoaded();
	return byIndex(0);
}

const QList<Account> AccountManager::byIdentity(Identity identity)
{
	QMutexLocker(&mutex());

	ensureLoaded();

	QList<Account> list;
	foreach (const Account &account, allItems())
		if (account.accountIdentity() == identity)
			list.append(account);

	return list;
}

Account AccountManager::byId(const QString& protocolName, const QString& id)
{
	QMutexLocker(&mutex());

	ensureLoaded();

	foreach (const Account &account, allItems())
		if (account.protocolName() == protocolName && account.id() == id)
			return account;

	return Account::null;
}

const QList<Account> AccountManager::byProtocolName(const QString &name)
{
	QMutexLocker(&mutex());

	ensureLoaded();

	QList<Account> list;
	foreach (const Account &account, allItems())
		if (account.protocolName() == name)
			list.append(account);

	return list;
}

Status AccountManager::status()
{
	QMutexLocker(&mutex());

	Account account = defaultAccount();
	return !account.isNull()
			? account.statusContainer()->status()
			: Status();
}

void AccountManager::accountDataUpdated()
{
	QMutexLocker(&mutex());

	Account account(sender());
	if (account)
		emit accountUpdated(account);
}

void AccountManager::connectionError(Account account, const QString &server, const QString &message)
{
	QMutexLocker(&mutex());

	if (!ConnectionErrorNotification::activeError(account, message))
	{
		ConnectionErrorNotification *connectionErrorNotification = new ConnectionErrorNotification(account,
				server, message);
		NotificationManager::instance()->notify(connectionErrorNotification);
	}
}

void AccountManager::invalidPassword(Account account)
{
	QMutexLocker(&mutex());

	QString message = tr("Please provide valid password for %1 (%2) account")
			.arg(account.accountIdentity().name())
			.arg(account.id());
	PasswordWindow::getPassword(message, account.protocolHandler(), SLOT(login(const QString &, bool)));
}

void AccountManager::removeAccountAndBuddies(Account account)
{
	account.setRemoving(true);

	QList<Contact> contacts = ContactManager::instance()->contacts(account);
	foreach (const Contact &contact, contacts)
		BuddyManager::instance()->clearOwnerAndRemoveEmptyBuddy(contact);

	removeItem(account);
}

void AccountManager::loaded()
{
	foreach (const Account &account, allItems())
		account.accountContact().setOwnerBuddy(Core::instance()->myself());
}
