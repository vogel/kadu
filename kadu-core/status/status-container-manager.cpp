
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "core/core.h"
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
		StatusContainer(0)
{
	configurationUpdated();
	triggerAllAccountsRegistered();
}

StatusContainerManager::~StatusContainerManager()
{
	triggerAllAccountsUnregistered();
}

void StatusContainerManager::accountRegistered(Account account)
{
	registerStatusContainer(account.statusContainer());
}

void StatusContainerManager::accountUnregistered(Account account)
{
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

void StatusContainerManager::registerStatusContainer(StatusContainer *statusContainer)
{
	if (statusContainer == AccountManager::instance()->defaultAccount().statusContainer())
		connect(statusContainer, SIGNAL(statusChanged()), this, SIGNAL(statusChanged()));

	emit statusContainerAboutToBeRegistered(statusContainer);
	StatusContainers << statusContainer;
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
			: IconsManager::instance()->loadPixmap("Offline");
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

void StatusContainerManager::disconnectAndStoreLastStatus(bool disconnectWithCurrentDescription, const QString& disconnectDescription)
{
	foreach (StatusContainer *statusContainer, StatusContainers)
		statusContainer->disconnectAndStoreLastStatus(DisconnectWithCurrentDescription, DisconnectDescription);
}

void StatusContainerManager::setPrivateStatus(bool isPrivate)
{
	foreach (StatusContainer *container, StatusContainers)
		container->setPrivateStatus(isPrivate);
}
