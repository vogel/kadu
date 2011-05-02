/*
 * %kadu copyright begin%
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ACCOUNT_SHARED_H
#define ACCOUNT_SHARED_H

#include <QtCore/QObject>
#include <QtCore/QSharedData>
#include <QtCore/QUuid>
#include <QtNetwork/QHostAddress>

#include "accounts/account-proxy-settings.h"
#include "contacts/contact.h"
#include "identities/identity.h"
#include "protocols/protocols-aware-object.h"
#include "status/base-status-container.h"
#include "storage/details-holder.h"
#include "storage/shared.h"

class AccountDetails;
class FileTransferService;
class Protocol;
class ProtocolFactory;
class StatusType;

class KADUAPI AccountShared : public BaseStatusContainer, public Shared, public DetailsHolder<AccountDetails>, ProtocolsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(AccountShared)

private:
	QString ProtocolName;
	Protocol *ProtocolHandler;

	Identity AccountIdentity;
	Contact AccountContact;

	QString Id;

	bool RememberPassword;
	bool HasPassword;
	QString Password;

	AccountProxySettings ProxySettings;

	short int MaximumImageSize;

	bool PrivateStatus;
	// TODO: hack, remove at some time
	bool Removing;

	void setDisconnectStatus();
	void useProtocolFactory(ProtocolFactory *factory);

	void doSetAccountIdentity(const Identity &accountIdentity);
	void doSetId(const QString &id);

protected:
	virtual void load();

	// TODO: 0.11, fix this
	// hack, changing details does not trigger this
	friend class GaduEditAccountWidget;
	void emitUpdated();

	virtual void detailsAdded();
	virtual void detailsAboutToBeRemoved();
	virtual void detailsRemoved();

	virtual void protocolRegistered(ProtocolFactory *protocolHandler);
	virtual void protocolUnregistered(ProtocolFactory *protocolHandler);

	virtual void doSetStatus(Status newStatus);

public:
	static AccountShared * loadStubFromStorage(const QSharedPointer<StoragePoint> &storagePoint);
	static AccountShared * loadFromStorage(const QSharedPointer<StoragePoint> &storagePoint);

	explicit AccountShared(const QUuid &uuid = QUuid());
	virtual ~AccountShared();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

	virtual void store();
	virtual bool shouldStore();
	virtual void aboutToBeRemoved();

	Contact accountContact();

	void setAccountIdentity(const Identity &accountIdentity);
	KaduShared_PropertyRead(const Identity &, accountIdentity, AccountIdentity)

	void setProtocolName(const QString &protocolName);
	KaduShared_PropertyRead(const QString &, protocolName, ProtocolName)

	void setId(const QString &id);
	KaduShared_PropertyRead(const QString &, id, Id)

	void setPrivateStatus(bool isPrivate);
	KaduShared_PropertyRead(bool, privateStatus, PrivateStatus)

	KaduShared_Property(Protocol *, protocolHandler, ProtocolHandler)
	KaduShared_Property(bool, rememberPassword, RememberPassword)
	KaduShared_Property(bool, hasPassword, HasPassword)
	KaduShared_Property(const QString &, password, Password)
	KaduShared_Property(const AccountProxySettings &, proxySettings, ProxySettings)
	KaduShared_Property(bool, removing, Removing)

	// StatusContainer implementation

	virtual QString statusContainerName();

	virtual Status status();
	virtual bool isStatusSettingInProgress();
	virtual int maxDescriptionLength();

	virtual QString statusDisplayName();

	virtual KaduIcon statusIcon();
	virtual KaduIcon statusIcon(const Status &status);
	virtual KaduIcon statusIcon(const QString &statusType);

	virtual QList<StatusType *> supportedStatusTypes();

	// TODO: 0.11, find better API
	// this is only for GG now
	void fileTransferServiceChanged(FileTransferService *service);

signals:
	void buddyStatusChanged(const Contact &contact, const Status &oldStatus);
	void protocolLoaded();
	void protocolUnloaded();

	void connected();
	void disconnected();

	void updated();

	// TODO: 0.11, find better API
	// this is only for GG now
	void fileTransferServiceRegistered();
	void fileTransferServiceUnregistered();

};

// for MOC
#include "status/status.h"

#endif // ACCOUNT_SHARED_H
