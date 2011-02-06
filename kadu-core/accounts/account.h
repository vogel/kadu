/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QtCore/QUuid>
#include <QtXml/QDomElement>

#include "accounts/account-shared.h"
#include "buddies/buddy.h"
#include "buddies/buddies-aware-object.h"
#include "status/base-status-container.h"
#include "status/status.h"
#include "storage/shared-base.h"

class AccountDetails;
class AccountProxySettings;
class Protocol;
class ProtocolFactory;
class Status;
class XmlConfigFile;

class KADUAPI Account : public SharedBase<AccountShared>
{
	KaduSharedBaseClass(Account)

public:
	static Account create();
	static Account loadStubFromStorage(const QSharedPointer<StoragePoint> &accountStoragePoint);
	static Account loadFromStorage(const QSharedPointer<StoragePoint> &accountStoragePoint);
	static Account null;

	Account();
	Account(AccountShared *data);
	Account(QObject *data);
	Account(const Account &copy);
	virtual ~Account();

	void importProxySettings();

	/**
	 * Returns appropriate StatusContainer for this Account, i.e. its Identity's
	 * StatusContainer in Simple Mode and this Account's otherwise. If you want to get
	 * status information that is specific for this Account, e.g., non-generic status
	 * icon, you probably want to call something like account.data()->statusIcon()
	 * rather than using this method.
	 */
	StatusContainer * statusContainer();

	KaduSharedBase_Property(Identity, accountIdentity, AccountIdentity)
	KaduSharedBase_PropertyRead(QSharedPointer<StoragePoint>, storage, Storage)
	KaduSharedBase_Property(QString, protocolName, ProtocolName)
	KaduSharedBase_Property(Protocol *, protocolHandler, ProtocolHandler)
	KaduSharedBase_PropertyRead(AccountDetails *, details, Details)
	KaduSharedBase_PropertyRead(Contact, accountContact, AccountContact)
	KaduSharedBase_Property(QString, id, Id)
	KaduSharedBase_Property(bool, rememberPassword, RememberPassword)
	KaduSharedBase_Property(bool, hasPassword, HasPassword)
	KaduSharedBase_Property(QString, password, Password)
	KaduSharedBase_Property(AccountProxySettings, proxySettings, ProxySettings)
	KaduSharedBase_Property(bool, privateStatus, PrivateStatus)
	KaduSharedBase_Property(bool, removing, Removing)

};

Q_DECLARE_METATYPE(Account)

#endif // ACCOUNT_H
