/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef ACCOUNT_SHARED_H
#define ACCOUNT_SHARED_H

#include <QtNetwork/QHostAddress>

#include "network/proxy/network-proxy.h"
#include "status/status.h"
#include "storage/shared.h"

class AccountDetails;
class AccountStatusContainer;
class Contact;
class FileTransferService;
class Identity;
class Protocol;
class ProtocolFactory;
class StatusContainer;

class KADUAPI AccountShared : public QObject, public Shared
{
	Q_OBJECT
	Q_DISABLE_COPY(AccountShared)

	friend class AccountStatusContainer;

	QString ProtocolName;
	Protocol *ProtocolHandler;
	AccountStatusContainer *MyStatusContainer;

	Identity *AccountIdentity;
	Contact *AccountContact;

	AccountDetails *Details;

	QString Id;

	bool RememberPassword;
	bool HasPassword;
	QString Password;

	bool UseDefaultProxy;
	NetworkProxy Proxy;

	bool PrivateStatus;

	void setDisconnectStatus();
	void useProtocolFactory(ProtocolFactory *factory);

	void doSetAccountIdentity(const Identity &accountIdentity);
	void doSetId(const QString &id);

	void importNetworkProxy();

	void loadRosterTasks();
	void storeRosterTasks();

private slots:
	void protocolRegistered(ProtocolFactory *protocolHandler);
	void protocolUnregistered(ProtocolFactory *protocolHandler);

protected:
	virtual void load();
	virtual void store();
	virtual bool shouldStore();

	// TODO: 0.11, fix this
	// hack, changing details does not trigger this
	friend class GaduEditAccountWidget;
	void forceEmitUpdated();

public:
	static AccountShared * loadStubFromStorage(const std::shared_ptr<StoragePoint> &storagePoint);
	static AccountShared * loadFromStorage(const std::shared_ptr<StoragePoint> &storagePoint);

	explicit AccountShared(const QString &protocolName = QString());
	virtual ~AccountShared();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

	virtual void aboutToBeRemoved();

	AccountDetails * details() const { return Details; }

	Contact accountContact();

	StatusContainer * statusContainer();

	KaduShared_PropertyDeclCRW(Identity, accountIdentity, AccountIdentity)

	KaduShared_PropertyRead(const QString &, protocolName, ProtocolName)

	void setId(const QString &id);
	KaduShared_PropertyRead(const QString &, id, Id)

	void setPrivateStatus(bool isPrivate);
	KaduShared_PropertyRead(bool, privateStatus, PrivateStatus)

	KaduShared_PropertyRead(Protocol *, protocolHandler, ProtocolHandler)
	KaduShared_Property(bool, rememberPassword, RememberPassword)
	KaduShared_Property(bool, hasPassword, HasPassword)
	KaduShared_Property(const QString &, password, Password)
	KaduShared_Property(bool, useDefaultProxy, UseDefaultProxy)
	KaduShared_Property(const NetworkProxy &, proxy, Proxy)

	// TODO: 0.11, find better API
	// this is only for GG now
	void fileTransferServiceChanged(FileTransferService *service);

signals:
	void buddyStatusChanged(const Contact &contact, const Status &oldStatus);

	void connected();
	void disconnected();

	void updated();

	// TODO: 0.11, find better API
	// this is only for GG now
	void fileTransferServiceRegistered();
	void fileTransferServiceUnregistered();

};

#endif // ACCOUNT_SHARED_H
