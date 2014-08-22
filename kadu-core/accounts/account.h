/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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
#include "storage/shared-base.h"

class AccountDetails;
class Contact;
class Identity;
class NetworkProxy;
class Protocol;
class ProtocolFactory;
class XmlConfigFile;

class KADUAPI Account : public SharedBase<AccountShared>
{
	KaduSharedBaseClass(Account)

public:
	static Account create(const QString &protocolName);
	static Account loadStubFromStorage(const std::shared_ptr<StoragePoint> &accountStoragePoint);
	static Account loadFromStorage(const std::shared_ptr<StoragePoint> &accountStoragePoint);
	static Account null;

	Account();
	Account(AccountShared *data);
	explicit Account(QObject *data);
	Account(const Account &copy);
	virtual ~Account();

	StatusContainer * statusContainer() const;

	KaduSharedBase_PropertyCRW(Identity, accountIdentity, AccountIdentity)
	KaduSharedBase_PropertyRead(std::shared_ptr<StoragePoint>, storage, Storage)
	KaduSharedBase_PropertyRead(QString, protocolName, ProtocolName)
	KaduSharedBase_PropertyRead(Protocol *, protocolHandler, ProtocolHandler)
	KaduSharedBase_PropertyRead(AccountDetails *, details, Details)
	KaduSharedBase_PropertyRead(Contact, accountContact, AccountContact)
	KaduSharedBase_PropertyCRW(QString, id, Id)
	KaduSharedBase_Property(bool, rememberPassword, RememberPassword)
	KaduSharedBase_Property(bool, hasPassword, HasPassword)
	KaduSharedBase_PropertyCRW(QString, password, Password)
	KaduSharedBase_Property(bool, useDefaultProxy, UseDefaultProxy)
	KaduSharedBase_PropertyCRW(NetworkProxy, proxy, Proxy)
	KaduSharedBase_Property(bool, privateStatus, PrivateStatus)

};

Q_DECLARE_METATYPE(Account)

#endif // ACCOUNT_H
