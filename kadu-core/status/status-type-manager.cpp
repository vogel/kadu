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
#include "status/status.h"
#include "status/status-type.h"
#include "status/status-type-data.h"

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
	StatusTypes.insert(StatusTypeFreeForChat,
	                   StatusTypeData(StatusTypeFreeForChat, "FreeForChat", qApp->translate("@default", "Free for chat"), "free_for_chat", StatusTypeGroupOnline));
	StatusTypes.insert(StatusTypeOnline,
	                   StatusTypeData(StatusTypeOnline, "Online", qApp->translate("@default", "Online"), "online", StatusTypeGroupOnline));
	StatusTypes.insert(StatusTypeAway,
	                   StatusTypeData(StatusTypeAway, "Away", qApp->translate("@default", "Away"), "away", StatusTypeGroupAway));
	StatusTypes.insert(StatusTypeNotAvailable,
	                   StatusTypeData(StatusTypeNotAvailable, "NotAvailable", qApp->translate("@default", "Not available"), "not_available", StatusTypeGroupAway));
	StatusTypes.insert(StatusTypeDoNotDisturb,
	                   StatusTypeData(StatusTypeDoNotDisturb, "DoNotDisturb", qApp->translate("@default", "Do not disturb"), "do_not_disturb", StatusTypeGroupAway));
	StatusTypes.insert(StatusTypeInvisible,
	                   StatusTypeData(StatusTypeInvisible, "Invisible", qApp->translate("@default", "Invisible"), "invisible", StatusTypeGroupInvisible));
	StatusTypes.insert(StatusTypeOffline,
	                   StatusTypeData(StatusTypeOffline, "Offline", qApp->translate("@default", "Offline"), "offline", StatusTypeGroupOffline));
}

StatusTypeManager::~StatusTypeManager()
{
}

StatusType StatusTypeManager::fromName(const QString &name)
{
	foreach (StatusType statusType, StatusTypes.keys())
		if (StatusTypes.value(statusType).name() == name)
			return statusType;
	return StatusTypeOffline;
}

const StatusTypeData StatusTypeManager::statusTypeData(StatusType statusType)
{
	if (StatusTypes.contains(statusType))
		return StatusTypes.value(statusType);
	return StatusTypes.value(StatusTypeOffline);
}

KaduIcon StatusTypeManager::statusIcon(const QString &protocol, const Status &status)
{
	const StatusTypeData & statusTypeData = this->statusTypeData(status.type());

	QString iconName = QString("protocols/%1/%2%3")
			.arg(protocol)
			.arg(statusTypeData.iconName())
			.arg(status.description().isEmpty() ? QString() : QLatin1String("_d"));

	return KaduIcon(iconName, "16x16");
}
