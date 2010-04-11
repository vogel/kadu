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

Account Account::loadStubFromStorage(StoragePoint *accountStoragePoint)
{
	return AccountShared::loadStubFromStorage(accountStoragePoint);
}

Account Account::loadFromStorage(StoragePoint *accountStoragePoint)
{
	return AccountShared::loadFromStorage(accountStoragePoint);
}

Account::Account()
{
}

Account::Account(AccountShared *data) :
		SharedBase<AccountShared>(data)
{
	data->ref.ref();
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
	if (defaultAccount && defaultAccount.proxyHost().toString() != "0.0.0.0")
	{
		data()->setUseProxy(defaultAccount.useProxy());
		data()->setProxyHost(defaultAccount.proxyHost());
		data()->setProxyPort(defaultAccount.proxyPort());
		data()->setProxyRequiresAuthentication(defaultAccount.proxyRequiresAuthentication());
		data()->setProxyUser(defaultAccount.proxyUser());
		data()->setProxyPassword(defaultAccount.proxyPassword());
	}
}

KaduSharedBase_PropertyDef(Account, Identity, accountIdentity, AccountIdentity, Identity::null)
KaduSharedBase_PropertyReadDef(Account, StoragePoint *, storage, Storage, 0)
KaduSharedBase_PropertyDef(Account, QString, protocolName, ProtocolName, QString::null)
KaduSharedBase_PropertyDef(Account, Protocol *, protocolHandler, ProtocolHandler, 0)
KaduSharedBase_PropertyReadDef(Account, AccountDetails *, details, Details, 0)
KaduSharedBase_PropertyReadDef(Account, Contact, accountContact, AccountContact, Contact::null)
KaduSharedBase_PropertyDef(Account, QString, id, Id, QString::null)
KaduSharedBase_PropertyDef(Account, bool, rememberPassword, RememberPassword, true)
KaduSharedBase_PropertyDef(Account, bool, hasPassword, HasPassword, false)
KaduSharedBase_PropertyDef(Account, QString, password, Password, QString::null)
KaduSharedBase_PropertyDef(Account, bool, connectAtStart, ConnectAtStart, true)
KaduSharedBase_PropertyDef(Account, bool, useProxy, UseProxy, false)
KaduSharedBase_PropertyDef(Account, QHostAddress, proxyHost, ProxyHost, QHostAddress())
KaduSharedBase_PropertyDef(Account, short int, proxyPort, ProxyPort, 0)
KaduSharedBase_PropertyDef(Account, bool, proxyRequiresAuthentication, ProxyRequiresAuthentication, false)
KaduSharedBase_PropertyDef(Account, QString, proxyUser, ProxyUser, QString::null)
KaduSharedBase_PropertyDef(Account, QString, proxyPassword, ProxyPassword, QString::null)
