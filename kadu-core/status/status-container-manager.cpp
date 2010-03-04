
/*
 * %kadu copyright begin%
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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
#include "status/status-container-aware-object.h"
#include "status/status-type-manager.h"
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
		StatusContainer(0), SelfInList(false)
{
	configurationUpdated();
	triggerAllAccountsRegistered();

	connect(MainConfiguration::instance(), SIGNAL(simpleModeChanged()), this, SLOT(simpleModeChanged()));
}

StatusContainerManager::~StatusContainerManager()
{
	disconnect(MainConfiguration::instance(), SIGNAL(simpleModeChanged()), this, SLOT(simpleModeChanged()));

	triggerAllAccountsUnregistered();
}

void StatusContainerManager::accountRegistered(Account account)
{
	if (!StatusContainers.contains(account.statusContainer()))
		registerStatusContainer(account.statusContainer());
}

void StatusContainerManager::accountUnregistered(Account account)
{
	if (StatusContainers.contains(account.statusContainer()))
		unregisterStatusContainer(account.statusContainer());
}

void StatusContainerManager::configurationUpdated()
{
	StartupStatus = config_file.readEntry("General", "StartupStatus");
	StartupLastDescription = config_file.readBoolEntry("General", "StartupLastDescription");
	StartupDescription = config_file.readEntry("General", "StartupDescription");
	OfflineToInvisible = config_file.readBoolEntry("General", "StartupStatusInvisibleWhenLastWasOffline");

	if (StartupStatus.isEmpty())
		StartupStatus = "LastStatus";
	else if (StartupStatus == "Busy")
		StartupStatus =  "Away";

	PrivateStatus = config_file.readBoolEntry("General", "PrivateStatus");

	DisconnectWithCurrentDescription = config_file.readBoolEntry("General", "DisconnectWithCurrentDescription");
	DisconnectDescription = config_file.readEntry("General", "DisconnectDescription");
}

void StatusContainerManager::cleanStatusContainers()
{
	while (!StatusContainers.isEmpty())
		unregisterStatusContainer(StatusContainers.first());
}

void StatusContainerManager::addAllAccounts()
{
	foreach (Account account, AccountManager::instance()->items())
		registerStatusContainer(account);
}

void StatusContainerManager::addAllIdentities()
{
	foreach (Identity identity, IdentityManager::instance()->items())
		registerStatusContainer(identity);
}

void StatusContainerManager::addSelfToList()
{
	if (SelfInList || !StatusContainers.isEmpty())
		return;

	registerStatusContainer(this);
	SelfInList = true;
}

void StatusContainerManager::removeSelfFromList()
{
	if (!SelfInList)
		return;

	unregisterStatusContainer(this);
	SelfInList = false;
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
	removeSelfFromList();

	if (StatusContainers.isEmpty())
		connect(statusContainer, SIGNAL(statusChanged()), this, SIGNAL(statusChanged()));

	emit statusContainerAboutToBeRegistered(statusContainer);
	StatusContainers.append(statusContainer);
	emit statusContainerRegistered(statusContainer);
	StatusContainerAwareObject::notifyStatusContainerRegistered(statusContainer);

	statusContainer->setDefaultStatus(StartupStatus, OfflineToInvisible, StartupDescription, StartupLastDescription);
}

void StatusContainerManager::unregisterStatusContainer(StatusContainer *statusContainer)
{
	if (statusContainer == AccountManager::instance()->defaultAccount().statusContainer() && !AccountManager::instance()->byIndex(1).isNull())
		connect(AccountManager::instance()->byIndex(1).data(), SIGNAL(statusChanged()), this, SIGNAL(statusChanged()));

	emit statusContainerAboutToBeUnregistered(statusContainer);
	StatusContainers.removeAll(statusContainer);
	emit statusContainerUnregistered(statusContainer);
	StatusContainerAwareObject::notifyStatusContainerUnregistered(statusContainer);

	addSelfToList();
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

Status StatusContainerManager::status()
{
	return AccountManager::instance()->defaultAccount().statusContainer()
			? AccountManager::instance()->defaultAccount().statusContainer()->status()
			: Status::null;
}

QString StatusContainerManager::statusName()
{
	return AccountManager::instance()->defaultAccount().statusContainer()
			? AccountManager::instance()->defaultAccount().statusContainer()->statusName()
			: tr("Offline");
}

QPixmap StatusContainerManager::statusPixmap()
{
	return AccountManager::instance()->defaultAccount().statusContainer()
			? AccountManager::instance()->defaultAccount().statusContainer()->statusPixmap()
			: IconsManager::instance()->pixmapByPath("protocols/gadu-gadu/16x16/offline.png");
}

QPixmap StatusContainerManager::statusPixmap(Status status)
{
	return statusPixmap(status.type());
}

QPixmap StatusContainerManager::statusPixmap(const QString &statusType)
{
	return AccountManager::instance()->defaultAccount().statusContainer()
			? AccountManager::instance()->defaultAccount().statusContainer()->statusPixmap(statusType)
			: QPixmap();
// 			: IconsManager::instance()->loadPixmap(statusType);
}

QList<StatusType *> StatusContainerManager::supportedStatusTypes()
{
	return AccountManager::instance()->defaultAccount().statusContainer()
			? AccountManager::instance()->defaultAccount().statusContainer()->supportedStatusTypes()
			: QList<StatusType *>();
// 			: StatusTypeManager::instance()->statusTypes();
}

int StatusContainerManager::maxDescriptionLength()
{
	return AccountManager::instance()->defaultAccount().statusContainer()
			? AccountManager::instance()->defaultAccount().statusContainer()->maxDescriptionLength()
			: -1;
}

QString StatusContainerManager::statusNamePrefix()
{
	return QString(tr("All")) + " ";
}

void StatusContainerManager::storeStatus()
{
	foreach (StatusContainer *statusContainer, StatusContainers)
		statusContainer->storeStatus();
}

void StatusContainerManager::disconnectStatus(bool disconnectWithCurrentDescription, const QString& disconnectDescription)
{
	// TODO: 0.6.6
	Q_UNUSED(disconnectWithCurrentDescription)
	Q_UNUSED(disconnectDescription)

	foreach (StatusContainer *statusContainer, StatusContainers)
		statusContainer->disconnectStatus(DisconnectWithCurrentDescription, DisconnectDescription);
}

void StatusContainerManager::setPrivateStatus(bool isPrivate)
{
	foreach (StatusContainer *container, StatusContainers)
		container->setPrivateStatus(isPrivate);
}
