/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "history-importer-chat-data.h"

HistoryImporterChatData::HistoryImporterChatData(const QString &moduleName, StorableObject *parent, QObject *qobjectParent) :
		ModuleData(moduleName, parent, qobjectParent), Imported(false)
{
}

HistoryImporterChatData::~HistoryImporterChatData()
{
}
void HistoryImporterChatData::load()
{
	if (!isValidStorage())
		return;

	StorableObject::load();

	Imported = loadValue<bool>("Imported", false);
}

void HistoryImporterChatData::store()
{
	if (!isValidStorage())
		return;

	StorableObject::store();

	storeValue("Imported", Imported);
}

QString HistoryImporterChatData::name() const
{
	return QLatin1String("history-importer");
}
