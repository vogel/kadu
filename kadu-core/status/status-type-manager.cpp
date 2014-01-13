/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "status/status-type-data.h"
#include "status/status-type.h"
#include "status/status.h"

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
	                   StatusTypeData(StatusTypeFreeForChat, "FreeForChat", QCoreApplication::translate("@default", "Free for chat"), "free_for_chat", StatusTypeGroupOnline));
	StatusTypes.insert(StatusTypeOnline,
	                   StatusTypeData(StatusTypeOnline, "Online", QCoreApplication::translate("@default", "Online"), "online", StatusTypeGroupOnline));
	StatusTypes.insert(StatusTypeAway,
	                   StatusTypeData(StatusTypeAway, "Away", QCoreApplication::translate("@default", "Away"), "away", StatusTypeGroupAway));
	StatusTypes.insert(StatusTypeNotAvailable,
	                   StatusTypeData(StatusTypeNotAvailable, "NotAvailable", QCoreApplication::translate("@default", "Not available"), "not_available", StatusTypeGroupAway));
	StatusTypes.insert(StatusTypeDoNotDisturb,
	                   StatusTypeData(StatusTypeDoNotDisturb, "DoNotDisturb", QCoreApplication::translate("@default", "Do not disturb"), "do_not_disturb", StatusTypeGroupAway));
	StatusTypes.insert(StatusTypeInvisible,
	                   StatusTypeData(StatusTypeInvisible, "Invisible", QCoreApplication::translate("@default", "Invisible"), "invisible", StatusTypeGroupInvisible));
	StatusTypes.insert(StatusTypeOffline,
	                   StatusTypeData(StatusTypeOffline, "Offline", QCoreApplication::translate("@default", "Offline"), "offline", StatusTypeGroupOffline));
}

StatusTypeManager::~StatusTypeManager()
{
}

StatusType StatusTypeManager::fromName(const QString &name)
{
	for (QMap<StatusType, StatusTypeData>::const_iterator it = StatusTypes.constBegin(), end = StatusTypes.constEnd(); it != end; ++it)
		if (it.value().name() == name)
			return it.key();
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
