/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtCore/QtAlgorithms>
#include <QtGui/QApplication>

#include "icons/kadu-icon.h"
#include "status/status-group-manager.h"
#include "status/status-type.h"

#include "status-type-manager.h"

StatusTypeManager * StatusTypeManager::Instance = 0;

StatusTypeManager * StatusTypeManager::instance()
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

	registerStatusType("FreeForChat", qApp->translate("@default", "Free for chat"), "free_for_chat", online, 0);
	registerStatusType("Online", qApp->translate("@default", "Online"), "online", online, 20);
	registerStatusType("Away", qApp->translate("@default", "Away"), "away", busy, 0);
	registerStatusType("NotAvailable", qApp->translate("@default", "Not available"), "not_available", busy, 20);
	registerStatusType("DoNotDisturb", qApp->translate("@default", "Do not disturb"), "do_not_disturb", busy, 40);
	registerStatusType("Invisible", qApp->translate("@default", "Invisible"), "invisible", invisible, 0);
	registerStatusType("Offline", qApp->translate("@default", "Offline"), "offline", offline, 0);
}

StatusTypeManager::~StatusTypeManager()
{
	qDeleteAll(StatusTypes);
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

KaduIcon StatusTypeManager::statusIcon(const QString &protocol, const QString &type, bool description, bool mobile)
{
	StatusType *statusType = this->statusType(type);
	if (!statusType)
		return KaduIcon();

	QString iconName = QString("protocols/%1/%2%3%4")
			.arg(protocol)
			.arg(statusType->iconName())
			.arg(description ? QLatin1String("_d") : QString())
			.arg(mobile ? QLatin1String("_m") : QString());

	return KaduIcon(iconName, "16x16");
}
