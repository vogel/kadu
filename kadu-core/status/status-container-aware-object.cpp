/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "status-container-manager.h"

#include "status-container-aware-object.h"

KADU_AWARE_CLASS(StatusContainerAwareObject)

void StatusContainerAwareObject::notifyStatusContainerRegistered(StatusContainer *statusContainer)
{
	foreach (StatusContainerAwareObject *object, Objects)
		object->statusContainerRegistered(statusContainer);
}

void StatusContainerAwareObject::notifyStatusContainerUnregistered(StatusContainer *statusContainer)
{
	foreach (StatusContainerAwareObject *object, Objects)
		object->statusContainerUnregistered(statusContainer);
}

void StatusContainerAwareObject::triggerAllStatusContainerRegistered()
{
	foreach (StatusContainer *statusContainer, StatusContainerManager::instance()->statusContainers())
		statusContainerRegistered(statusContainer);
}

void StatusContainerAwareObject::triggerAllStatusContainerUnregistered()
{
	foreach (StatusContainer *statusContainer, StatusContainerManager::instance()->statusContainers())
		statusContainerUnregistered(statusContainer);
}
