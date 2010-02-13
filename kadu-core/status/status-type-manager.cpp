/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@go2.pl)
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

#include <QtGui/QPixmap>

#include "status/status-group-manager.h"
#include "status/status-type.h"
#include "icons-manager.h"

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
	registerStatusType("FreeForChat", "Free for chat", "free_for_chat", online, 0);
	registerStatusType("Online", "Online", "online", online, 20);
	registerStatusType("Away", "Away", "away", busy, 0);
	registerStatusType("NotAvailable", "Not available", "not_available", busy, 20);
	registerStatusType("DoNotDisturb", "Do not disturb", "do_not_disturb", busy, 40);
	registerStatusType("Invisible", "Invisible", "invisible", invisible, 0);
	registerStatusType("Offline", "Offline", "offline", offline, 0);
}

void StatusTypeManager::registerStatusType(const QString &name, const QString &displayName,
		const QString &iconName, StatusGroup *statusGroup, int sortIndex)
{
	foreach (StatusType *st, StatusTypes)
	{
		if (name != st->name())
			continue;

		StatusTypesCounter[st]++;
		return;
	}

	StatusType *newType = new StatusType(name, displayName, iconName, statusGroup, sortIndex);
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

QPixmap StatusTypeManager::statusPixmap(const QString &protocol, const QString &size, const QString &type,
		bool description, bool mobile)
{
	StatusType *statusType = this->statusType(type);
	if (!statusType)
		return QPixmap();

	QString pixmapName = QString("protocols/%1/%2/%3%4%5.png")
			.arg(protocol)
			.arg(size)
			.arg(statusType->iconName())
			.arg(description ? "_d" : "")
			.arg(mobile ? "_m" : "");
	return IconsManager::instance()->pixmapByPath(pixmapName);
}
