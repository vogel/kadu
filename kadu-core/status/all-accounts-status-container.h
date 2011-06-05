/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ALL_ACCOUNTS_STATUS_CONTAINER_H
#define ALL_ACCOUNTS_STATUS_CONTAINER_H

#include "accounts/accounts-aware-object.h"
#include "status/status-container.h"

class Account;

class KADUAPI AllAccountsStatusContainer : public StatusContainer, public AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(AllAccountsStatusContainer)

	QList<Account> Accounts;

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

	virtual void doSetStatus(Status newStatus);

public:
	explicit AllAccountsStatusContainer(QObject *parent = 0);
	virtual ~AllAccountsStatusContainer();

	virtual QString statusContainerName()  { return QString(); }

	virtual void setStatus(Status newStatus, bool flush = true);
	virtual Status status();
	virtual bool isStatusSettingInProgress();
	virtual void setDescription(const QString &description, bool flush = true);

	virtual int maxDescriptionLength();

	virtual QString statusDisplayName();

	virtual KaduIcon statusIcon();
	virtual KaduIcon statusIcon(const Status &status);
	virtual KaduIcon statusIcon(const QString &statusType);

	virtual QList<StatusType *> supportedStatusTypes();

	virtual void setDefaultStatus(const QString &startupStatus, bool offlineToInvisible,
				      const QString &startupDescription, bool StartupLastDescription);
	virtual void storeStatus(Status status);

};

#endif // ALL_ACCOUNTS_STATUS_CONTAINER_H
