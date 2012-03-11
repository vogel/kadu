/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 * Copyright 2011 Wojciech Treter (juzefwt@gmail.com)
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

#ifndef SQL_IMPORT_H
#define SQL_IMPORT_H

#include <QtCore/QObject>

class QSqlDatabase;

/**
 * @addtogroup SqlHistory
 * @{
 */

/**
 * @class SqlImport
 * @author Rafał 'Vogel' Malinowski
 * @short Class with ability to import Sqlite3 from one of old schemes to newest one.
 *
 * This class is used to initialize database schema (create table or indexes) or to import data
 * from old database versions.
 */
class SqlImport : public QObject
{
	Q_OBJECT

	void initTables(QSqlDatabase &database);
	void initKaduSchemaTable(QSqlDatabase &database);
	void initKaduMessagesTable(QSqlDatabase &database);
	void initKaduStatusesTable(QSqlDatabase &database);
	void initKaduSmsTable(QSqlDatabase &database);
	void initIndexes(QSqlDatabase &database);

	void initV4Tables(QSqlDatabase &database);
	void initV4Indexes(QSqlDatabase &database);
	void importAccountsToV4(QSqlDatabase &database);
	void importContactsToV4(QSqlDatabase &database);
	void importContactsToV4StatusesTable(QSqlDatabase &database);
	void importChatsToV4(QSqlDatabase &database);
	void dropBeforeV4Fields(QSqlDatabase &database);
	void dropBeforeV4Indexes(QSqlDatabase &database);

	void importVersion1Schema(QSqlDatabase &database);
	void importVersion2Schema(QSqlDatabase &database);
	void removeDuplicatesFromVersion2Schema(QSqlDatabase &database, const QString &idTableName, const QString &valueFieldName, const QString &idFieldName);

	void importVersion3Schema(QSqlDatabase &database);

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Read schema version from database.
	 * @param database database to read schema version from
	 * @return schema version of given database
	 */
	static quint16 databaseSchemaVersion(QSqlDatabase &database);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Check if database needs import to new version of schema.
	 * @param database database to check
	 *
	 * Any database that does not have schema_version table with one value equal to last schema
	 * version needs update.
	 */
	static bool importNeeded(QSqlDatabase &database);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Perform import from old schema version.
	 * @param database database to import
	 *
	 * Perform import from old schema version to new one. If database is empty - performs initialization
	 * of tables and indexes.
	 *
	 * This method is synchronous. Do not call from GUI thread.
	 */
	void performImport(QSqlDatabase &database);

};

/**
 * @}
 */

#endif // SQL_IMPORT_H
