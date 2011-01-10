/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "accounts/account-details.h"
#include "accounts/account-manager.h"
#include "buddies/buddy-manager.h"
#include "configuration/configuration-file.h"
#include "configuration/main-configuration.h"
#include "configuration/xml-configuration-file.h"
#include "contacts/contact.h"
#include "contacts/contact-details.h"
#include "contacts/contact-manager.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocols-manager.h"
#include "misc/misc.h"

#include "account.h"

KaduSharedBaseClassImpl(Account)

Account Account::null;

Account Account::create()
{
	return new AccountShared();
}

Account Account::loadStubFromStorage(const QSharedPointer<StoragePoint> &accountStoragePoint)
{
	return AccountShared::loadStubFromStorage(accountStoragePoint);
}

Account Account::loadFromStorage(const QSharedPointer<StoragePoint> &accountStoragePoint)
{
	return AccountShared::loadFromStorage(accountStoragePoint);
}

Account::Account()
{
}

Account::Account(AccountShared *data) :
		SharedBase<AccountShared>(data)
{
}

Account::Account(QObject *data)
{
	AccountShared *shared = dynamic_cast<AccountShared *>(data);
	if (shared)
		setData(shared);
}

Account::Account(const Account &copy) :
		SharedBase<AccountShared>(copy)
{
}

Account::~Account()
{
}

void Account::importProxySettings()
{
	if (isNull())
		return;

	Account defaultAccount = AccountManager::instance()->defaultAccount();
	if (defaultAccount)
		data()->setProxySettings(defaultAccount.proxySettings()); // data is copied
}

StatusContainer * Account::statusContainer()
{
	if (MainConfiguration::instance()->simpleMode())
		return accountIdentity().data();
	else
		return data();
}

KaduSharedBase_PropertyDef(Account, Identity, accountIdentity, AccountIdentity, Identity::null)
KaduSharedBase_PropertyReadDef(Account, QSharedPointer<StoragePoint>, storage, Storage, QSharedPointer<StoragePoint>())
KaduSharedBase_PropertyDef(Account, QString, protocolName, ProtocolName, QString())
KaduSharedBase_PropertyDef(Account, Protocol *, protocolHandler, ProtocolHandler, 0)
KaduSharedBase_PropertyReadDef(Account, AccountDetails *, details, Details, 0)
KaduSharedBase_PropertyReadDef(Account, Contact, accountContact, AccountContact, Contact::null)
KaduSharedBase_PropertyDef(Account, QString, id, Id, QString())
KaduSharedBase_PropertyDef(Account, bool, rememberPassword, RememberPassword, true)
KaduSharedBase_PropertyDef(Account, bool, hasPassword, HasPassword, false)
KaduSharedBase_PropertyDef(Account, QString, password, Password, QString())
KaduSharedBase_PropertyDef(Account, AccountProxySettings, proxySettings, ProxySettings, AccountProxySettings())
KaduSharedBase_PropertyDef(Account, bool, privateStatus, PrivateStatus, true)
