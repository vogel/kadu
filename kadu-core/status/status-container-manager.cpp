/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "configuration/configuration-file.h"
#include "configuration/main-configuration-holder.h"
#include "core/core.h"
#include "identities/identity-manager.h"
#include "icons/kadu-icon.h"
#include "protocols/protocol.h"
#include "status/all-accounts-status-container.h"
#include "status/status-container-aware-object.h"
#include "status/status-type-manager.h"
#include "status/status-type.h"
#include "icons/icons-manager.h"

#include "status-container-manager.h"

StatusContainerManager * StatusContainerManager::Instance = 0;

StatusContainerManager * StatusContainerManager::instance()
{
	if (!Instance)
		Instance = new StatusContainerManager();
	return Instance;
}

StatusContainerManager::StatusContainerManager() :
		StatusContainer(0), AllowSetDefaultStatus(false), DefaultStatusContainer(0)
{
	AllAccountsContainer = new AllAccountsStatusContainer(this);

	configurationUpdated();

	if (MainConfigurationHolder::instance()->isSetStatusPerIdentity())
		triggerAllIdentitiesAdded();
	else if (MainConfigurationHolder::instance()->isSetStatusPerAccount())
		triggerAllAccountsRegistered();
	else
		registerStatusContainer(AllAccountsContainer);

	connect(MainConfigurationHolder::instance(), SIGNAL(setStatusModeChanged()), this, SLOT(setStatusModeChanged()));
	connect(AccountManager::instance(), SIGNAL(accountUpdated(Account)), this, SLOT(updateIdentities()));
}

StatusContainerManager::~StatusContainerManager()
{
	disconnect(AccountManager::instance(), SIGNAL(accountUpdated(Account)), this, SLOT(updateIdentities()));
	disconnect(MainConfigurationHolder::instance(), SIGNAL(setStatusModeChanged()), this, SLOT(setStatusModeChanged()));

	if (MainConfigurationHolder::instance()->isSetStatusPerIdentity())
		triggerAllIdentitiesRemoved();
	else if (MainConfigurationHolder::instance()->isSetStatusPerAccount())
		triggerAllAccountsUnregistered();
	else
		unregisterStatusContainer(AllAccountsContainer);
}

void StatusContainerManager::updateIdentities()
{
	if (!MainConfigurationHolder::instance()->isSetStatusPerIdentity())
		return;

	foreach (const Identity &identity, IdentityManager::instance()->items())
		if (StatusContainers.contains(identity) && !identity.hasAnyAccountWithDetails())
			unregisterStatusContainer(identity);
		else if (!StatusContainers.contains(identity) && identity.hasAnyAccountWithDetails())
			registerStatusContainer(identity);
}

void StatusContainerManager::accountRegistered(Account account)
{
	if (MainConfigurationHolder::instance()->isSetStatusPerAccount() && !StatusContainers.contains(account.statusContainer()))
		registerStatusContainer(account.statusContainer());

	if (MainConfigurationHolder::instance()->isSetStatusPerIdentity() && !StatusContainers.contains(account.accountIdentity()))
		updateIdentities();
}

void StatusContainerManager::accountUnregistered(Account account)
{
	if (MainConfigurationHolder::instance()->isSetStatusPerAccount() && StatusContainers.contains(account.statusContainer()))
		unregisterStatusContainer(account.statusContainer());

	if (MainConfigurationHolder::instance()->isSetStatusPerIdentity())
		updateIdentities();
}

void StatusContainerManager::identityAdded(Identity identity)
{
	if (MainConfigurationHolder::instance()->isSetStatusPerIdentity() && !StatusContainers.contains(identity) && identity.hasAnyAccountWithDetails())
		registerStatusContainer(identity);
}

void StatusContainerManager::identityRemoved(Identity identity)
{
	if (MainConfigurationHolder::instance()->isSetStatusPerIdentity() && StatusContainers.contains(identity))
		unregisterStatusContainer(identity);
}

void StatusContainerManager::configurationUpdated()
{
	StartupStatus = config_file.readEntry("General", "StartupStatus");
	StartupLastDescription = config_file.readBoolEntry("General", "StartupLastDescription");
	StartupDescription = config_file.readEntry("General", "StartupDescription");
	OfflineToInvisible = config_file.readBoolEntry("General", "StartupStatusInvisibleWhenLastWasOffline") && StartupStatus != "Offline";

	if (StartupStatus.isEmpty())
		StartupStatus = "LastStatus";
	else if (StartupStatus == "Busy")
		StartupStatus =  "Away";
}

void StatusContainerManager::cleanStatusContainers()
{
	while (!StatusContainers.isEmpty())
		unregisterStatusContainer(StatusContainers.at(0));
}

void StatusContainerManager::addAllAccounts()
{
	foreach (const Account &account, AccountManager::instance()->items())
		registerStatusContainer(account);
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
		disconnect(DefaultStatusContainer, SIGNAL(statusUpdated()), this, SIGNAL(statusUpdated()));

	if (this != defaultStatusContainer)
		DefaultStatusContainer = defaultStatusContainer;
	else
		DefaultStatusContainer = 0;

	if (DefaultStatusContainer)
		connect(DefaultStatusContainer, SIGNAL(statusUpdated()), this, SIGNAL(statusUpdated()));

	emit statusUpdated();
}

void StatusContainerManager::setStatusModeChanged()
{
	cleanStatusContainers();
	if (MainConfigurationHolder::instance()->isSetStatusPerIdentity())
		addAllIdentities();
	else if (MainConfigurationHolder::instance()->isSetStatusPerAccount())
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

	connect(statusContainer, SIGNAL(statusUpdated()), this, SIGNAL(statusUpdated()));

	if (AllowSetDefaultStatus)
		statusContainer->setDefaultStatus(StartupStatus, OfflineToInvisible, StartupDescription, StartupLastDescription);
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

	disconnect(statusContainer, SIGNAL(statusUpdated()), this, SIGNAL(statusUpdated()));
}

void StatusContainerManager::setAllowSetDefaultStatus(bool allowSetDefaultStatus)
{
	if (AllowSetDefaultStatus == allowSetDefaultStatus)
		return;

	AllowSetDefaultStatus = allowSetDefaultStatus;
	if (AllowSetDefaultStatus)
		foreach (StatusContainer *statusContainer, StatusContainers)
			statusContainer->setDefaultStatus(StartupStatus, OfflineToInvisible, StartupDescription, StartupLastDescription);
}

bool StatusContainerManager::allStatusEqual(StatusType *type)
{
	foreach (StatusContainer *container, StatusContainers)
		if (container->status().type() != type->name())
			return false;
	return true;
}

QString StatusContainerManager::statusContainerName()
{
	return tr("All");
}

void StatusContainerManager::setStatus(Status newStatus, bool flush)
{
	foreach (StatusContainer *container, StatusContainers)
		container->setStatus(newStatus, false);

	if (flush)
		ConfigurationManager::instance()->flush();
}

void StatusContainerManager::setDescription(const QString &description, bool flush)
{
	foreach (StatusContainer *container, StatusContainers)
		container->setDescription(description, false);

	if (flush)
		ConfigurationManager::instance()->flush();
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

QString StatusContainerManager::statusDisplayName()
{
	return DefaultStatusContainer
			? DefaultStatusContainer->statusDisplayName()
			: tr("Offline");
}

KaduIcon StatusContainerManager::statusIcon()
{
	return statusIcon(status());
}

KaduIcon StatusContainerManager::statusIcon(const Status &status)
{
	if (!DefaultStatusContainer)
		return StatusTypeManager::instance()->statusIcon("common", "Offline", false, false);

	return StatusTypeManager::instance()->statusIcon("common", status.type(), status.hasDescription(), false);
}

KaduIcon StatusContainerManager::statusIcon(const QString &statusType)
{
	if (!DefaultStatusContainer)
		return StatusTypeManager::instance()->statusIcon("common", "Offline", false, false);

	return StatusTypeManager::instance()->statusIcon("common", statusType, false, false);
}

QList<StatusType *> StatusContainerManager::supportedStatusTypes()
{
	return DefaultStatusContainer
			? DefaultStatusContainer->supportedStatusTypes()
			: QList<StatusType *>();
// 			: StatusTypeManager::instance()->statusTypes();
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

void StatusContainerManager::storeStatus(Status status)
{
	foreach (StatusContainer *statusContainer, StatusContainers)
		statusContainer->storeStatus(status);
}
