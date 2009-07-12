/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "status/status-group.h"

#include "status-group-manager.h"

StatusGroupManager * StatusGroupManager::Instance = 0;

KADUAPI StatusGroupManager * StatusGroupManager::instance()
{
	if (0 == Instance)
		Instance = new StatusGroupManager();
	return Instance;
}

StatusGroupManager::StatusGroupManager()
{
	registerStatusGroup("Online", 0);
	registerStatusGroup("Away", 25);
	registerStatusGroup("Invisible", 50);
	registerStatusGroup("Offline", StatusGroup::StatusGroupSortIndexAfterSetDescription);
}

void StatusGroupManager::registerStatusGroup(const QString &name, int sortIndex)
{
	foreach (StatusGroup *sg, StatusGroups)
	{
		if (name != sg->name())
			continue;

		StatusGroupsCounter[sg]++;
		return;
	}

	StatusGroup *newGroup = new StatusGroup(name, sortIndex);
	StatusGroups.append(newGroup);
	StatusGroupsCounter[newGroup] = 1;
}

// TODO 0.6.6 add counters how many statuses uses this status-group
// maybe even auto-remove
void StatusGroupManager::unregisterStatusGroup(const QString &name)
{
	foreach (StatusGroup *sg, StatusGroups)
	{
		if (name != sg->name())
			continue;

		int newCount = StatusGroupsCounter.contains(sg) ? --StatusGroupsCounter[sg] : 0;
		if (0 != newCount)
			return;

		StatusGroupsCounter.remove(sg);
		StatusGroups.removeAll(sg);
		delete sg;
		return;
	}
}

StatusGroup * StatusGroupManager::statusGroup(const QString& name)
{
	foreach (StatusGroup *sg, StatusGroups)
		if (name == sg->name())
			return sg;

	return 0;
}
