/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
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

#include <QtSql/QSqlDatabase>

#include "plugins/history/history.h"

#include "storage/history-sql-storage.h"

#include "sql-history-plugin.h"

SqlHistoryPlugin::~SqlHistoryPlugin()
{
}

int SqlHistoryPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	Storage = new HistorySqlStorage();

	return 0;
}

void SqlHistoryPlugin::done()
{
	if (Storage)
		History::instance()->unregisterStorage(Storage.data());

	QSqlDatabase::removeDatabase("kadu-history");
}

Q_EXPORT_PLUGIN2(sql_history, SqlHistoryPlugin)
