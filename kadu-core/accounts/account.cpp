/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account-details.h"
#include "accounts/account-manager.h"
#include "buddies/buddy-manager.h"
#include "configuration/configuration-file.h"
#include "configuration/xml-configuration-file.h"
#include "contacts/contact.h"
#include "contacts/contact-details.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocols-manager.h"
#include "misc/misc.h"

#include "account.h"

Account Account::null(true);

Account Account::loadFromStorage(StoragePoint *accountStoragePoint)
{
	return Account(AccountShared::loadFromStorage(accountStoragePoint));
}

Account::Account(bool null) :
		SharedBase<AccountShared>(null)
{
}

Account::Account(AccountShared *data) :
		SharedBase<AccountShared>(data)
{
}

Account::Account(const Account &copy) :
		SharedBase<AccountShared>(copy)
{
}

Account::~Account()
{
}

void Account::connectDataSignals()
{
	if (isNull())
		return;

	connect(data(), SIGNAL(buddyStatusChanged(Account, Buddy, Status)),
			this, SIGNAL(buddyStatusChanged(Account, Buddy, Status)));
}

void Account::disconnectDataSignals()
{
	if (isNull())
		return;

	disconnect(data(), SIGNAL(buddyStatusChanged(Account, Buddy, Status)),
			this, SIGNAL(buddyStatusChanged(Account, Buddy, Status)));
}

void Account::loadProtocol(ProtocolFactory *protocolFactory)
{
	printf("loading protocol: %s\n", qPrintable(protocolFactory->name()));
	if (!isNull())
		data()->loadProtocol(protocolFactory);
}

void Account::unloadProtocol()
{
	if (!isNull())
		data()->unloadProtocol();
}

Buddy Account::getBuddyById(const QString& id)
{
	return BuddyManager::instance()->byId(*this, id);
}

Buddy Account::createAnonymous(const QString& id)
{
	if (isNull())
		return Buddy::null;

	Buddy result;
	result.setAnonymous(true);

	ProtocolFactory *protocolFactory = data()->protocolHandler()->protocolFactory();

	Contact contact;
	ContactDetails *details = protocolFactory->createContactDetails(contact);
	contact.setContactAccount(*this);
	contact.setOwnerBuddy(result);
	contact.setId(id);

	if (!contact.isValid())
		return Buddy::null;

	result.addContact(contact);
	return result;
}

void Account::importProxySettings()
{
	if (isNull())
		return;

	Account defaultAccount = AccountManager::instance()->defaultAccount();
	if (!defaultAccount.isNull() && defaultAccount.proxyHost().toString() != "0.0.0.0")
	{
		data()->setUseProxy(defaultAccount.useProxy());
		data()->setProxyHost(defaultAccount.proxyHost());
		data()->setProxyPort(defaultAccount.proxyPort());
		data()->setProxyRequiresAuthentication(defaultAccount.proxyRequiresAuthentication());
		data()->setProxyUser(defaultAccount.proxyUser());
		data()->setProxyPassword(defaultAccount.proxyPassword());
	}
}

KaduSharedBase_PropertyReadDef(Account, StoragePoint *, storage, Storage, 0)
KaduSharedBase_PropertyDef(Account, QString, protocolName, ProtocolName, QString::null)
KaduSharedBase_PropertyDef(Account, Protocol *, protocolHandler, ProtocolHandler, 0)
KaduSharedBase_PropertyDef(Account, AccountDetails *, details, Details, 0)
KaduSharedBase_PropertyDef(Account, QString, name, Name, QString::null)
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
