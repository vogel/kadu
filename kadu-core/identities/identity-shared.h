/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef IDENTITY_SHARED_H
#define IDENTITY_SHARED_H

#include <QtCore/QObject>
#include <QtCore/QSharedData>
#include <QtCore/QUuid>

#include "accounts/account.h"
#include "accounts/accounts-aware-object.h"
#include "configuration/uuid-storable-object.h"
#include "identity-manager.h"
#include "status/base-status-container.h"

#undef Property
#define Property(type, name, capitalized_name) \
	type name() { ensureLoaded(); return capitalized_name; } \
	void set##capitalized_name(type name) { ensureLoaded(); capitalized_name = name; }

class IdentityDetails;
class Buddy;
class Protocol;
class ProtocolFactory;

class KADUAPI IdentityShared : public BaseStatusContainer, public UuidStorableObject, public AccountsAwareObject, public QSharedData
{
	Q_OBJECT
	Q_DISABLE_COPY(IdentityShared)
	
	QString Name;
	QUuid Uuid;

	QList<Account> Accounts;
	QStringList AccountsUuids;

public:
	static IdentityShared * loadFromStorage(StoragePoint *accountStoragePoint);

	explicit IdentityShared(const QUuid &uuid = QUuid());
	explicit IdentityShared(StoragePoint *storagePoint);
	virtual ~IdentityShared();

	virtual void load();
	virtual void store();

	virtual QUuid uuid() const { return Uuid; }
	void setUuid(const QUuid uuid) { Uuid = uuid; }

	void accountRegistered(Account account);
	void accountUnregistered(Account account);
	void addAccount(Account account);
	
	Property(QString, name, Name)
	Property(QList<Account>, accounts, Accounts)
	Property(QStringList, accountsUuids, AccountsUuids)

	// StatusContainer implementation

	virtual QString statusContainerName()  { return name(); };

	virtual void setStatus(Status newStatus);
	virtual const Status & status();
	virtual int maxDescriptionLength();

	virtual QString statusName();
	virtual QPixmap statusPixmap();
	virtual QPixmap statusPixmap(const QString &statusType);

	virtual QList<StatusType *> supportedStatusTypes();

	QPixmap statusPixmap(Status status);

	virtual void setPrivateStatus(bool isPrivate);
};

#include "buddies/buddy.h" // for MOC

#endif // IDENTITY_SHARED_H
