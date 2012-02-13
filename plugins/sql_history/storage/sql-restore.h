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

#ifndef SQL_RESTORE_H
#define SQL_RESTORE_H

#include <QtCore/QObject>

class QSqlDatabase;

/**
 * @addtogroup SqlHistory
 * @{
 */

/**
 * @class SqlRestore
 * @author Rafał 'Vogel' Malinowski
 * @short Class with ability to restore corrupted Sqlite3 database.
 *
 * This class can be used to restored corrupted Sqlite3 database to a valid state.
 * At first one should always check if database needs to be restored. Unfortunately,
 * Qt's Sqlite3 does not provide valid interface to checking if database is in sane state.
 * This class use simple heuristics to validate database - if list of available tables is
 * empty or an error occured during reading such list, database is marked as corrupted.
 * Do not use this class on newly created databases that do not have any tables yet.
 */
class SqlRestore : public QObject
{
	Q_OBJECT

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Check if database is corrupted.
	 * @param database database to check
	 * @return true, if database is corrupted
	 *
	 * This method uses simple heuristics to check if given database is corrupted.
	 * Database must be open to perform valid check.
	 *
	 * If database was not properly opened (ie: QSqlDatabase::isOpenError() return true)
	 * then it is marked as corrupted. If list of tables can not be read or is empty,
	 * then database is marked as corrupted.
	 *
	 * Do not use this method on fresh databases without tables - invalid result will be
	 * returned.
	 */
	static bool isCorrupted(const QSqlDatabase &database);

};

/**
 * @}
 */

#endif // SQL_RESTORE_H
