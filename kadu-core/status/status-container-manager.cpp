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
#include "configuration/main-configuration.h"
#include "core/core.h"
#include "identities/identity-manager.h"
#include "protocols/protocol.h"
#include "status/status-container-aware-object.h"
#include "status/status-type-manager.h"
#include "status/status-type.h"
#include "icons-manager.h"

#include "status-container-manager.h"

StatusContainerManager * StatusContainerManager::Instance = 0;

KADUAPI StatusContainerManager * StatusContainerManager::instance()
{
	if (!Instance)
		Instance = new StatusContainerManager();
	return Instance;
}

StatusContainerManager::StatusContainerManager() :
		StatusContainer(0), AllowSetDefaultStatus(false), DefaultStatusContainer(0)
{
	configurationUpdated();

	if (MainConfiguration::instance()->simpleMode())
		triggerAllIdentitiesAdded();
	else
		triggerAllAccountsRegistered();

	connect(MainConfiguration::instance(), SIGNAL(simpleModeChanged()), this, SLOT(simpleModeChanged()));
	connect(AccountManager::instance(), SIGNAL(accountUpdated(Account)), this, SLOT(updateIdentities()));
}

StatusContainerManager::~StatusContainerManager()
{
	disconnect(AccountManager::instance(), SIGNAL(accountUpdated(Account)), this, SLOT(updateIdentities()));
	disconnect(MainConfiguration::instance(), SIGNAL(simpleModeChanged()), this, SLOT(simpleModeChanged()));

	if (MainConfiguration::instance()->simpleMode())
		triggerAllIdentitiesRemoved();
	else
		triggerAllAccountsUnregistered();
}

void StatusContainerManager::updateIdentities()
{
	if (!MainConfiguration::instance()->simpleMode())
		return;

	foreach (const Identity &identity, IdentityManager::instance()->items())
		if (StatusContainers.contains(identity) && !identity.hasAnyAccountWithDetails())
			unregisterStatusContainer(identity);
		else if (!StatusContainers.contains(identity) && identity.hasAnyAccountWithDetails())
			registerStatusContainer(identity);
}

void StatusContainerManager::accountRegistered(Account account)
{
	if (!MainConfiguration::instance()->simpleMode() && !StatusContainers.contains(account.statusContainer()))
		registerStatusContainer(account.statusContainer());

	if (MainConfiguration::instance()->simpleMode() && !StatusContainers.contains(account.accountIdentity()))
		updateIdentities();
}

void StatusContainerManager::accountUnregistered(Account account)
{
	if (!MainConfiguration::instance()->simpleMode() && StatusContainers.contains(account.statusContainer()))
		unregisterStatusContainer(account.statusContainer());

	if (MainConfiguration::instance()->simpleMode())
		updateIdentities();
}

void StatusContainerManager::identityAdded(Identity identity)
{
	if (MainConfiguration::instance()->simpleMode() && !StatusContainers.contains(identity) && identity.hasAnyAccountWithDetails())
		registerStatusContainer(identity);
}

void StatusContainerManager::identityRemoved(Identity identity)
{
	if (MainConfiguration::instance()->simpleMode() && StatusContainers.contains(identity))
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

	DisconnectWithCurrentDescription = config_file.readBoolEntry("General", "DisconnectWithCurrentDescription");
	DisconnectDescription = config_file.readEntry("General", "DisconnectDescription");
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
		disconnect(DefaultStatusContainer, SIGNAL(statusChanged()), this, SIGNAL(statusChanged()));

	if (this != defaultStatusContainer)
		DefaultStatusContainer = defaultStatusContainer;
	else
		DefaultStatusContainer = 0;

	if (DefaultStatusContainer)
		connect(DefaultStatusContainer, SIGNAL(statusChanged()), this, SIGNAL(statusChanged()));

	emit statusChanged();
}

void StatusContainerManager::simpleModeChanged()
{
	cleanStatusContainers();
	if (MainConfiguration::instance()->simpleMode())
		addAllIdentities();
	else
		addAllAccounts();
}

void StatusContainerManager::registerStatusContainer(StatusContainer *statusContainer)
{
	if (StatusContainers.isEmpty())
		setDefaultStatusContainer(statusContainer);

	emit statusContainerAboutToBeRegistered(statusContainer);
	StatusContainers.append(statusContainer);
	emit statusContainerRegistered(statusContainer);
	StatusContainerAwareObject::notifyStatusContainerRegistered(statusContainer);

	connect(statusContainer, SIGNAL(statusChanged()), this, SIGNAL(statusChanged()));

	if (AllowSetDefaultStatus)
		statusContainer->setDefaultStatus(StartupStatus, OfflineToInvisible, StartupDescription, StartupLastDescription);

	emitStatusContainerUpdated();
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

	disconnect(statusContainer, SIGNAL(statusChanged()), this, SIGNAL(statusChanged()));

	emitStatusContainerUpdated();
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

void StatusContainerManager::setStatus(Status newStatus)
{
	foreach (StatusContainer *container, StatusContainers)
		container->setStatus(newStatus);
}

void StatusContainerManager::setDescription(const QString &description)
{
	foreach (StatusContainer *container, StatusContainers)
		container->setDescription(description);
}

Status StatusContainerManager::status()
{
	return DefaultStatusContainer
			? DefaultStatusContainer->status()
			: Status();
}

QString StatusContainerManager::statusDisplayName()
{
	return DefaultStatusContainer
			? DefaultStatusContainer->statusDisplayName()
			: tr("Offline");
}

QIcon StatusContainerManager::statusIcon()
{
	if (!DefaultStatusContainer)
		return IconsManager::instance()->iconByPath("protocols/common/offline");
	Status status = DefaultStatusContainer->status();
	return StatusTypeManager::instance()->statusIcon("common", status.type(), status.hasDescription(), false);
}

QIcon StatusContainerManager::statusIcon(Status status)
{
	if (!DefaultStatusContainer)
		return IconsManager::instance()->iconByPath("protocols/common/offline");
	return StatusTypeManager::instance()->statusIcon("common", status.type(), status.hasDescription(), false);
}

QString StatusContainerManager::statusIconPath(const QString &statusType)
{
	if (!DefaultStatusContainer)
		return "protocols/common/offline";
	return StatusTypeManager::instance()->statusIconPath("common", statusType, false, false);
}

QIcon StatusContainerManager::statusIcon(const QString &statusType)
{
	if (!DefaultStatusContainer)
		return IconsManager::instance()->iconByPath("protocols/common/offline");
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
