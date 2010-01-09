/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "status/status-changer.h"
#include "status/status-container-manager.h"
#include "debug.h"

#include "status-changer-manager.h"

StatusChangerManager * StatusChangerManager::Instance = 0;

StatusChangerManager * StatusChangerManager::instance()
{
	if (0 == Instance)
		Instance = new StatusChangerManager();

	return Instance;
}

StatusChangerManager::StatusChangerManager()
{
}

StatusChangerManager::~StatusChangerManager()
{
}

void StatusChangerManager::registerStatusChanger(StatusChanger *statusChanger)
{
	kdebugf();

	connect(statusChanger, SIGNAL(statusChanged(StatusContainer *)), this, SLOT(statusChanged(StatusContainer *)));

	for (int i = 0; i < StatusChangers.count(); i++)
		if (StatusChangers.at(i)->priority() > statusChanger->priority())
		{
			StatusChangers.insert(i, statusChanger);
			return;
		}

	StatusChangers.insert(StatusChangers.end(), statusChanger);
	statusChanged();

	kdebugf2();
}

void StatusChangerManager::unregisterStatusChanger(StatusChanger *statusChanger)
{
	kdebugf();

	if (StatusChangers.removeAll(statusChanger))
	{
		disconnect(statusChanger, SIGNAL(statusChanged(StatusContainer *)), this, SLOT(statusChanged(StatusContainer *)));
		statusChanged();
	}

	kdebugf2();
}

void StatusChangerManager::setStatus(StatusContainer *statusContainer, Status status)
{
	if (statusContainer)
	{
		Statuses[statusContainer] = status;
		statusChanged(statusContainer);
	}
}

void StatusChangerManager::statusChanged(StatusContainer *container)
{
	if (!container)
	{
		foreach (StatusContainer *statusContainer, StatusContainerManager::instance()->statusContainers())
			if (statusContainer)
				statusChanged(statusContainer);

		return;
	}

	kdebugf();

	Status status = Statuses[container];
	for (int i = 0; i < StatusChangers.count(); i++)
		StatusChangers.at(i)->changeStatus(container, status);
	RealStatuses[container] = status;

	emit statusChanged(container, status);

	kdebugf2();
}

Status StatusChangerManager::status(StatusContainer *statusContainer)
{
	if (RealStatuses.contains(statusContainer))
		return RealStatuses[statusContainer];
	if (Statuses.contains(statusContainer))
		return Statuses[statusContainer];
	return Status("Offline");
}
