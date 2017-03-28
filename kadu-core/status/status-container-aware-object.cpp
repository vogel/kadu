/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "status-container-manager.h"

#include "status-container-aware-object.h"

QList<StatusContainerAwareObject *> StatusContainerAwareObject::Objects;

StatusContainerAwareObject::StatusContainerAwareObject()
{
    Objects.append(this);
}

StatusContainerAwareObject::~StatusContainerAwareObject()
{
    Objects.removeAll(this);
}

void StatusContainerAwareObject::notifyStatusContainerRegistered(StatusContainer *statusContainer)
{
    for (auto object : Objects)
        object->statusContainerRegistered(statusContainer);
}

void StatusContainerAwareObject::notifyStatusContainerUnregistered(StatusContainer *statusContainer)
{
    for (auto object : Objects)
        object->statusContainerUnregistered(statusContainer);
}

void StatusContainerAwareObject::triggerAllStatusContainerRegistered(StatusContainerManager *statusContainerManager)
{
    for (auto statusContainer : statusContainerManager->statusContainers())
        statusContainerRegistered(statusContainer);
}

void StatusContainerAwareObject::triggerAllStatusContainerUnregistered(StatusContainerManager *statusContainerManager)
{
    for (auto statusContainer : statusContainerManager->statusContainers())
        statusContainerUnregistered(statusContainer);
}
