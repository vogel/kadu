/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011, 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "core/core.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "identities/identity-manager.h"
#include "protocols/protocol.h"
#include "status/all-accounts-status-container.h"
#include "status/status-configuration-holder.h"
#include "status/status-container-aware-object.h"
#include "status/status-type-manager.h"
#include "status/status-type.h"

#include "status-container-manager.h"

StatusContainerManager * StatusContainerManager::Instance = 0;

StatusContainerManager * StatusContainerManager::instance()
{
	if (!Instance)
		Instance = new StatusContainerManager();
	return Instance;
}

StatusContainerManager::StatusContainerManager() :
		StatusContainer(0), DefaultStatusContainer(0)
{
	AllAccountsContainer = new AllAccountsStatusContainer(this);

	if (StatusConfigurationHolder::instance()->isSetStatusPerIdentity())
		triggerAllIdentitiesAdded();
	else if (StatusConfigurationHolder::instance()->isSetStatusPerAccount())
		triggerAllAccountsRegistered();
	else
		registerStatusContainer(AllAccountsContainer);

	connect(StatusConfigurationHolder::instance(), SIGNAL(setStatusModeChanged()), this, SLOT(setStatusModeChanged()));
	connect(AccountManager::instance(), SIGNAL(accountUpdated(Account)), this, SLOT(updateIdentities()));
}

StatusContainerManager::~StatusContainerManager()
{
	disconnect(AccountManager::instance(), 0, this, 0);
	disconnect(StatusConfigurationHolder::instance(), 0, this, 0);

	if (StatusConfigurationHolder::instance()->isSetStatusPerIdentity())
		triggerAllIdentitiesRemoved();
	else if (StatusConfigurationHolder::instance()->isSetStatusPerAccount())
		triggerAllAccountsUnregistered();
	else
		unregisterStatusContainer(AllAccountsContainer);
}

void StatusContainerManager::updateIdentities()
{
	if (!StatusConfigurationHolder::instance()->isSetStatusPerIdentity())
		return;

	foreach (const Identity &identity, IdentityManager::instance()->items())
		if (StatusContainers.contains(identity) && !identity.hasAnyAccountWithDetails())
			unregisterStatusContainer(identity);
		else if (!StatusContainers.contains(identity) && identity.hasAnyAccountWithDetails())
			registerStatusContainer(identity);
}

void StatusContainerManager::accountRegistered(Account account)
{
	if (StatusConfigurationHolder::instance()->isSetStatusPerAccount() && !StatusContainers.contains(account.statusContainer()))
		registerStatusContainer(account.statusContainer());

	if (StatusConfigurationHolder::instance()->isSetStatusPerIdentity() && !StatusContainers.contains(account.accountIdentity()))
		updateIdentities();
}

void StatusContainerManager::accountUnregistered(Account account)
{
	if (StatusConfigurationHolder::instance()->isSetStatusPerAccount() && StatusContainers.contains(account.statusContainer()))
		unregisterStatusContainer(account.statusContainer());

	if (StatusConfigurationHolder::instance()->isSetStatusPerIdentity())
		updateIdentities();
}

void StatusContainerManager::identityAdded(Identity identity)
{
	if (StatusConfigurationHolder::instance()->isSetStatusPerIdentity() && !StatusContainers.contains(identity) && identity.hasAnyAccountWithDetails())
		registerStatusContainer(identity);
}

void StatusContainerManager::identityRemoved(Identity identity)
{
	if (StatusConfigurationHolder::instance()->isSetStatusPerIdentity() && StatusContainers.contains(identity))
		unregisterStatusContainer(identity);
}

void StatusContainerManager::cleanStatusContainers()
{
	while (!StatusContainers.isEmpty())
		unregisterStatusContainer(StatusContainers.at(0));
}

void StatusContainerManager::addAllAccounts()
{
	foreach (Account account, AccountManager::instance()->items())
		registerStatusContainer(account.statusContainer());
}

void StatusContainerManager::addAllIdentities()
{
	updateIdentities();
}

void StatusContainerManager::setDefaultStatusContainer(StatusContainer *defaultStatusContainer)
{
	if (defaultStatusContainer == DefaultStatusContainer)
		return;

	if (DefaultStatusContainer)
		disconnect(DefaultStatusContainer, 0, this, 0);

	if (this != defaultStatusContainer)
		DefaultStatusContainer = defaultStatusContainer;
	else
		DefaultStatusContainer = 0;

	if (DefaultStatusContainer)
		connect(DefaultStatusContainer, SIGNAL(statusUpdated(StatusContainer *)), this, SIGNAL(statusUpdated(StatusContainer *)));

	emit statusUpdated(this);
}

void StatusContainerManager::setStatusModeChanged()
{
	cleanStatusContainers();
	if (StatusConfigurationHolder::instance()->isSetStatusPerIdentity())
		addAllIdentities();
	else if (StatusConfigurationHolder::instance()->isSetStatusPerAccount())
		addAllAccounts();
	else
		registerStatusContainer(AllAccountsContainer);
}

void StatusContainerManager::registerStatusContainer(StatusContainer *statusContainer)
{
	if (StatusContainers.isEmpty())
		setDefaultStatusContainer(statusContainer);

	emit statusContainerAboutToBeRegistered(statusContainer);
	StatusContainers.append(statusContainer);
	emit statusContainerRegistered(statusContainer);
	StatusContainerAwareObject::notifyStatusContainerRegistered(statusContainer);

	connect(statusContainer, SIGNAL(statusUpdated(StatusContainer *)), this, SIGNAL(statusUpdated(StatusContainer *)));
}

void StatusContainerManager::unregisterStatusContainer(StatusContainer *statusContainer)
{
	emit statusContainerAboutToBeUnregistered(statusContainer);
	StatusContainers.removeAll(statusContainer);
	emit statusContainerUnregistered(statusContainer);
	StatusContainerAwareObject::notifyStatusContainerUnregistered(statusContainer);

	if (statusContainer == DefaultStatusContainer)
	{
		if (StatusContainers.isEmpty())
			setDefaultStatusContainer(0);
		else
			setDefaultStatusContainer(StatusContainers.at(0));
	}

	disconnect(statusContainer, 0, this, 0);
}

bool StatusContainerManager::allStatusOfType(StatusType type)
{
	if (StatusTypeNone == type)
		return false;

	foreach (StatusContainer *container, StatusContainers)
		if (container->status().type() != type)
			return false;
	return true;
}

QString StatusContainerManager::statusContainerName()
{
	return tr("All");
}

void StatusContainerManager::setStatus(Status status, StatusChangeSource source)
{
	foreach (StatusContainer *container, StatusContainers)
		container->setStatus(status, source);
}

Status StatusContainerManager::status()
{
	return DefaultStatusContainer
			? DefaultStatusContainer->status()
			: Status();
}

bool StatusContainerManager::isStatusSettingInProgress()
{
	return DefaultStatusContainer
			? DefaultStatusContainer->isStatusSettingInProgress()
			: false;
}

KaduIcon StatusContainerManager::statusIcon()
{
	return statusIcon(status());
}

KaduIcon StatusContainerManager::statusIcon(const Status &status)
{
	if (!DefaultStatusContainer)
		return StatusTypeManager::instance()->statusIcon("common", StatusTypeOffline);

	return StatusTypeManager::instance()->statusIcon("common", status);
}

QList<StatusType> StatusContainerManager::supportedStatusTypes()
{
	return DefaultStatusContainer
			? DefaultStatusContainer->supportedStatusTypes()
			: QList<StatusType>();
}

int StatusContainerManager::maxDescriptionLength()
{
	return DefaultStatusContainer
			? DefaultStatusContainer->maxDescriptionLength()
			: -1;
}

QString StatusContainerManager::statusNamePrefix()
{
	return tr("All") + ' ';
}

Status StatusContainerManager::loadStatus()
{
	return DefaultStatusContainer
			? DefaultStatusContainer->loadStatus()
			: Status();
}

void StatusContainerManager::storeStatus(Status status)
{
	foreach (StatusContainer *statusContainer, StatusContainers)
		statusContainer->storeStatus(status);
}

QList<StatusContainer *> StatusContainerManager::subStatusContainers()
{
	return StatusContainers;
}

#include "moc_status-container-manager.cpp"
