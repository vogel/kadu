/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "status/status-group-manager.h"
#include "status/status-type.h"

#include "status-type-manager.h"

StatusTypeManager * StatusTypeManager::Instance = 0;

KADUAPI StatusTypeManager * StatusTypeManager::instance()
{
	if (0 == Instance)
		Instance = new StatusTypeManager();
	return Instance;
}

StatusTypeManager::StatusTypeManager()
{
	StatusGroup *online = StatusGroupManager::instance()->statusGroup("Online");
	StatusGroup *busy = StatusGroupManager::instance()->statusGroup("Away");
	StatusGroup *invisible = StatusGroupManager::instance()->statusGroup("Invisible");
	StatusGroup *offline = StatusGroupManager::instance()->statusGroup("Offline");

	// TODO: 0.6.6 add translation of second strings
	registerStatusType("FreeForChat", "Free for chat", online, 0);
	registerStatusType("Online", "Online", online, 20);
	registerStatusType("Away", "Away", busy, 0);
	registerStatusType("Not available", "Not available", busy, 20);
	registerStatusType("Do not disturb", "Do not disturb", busy, 40);
	registerStatusType("Invisible", "Invisible", invisible, 0);
	registerStatusType("Offline", "Offline", offline, 0);
}

void StatusTypeManager::registerStatusType(const QString &name, const QString &displayName, StatusGroup *statusGroup, int sortIndex)
{
	foreach (StatusType *st, StatusTypes)
	{
		if (name != st->name())
			continue;

		StatusTypesCounter[st]++;
		return;
	}

	StatusType *newType = new StatusType(name, displayName, statusGroup, sortIndex);
	StatusTypes.append(newType);
	StatusTypesCounter[newType] = 1;
}

void StatusTypeManager::unregisterStatusType(const QString &name)
{
	foreach (StatusType *st, StatusTypes)
	{
		if (name != st->name())
			continue;

		int newCount = StatusTypesCounter.contains(st) ? --StatusTypesCounter[st] : 0;
		if (0 != newCount)
			return;

		StatusTypesCounter.remove(st);
		StatusTypes.removeAll(st);
		delete st;
		return;
	}
}

StatusType * StatusTypeManager::statusType(const QString& name)
{
	foreach (StatusType *st, StatusTypes)
		if (name == st->name())
			return st;

	return 0;
}
