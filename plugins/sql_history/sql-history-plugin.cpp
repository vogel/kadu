/*
 * %kadu copyright begin%
 * Copyright 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtSql/QSqlDatabase>

#include "core/core.h"

#include "plugins/history/history.h"

#include "storage/history-sql-storage.h"

#include "sql-history-plugin.h"

SqlHistoryPlugin::~SqlHistoryPlugin()
{
}

bool SqlHistoryPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	Storage = new HistorySqlStorage();
	Storage->setFormattedStringFactory(Core::instance()->formattedStringFactory());

	return true;
}

void SqlHistoryPlugin::done()
{
	if (Storage && History::instance())
		History::instance()->unregisterStorage(Storage.data());

	QSqlDatabase::removeDatabase("kadu-history");
}

Q_EXPORT_PLUGIN2(sql_history, SqlHistoryPlugin)

#include "moc_sql-history-plugin.cpp"
