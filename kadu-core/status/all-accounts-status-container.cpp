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

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "icons/kadu-icon.h"
#include "protocols/protocol.h"

#include "all-accounts-status-container.h"

AllAccountsStatusContainer::AllAccountsStatusContainer(QObject *parent) :
		StatusContainer(parent)
{
	triggerAllAccountsRegistered();
}

AllAccountsStatusContainer::~AllAccountsStatusContainer()
{
	triggerAllAccountsUnregistered();
}

void AllAccountsStatusContainer::accountRegistered(Account account)
{
	Accounts.append(account);
	connect(account, SIGNAL(statusUpdated()), this, SIGNAL(statusUpdated()));

	emit statusUpdated();
}

void AllAccountsStatusContainer::accountUnregistered(Account account)
{
	if (Accounts.removeAll(account) > 0)
	{
		disconnect(account, SIGNAL(statusUpdated()), this, SIGNAL(statusUpdated()));
		emit statusUpdated();
	}
}

void AllAccountsStatusContainer::doSetStatus(Status status)
{
	foreach (const Account &account, Accounts)
		if (account)
			account.data()->setStatus(status, false);
}

Status AllAccountsStatusContainer::status()
{
	Account account = AccountManager::bestAccount(Accounts);
	return account ? account.data()->status() : Status();
}

bool AllAccountsStatusContainer::isStatusSettingInProgress()
{
	Account account = AccountManager::bestAccount(Accounts);
	return account ? account.data()->isStatusSettingInProgress() : false;
}

QString AllAccountsStatusContainer::statusDisplayName()
{
	return status().displayName();
}

KaduIcon AllAccountsStatusContainer::statusIcon()
{
	return statusIcon(status());
}

KaduIcon AllAccountsStatusContainer::statusIcon(const Status &status)
{
	Account account = AccountManager::bestAccount(Accounts);
	return account ? account.data()->statusIcon(status) : KaduIcon();
}

KaduIcon AllAccountsStatusContainer::statusIcon(const QString &statusType)
{
	Account account = AccountManager::bestAccount(Accounts);
	return account ? account.data()->statusIcon(statusType) : KaduIcon();
}

QList<StatusType *> AllAccountsStatusContainer::supportedStatusTypes()
{
	Account account = AccountManager::bestAccount(Accounts);
	return account ? account.data()->supportedStatusTypes() : QList<StatusType *>();
}

int AllAccountsStatusContainer::maxDescriptionLength()
{
	Account account = AccountManager::bestAccount(Accounts);
	return account ? account.data()->maxDescriptionLength() : -1;
}

void AllAccountsStatusContainer::setStatus(Status status, bool flush)
{
	foreach (const Account &account, Accounts)
		account.data()->setStatus(status, flush);
}

void AllAccountsStatusContainer::setDescription(const QString &description, bool flush)
{
	foreach (const Account &account, Accounts)
		account.data()->setDescription(description, flush);
}

void AllAccountsStatusContainer::setDefaultStatus(const QString &startupStatus, bool offlineToInvisible,
		const QString &startupDescription, bool StartupLastDescription)
{
	foreach (const Account &account, Accounts)
		account.data()->setDefaultStatus(startupStatus, offlineToInvisible, startupDescription, StartupLastDescription);
}

void AllAccountsStatusContainer::storeStatus(Status status)
{
	foreach (const Account &account, Accounts)
		account.data()->storeStatus(status);
}
