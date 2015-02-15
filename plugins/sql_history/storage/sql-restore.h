/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
 *
 * Restore process is performed synchronously. Do not call performRestore() in GUI thread.
 */
class SqlRestore : public QObject
{
	Q_OBJECT

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Restore process error.
	 */
	enum RestoreError
	{
		/**
		 * Restore process was performed without errors
		 */
		ErrorNoError = 0,
		/**
		 * No sqlite3 executable was found.
		 */
		ErrorSqlite3NotExecutable = 1,
		/**
		 * Number of parameters passed to restore script is invalid.
		 */
		ErrorInvalidParameters = 2,
		/**
		 * Corrupted database cannot be read. It is signal of some deeper error.
		 */
		ErrorUnreadableCorruptedDatabase = 3,
		/**
		 * Directory with corrupted database is invalid. It is signal of some deeper error.
		 */
		ErrorInvalidDirectory = 4,
		/**
		 * Directory with corrupted database is invalid. It is signal of some deeper error.
		 * Possibly disk is full.
		 */
		ErrorUnableToCreateBackup = 5,
		/**
		 * Unkown error during recovery.
		 */
		ErrorRecovering = 6,
		/**
		 * Restore script was not found.
		 */
		ErrorNoRestoreScriptExecutable = 100
	};

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

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Translate error enum to human-readable string.
	 * @param error error number
	 * @return human-readable string describing given error
	 */
	static QString errorMessage(RestoreError error);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Try to restore database file.
	 * @param databaseFilePath path to database file
	 * @return error value of recovery process
	 *
	 * This method executes recovery script with given database file path as parameter. This methods
	 * returns after script is finished, so executing this method is main thread is not a good idea.
	 */
	RestoreError performRestore(const QString &databaseFilePath);

};

/**
 * @}
 */

#endif // SQL_RESTORE_H
