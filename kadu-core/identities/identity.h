/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef IDENTITY_H
#define IDENTITY_H

#include <QtCore/QUuid>
#include <QtCore/QStringList>

#include "accounts/accounts-aware-object.h"
#include "status/base-status-container.h"

#include "exports.h"

class QPixmap;

class Account;
class Status;

class KADUAPI Identity : public BaseStatusContainer, public AccountsAwareObject
{
	Q_OBJECT

	QString Name;
	QUuid Uuid;

	QList<Account *> Accounts;
	QStringList AccountsUuids;

public:
	static Identity * loadFromStorage(StoragePoint *groupStoragePoint);

	explicit Identity(StoragePoint *storagePoint);
	explicit Identity(const QUuid &uuid = QUuid());
	virtual ~Identity();

	virtual void load();
	virtual void store();

	virtual QUuid uuid() const { return Uuid; }

	void setName(const QString &name) { Name = name; }

	QString name() const { return Name; }

	QList<Account *> accounts() const { return Accounts; }
	bool hasAccount(Account *account) const { return Accounts.contains(account); }

	void accountRegistered(Account *account);
	void accountUnregistered(Account *account);

	void addAccount(Account *account);

	// StatusContainer implementation
	virtual QString statusContainerName() { return Name; }

	virtual void setStatus(Status newStatus);
	virtual Status status();

	virtual QString statusName();
	virtual QPixmap statusPixmap();
	virtual QPixmap statusPixmap(Status status);
	virtual QPixmap statusPixmap(const QString &statusType);

	virtual QList<StatusType *> supportedStatusTypes();

	virtual int maxDescriptionLength();

	virtual void setPrivateStatus(bool isPrivate);


public slots:
	void removeAccount(Account *account);

};

#endif // IDENTITY_H
