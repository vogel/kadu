/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "remote-status-request-handler.h"

#include "accounts/account-manager.h"
#include "status/status-container-manager.h"
#include "status/status-setter.h"

RemoteStatusRequestHandler::RemoteStatusRequestHandler(QObject *parent) :
		QObject{parent}
{
}

RemoteStatusRequestHandler::~RemoteStatusRequestHandler()
{
}

void RemoteStatusRequestHandler::setAccountManager(AccountManager *accountManager)
{
	m_accountManager = accountManager;
}

void RemoteStatusRequestHandler::setStatusContainerManager(StatusContainerManager *statusContainerManager)
{
	m_statusContainerManager = statusContainerManager;
}

void RemoteStatusRequestHandler::setStatusSetter(StatusSetter *statusSetter)
{
	m_statusSetter = statusSetter;
}

void RemoteStatusRequestHandler::init()
{
	connect(m_accountManager, &AccountManager::accountAdded, this, &RemoteStatusRequestHandler::accountAdded);
	connect(m_accountManager, &AccountManager::accountRemoved, this, &RemoteStatusRequestHandler::accountRemoved);

	for (auto account : m_accountManager->items())
		accountAdded(account);
}

void RemoteStatusRequestHandler::accountAdded(Account account)
{
	connect(account, &AccountShared::remoteStatusChangeRequest, this, &RemoteStatusRequestHandler::remoteStatusChangeRequest);
}

void RemoteStatusRequestHandler::accountRemoved(Account account)
{
	disconnect(account, nullptr, this, nullptr);
}

void RemoteStatusRequestHandler::remoteStatusChangeRequest(Account account, Status requestedStatus)
{
	auto statusContainer = m_statusContainerManager->statusContainerForAccount(account);
	if (statusContainer)
		m_statusSetter->setStatusManually(statusContainer, requestedStatus);
}
