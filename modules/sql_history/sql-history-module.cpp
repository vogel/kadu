/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QPushButton>
#include <QtSql/QSqlDatabase>

#include "configuration/configuration-file.h"
#include "debug.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "misc/path-conversion.h"

#include "modules/history/history.h"

#include "sql-history-module.h"

extern "C" KADU_EXPORT int sql_history_init(bool firstLoad)
{
	kdebugf();
	sqlHistoryModule = new SqlHistoryModule(firstLoad);
	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void sql_history_close()
{
	kdebugf();
	delete sqlHistoryModule;
	sqlHistoryModule = 0;
	kdebugf2();
}

SqlHistoryModule::SqlHistoryModule(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	Storage = new HistorySqlStorage();
	History::instance()->registerStorage(Storage);
}

SqlHistoryModule::~SqlHistoryModule()
{
	History::instance()->unregisterStorage(Storage);
	Storage = 0;

	QSqlDatabase::removeDatabase("kadu-history");
}

SqlHistoryModule *sqlHistoryModule = 0;
