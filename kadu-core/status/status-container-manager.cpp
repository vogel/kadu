/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "status/status-container-aware-object.h"

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
}

StatusContainerManager::~StatusContainerManager()
{
}

void StatusContainerManager::registerStatusContainer(StatusContainer *statusContainer)
{
	emit statusContainerAboutToBeRegistered(statusContainer);
	StatusContainers << statusContainer;
	emit statusContainerRegistered(statusContainer);
	StatusContainerAwareObject::notifyStatusContainerRegistered(statusContainer);
}

void StatusContainerManager::unregisterStatusContainer(StatusContainer *statusContainer)
{
	StatusContainerAwareObject::notifyStatusContainerUnregistered(statusContainer);
	emit statusContainerAboutToBeUnregistered(statusContainer);
	StatusContainers.removeAll(statusContainer);
	emit statusContainerUnregistered(statusContainer);
}
