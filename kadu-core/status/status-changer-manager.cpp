/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "status/status-changer.h"
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
	: enabled(false)
{
}

StatusChangerManager::~StatusChangerManager()
{
}

void StatusChangerManager::registerStatusChanger(StatusChanger *statusChanger)
{
	kdebugf();

	connect(statusChanger, SIGNAL(statusChanged()), this, SLOT(statusChanged()));

	for (int i = 0; i < statusChangers.count(); i++)
		if (statusChangers.at(i)->priority() > statusChanger->priority())
		{
			statusChangers.insert(i, statusChanger);
			return;
		}

	statusChangers.insert(statusChangers.end(), statusChanger);
	statusChanged();

	kdebugf2();
}

void StatusChangerManager::unregisterStatusChanger(StatusChanger *statusChanger)
{
	kdebugf();

	if (statusChangers.removeAll(statusChanger))
	{
		disconnect(statusChanger, SIGNAL(statusChanged()), this, SLOT(statusChanged()));
		statusChanged();
	}

	kdebugf2();
}

void StatusChangerManager::statusChanged()
{
	kdebugf();

	if (!enabled)
		return;

	LastStatus = Status();
	for (int i = 0; i < statusChangers.count(); i++)
		statusChangers.at(i)->changeStatus(LastStatus);

	emit statusChanged(LastStatus);

	kdebugf2();
}

void StatusChangerManager::enable()
{
	if (enabled)
		return;

	enabled = true;
	statusChanged();
}
