/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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
#include "configuration/main-configuration-holder.h"
#include "configuration/xml-configuration-file.h"
#include "contacts/contact-details.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "identities/identity.h"
#include "misc/misc.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol.h"
#include "protocols/protocols-manager.h"

#include "account.h"

KaduSharedBaseClassImpl(Account)

Account Account::null;

Account Account::create(const QString &protocolName)
{
	return new AccountShared(protocolName);
}

Account Account::loadStubFromStorage(const std::shared_ptr<StoragePoint> &accountStoragePoint)
{
	return AccountShared::loadStubFromStorage(accountStoragePoint);
}

Account Account::loadFromStorage(const std::shared_ptr<StoragePoint> &accountStoragePoint)
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
	AccountShared *shared = qobject_cast<AccountShared *>(data);
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

StatusContainer * Account::statusContainer() const
{
	if (!data())
		return 0;
	else
		return data()->statusContainer();
}

KaduSharedBase_PropertyDefCRW(Account, Identity, accountIdentity, AccountIdentity, Identity::null)
KaduSharedBase_PropertyReadDef(Account, std::shared_ptr<StoragePoint>, storage, Storage, std::shared_ptr<StoragePoint>())
KaduSharedBase_PropertyReadDef(Account, QString, protocolName, ProtocolName, QString())
KaduSharedBase_PropertyReadDef(Account, Protocol *, protocolHandler, ProtocolHandler, 0)
KaduSharedBase_PropertyReadDef(Account, AccountDetails *, details, Details, 0)
KaduSharedBase_PropertyReadDef(Account, Contact, accountContact, AccountContact, Contact::null)
KaduSharedBase_PropertyDefCRW(Account, QString, id, Id, QString())
KaduSharedBase_PropertyDef(Account, bool, rememberPassword, RememberPassword, true)
KaduSharedBase_PropertyDef(Account, bool, hasPassword, HasPassword, false)
KaduSharedBase_PropertyDefCRW(Account, QString, password, Password, QString())
KaduSharedBase_PropertyDef(Account, bool, useDefaultProxy, UseDefaultProxy, true)
KaduSharedBase_PropertyDefCRW(Account, NetworkProxy, proxy, Proxy, NetworkProxy::null)
KaduSharedBase_PropertyDef(Account, bool, privateStatus, PrivateStatus, true)
