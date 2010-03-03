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

class QPixmap;

class AccountDetails;
class Protocol;
class ProtocolFactory;
class Status;
class XmlConfigFile;

class KADUAPI Account : public SharedBase<AccountShared>
{
	KaduSharedBaseClass(Account)

public:
	static Account create();
	static Account loadFromStorage(StoragePoint *storage);
	static Account null;

	Account();
	Account(AccountShared *data);
	Account(QObject *data);
	Account(const Account &copy);
	virtual ~Account();

	void importProxySettings();

	StatusContainer * statusContainer() { return data(); }

	KaduSharedBase_Property(Identity, accountIdentity, AccountIdentity)
	KaduSharedBase_PropertyRead(StoragePoint *, storage, Storage)
	KaduSharedBase_Property(QString, protocolName, ProtocolName)
	KaduSharedBase_Property(Protocol *, protocolHandler, ProtocolHandler)
	KaduSharedBase_Property(AccountDetails *, details, Details)
	KaduSharedBase_PropertyRead(Contact, accountContact, AccountContact)
	KaduSharedBase_Property(QString, id, Id)
	KaduSharedBase_Property(bool, rememberPassword, RememberPassword)
	KaduSharedBase_Property(bool, hasPassword, HasPassword)
	KaduSharedBase_Property(QString, password, Password)
	KaduSharedBase_Property(bool, connectAtStart, ConnectAtStart)
	KaduSharedBase_Property(bool, useProxy, UseProxy)
	KaduSharedBase_Property(QHostAddress, proxyHost, ProxyHost)
	KaduSharedBase_Property(short int, proxyPort, ProxyPort)
	KaduSharedBase_Property(bool, proxyRequiresAuthentication, ProxyRequiresAuthentication)
	KaduSharedBase_Property(QString, proxyUser, ProxyUser)
	KaduSharedBase_Property(QString, proxyPassword, ProxyPassword)

};

Q_DECLARE_METATYPE(Account)

#endif // ACCOUNT_H
