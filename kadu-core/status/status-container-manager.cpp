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
#include "status/status-container-aware-object.h"
#include "status/status-type-manager.h"
#include "icons-manager.h"

#include "status-container-manager.h"

StatusContainerManager * StatusContainerManager::Instance = 0;

KADUAPI StatusContainerManager * StatusContainerManager::instance()
{
	if (0 == Instance)
		Instance = new StatusContainerManager();
	return Instance;
}

StatusContainerManager::StatusContainerManager()
{
	triggerAllAccountsRegistered();
}

StatusContainerManager::~StatusContainerManager()
{
	triggerAllAccountsUnregistered();
}


void StatusContainerManager::accountRegistered(Account *account)
{
	registerStatusContainer(account);
}

void StatusContainerManager::accountUnregistered(Account *account)
{
	unregisterStatusContainer(account);
}

void StatusContainerManager::registerStatusContainer(StatusContainer *statusContainer)
{
	if (statusContainer == AccountManager::instance()->defaultAccount())
		connect(statusContainer, SIGNAL(statusChanged()), this, SIGNAL(statusChanged()));

	emit statusContainerAboutToBeRegistered(statusContainer);
	StatusContainers << statusContainer;
	emit statusContainerRegistered(statusContainer);
	StatusContainerAwareObject::notifyStatusContainerRegistered(statusContainer);
}

void StatusContainerManager::unregisterStatusContainer(StatusContainer *statusContainer)
{
    	if (statusContainer == AccountManager::instance()->defaultAccount() && AccountManager::instance()->byIndex(1))
		connect(AccountManager::instance()->byIndex(1), SIGNAL(statusChanged()), this, SIGNAL(statusChanged()));

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
    	return AccountManager::instance()->defaultAccount()
		? AccountManager::instance()->defaultAccount()->status()
		: Status("Offline");
}

QString StatusContainerManager::statusName()
{
	return AccountManager::instance()->defaultAccount()
		? AccountManager::instance()->defaultAccount()->statusName()
		: tr("Offline");
}

QPixmap StatusContainerManager::statusPixmap()
{
	return AccountManager::instance()->defaultAccount()
		? AccountManager::instance()->defaultAccount()->statusPixmap()
		: IconsManager::instance()->loadPixmap("Offline");
}

QPixmap StatusContainerManager::statusPixmap(const QString &statusType)
{
    	return AccountManager::instance()->defaultAccount()
		? AccountManager::instance()->defaultAccount()->statusPixmap(statusType)
		: IconsManager::instance()->loadPixmap(statusType);
}

QList<StatusType *> StatusContainerManager::supportedStatusTypes()
{
    	return AccountManager::instance()->defaultAccount()
		? AccountManager::instance()->defaultAccount()->supportedStatusTypes()
		: StatusTypeManager::instance()->statusTypes();
}

int StatusContainerManager::maxDescriptionLength()
{
	return AccountManager::instance()->defaultAccount()
		? AccountManager::instance()->defaultAccount()->maxDescriptionLength()
		: -1;
}
