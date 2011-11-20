/*
 * %kadu copyright begin%
 * Copyright 2008, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

bool HistoryImporterChatData::shouldStore()
{
	ensureLoaded();

	return ModuleData::shouldStore() && Imported;
}

QString HistoryImporterChatData::name() const
{
	return QLatin1String("history-importer");
}
