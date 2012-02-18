/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QVariant>

#include "configuration/xml-configuration-file.h"
#include "storage/storage-point.h"

#include "history-talkable-data.h"

HistoryTalkableData::HistoryTalkableData(const QString &moduleName, StorableObject *parent, QObject *qobjectParent)
		: ModuleData(moduleName, parent, qobjectParent), StoreHistory(true)
{
}

HistoryTalkableData::~HistoryTalkableData()
{
}

void HistoryTalkableData::load()
{
	if (!isValidStorage())
		return;

	StorableObject::load();

	StoreHistory = loadValue<bool>("StoreHistory", true);
}

void HistoryTalkableData::store()
{
	if (!isValidStorage())
		return;

	storeValue("StoreHistory", StoreHistory);
}

bool HistoryTalkableData::shouldStore()
{
	ensureLoaded();

	return ModuleData::shouldStore() && !StoreHistory;
}

QString HistoryTalkableData::name() const
{
	return QLatin1String("history");
}

bool HistoryTalkableData::storeHistory()
{
	ensureLoaded();
	return StoreHistory;
}

void HistoryTalkableData::setStoreHistory(bool storeHistory)
{
	ensureLoaded();
	StoreHistory = storeHistory;
}
