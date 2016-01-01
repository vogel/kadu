/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "sql-history-plugin-object.h"

#include "storage/history-sql-storage.h"

#include "plugins/history/history-plugin-object.h"
#include "plugins/history/history.h"

#include "plugin/plugin-repository.h"

#include <QtSql/QSqlDatabase>

SqlHistoryPluginObject::SqlHistoryPluginObject(QObject *parent) :
		PluginObject{parent}
{
}

SqlHistoryPluginObject::~SqlHistoryPluginObject()
{
}

void SqlHistoryPluginObject::setHistorySqlStorage(HistorySqlStorage *historySqlStorage)
{
	m_historySqlStorage = historySqlStorage;
}

void SqlHistoryPluginObject::setPluginRepository(PluginRepository *pluginRepository)
{
	m_pluginRepository = pluginRepository;
}

void SqlHistoryPluginObject::init()
{
	m_pluginRepository->pluginObject<HistoryPluginObject>("history")->history()->registerStorage(m_historySqlStorage);
}

void SqlHistoryPluginObject::done()
{
	m_pluginRepository->pluginObject<HistoryPluginObject>("history")->history()->unregisterStorage(m_historySqlStorage);
	QSqlDatabase::removeDatabase("kadu-history");
}

#include "moc_sql-history-plugin-object.cpp"
