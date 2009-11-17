/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QtCore/QUuid>
#include <QtXml/QDomElement>

#include "accounts/account-shared.h"
#include "buddies/buddy.h"
#include "buddies/buddies-aware-object.h"
#include "shared/shared-base.h"
#include "status/base-status-container.h"
#include "status/status.h"

class QPixmap;

class AccountDetails;
class Protocol;
class ProtocolFactory;
class Status;
class XmlConfigFile;

class KADUAPI Account : public SharedBase<AccountShared>
{
	Account(bool null);

public:
	static Account loadFromStorage(StoragePoint *storage);
	static Account null;

	Account();
	explicit Account(AccountShared *data);
	Account(const Account &copy);
	virtual ~Account();

	Account & operator = (const Account &copy)
	{
		SharedBase<AccountShared>::operator=(copy);
		return *this;
	}

	void importProxySettings();

	Buddy getBuddyById(const QString &id);
	Buddy createAnonymous(const QString &id);

	StatusContainer * statusContainer() { return data(); }

	KaduSharedBase_PropertyRead(StoragePoint *, storage, Storage)
	KaduSharedBase_Property(QString, protocolName, ProtocolName)
	KaduSharedBase_Property(Protocol *, protocolHandler, ProtocolHandler)
	KaduSharedBase_Property(AccountDetails *, details, Details)
	KaduSharedBase_Property(QString, name, Name)
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
