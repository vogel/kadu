/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QtCore/QStringList>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>

#include "misc/kadu-paths.h"

#include "sql-restore.h"

#define RECOVERY_SCRIPT "plugins/data/sql_history/scripts/history-database-recovery.sh"

bool SqlRestore::isCorrupted(const QSqlDatabase &database)
{
	if (!database.isOpen()) // do not restore closed database
		return false;

	if (database.isOpenError()) // restore every database that was not properly opened
		return true;

	const QStringList &tables = database.tables();
	if (QSqlError::NoError != database.lastError().type())
		return true;

	return tables.isEmpty();
}

QString SqlRestore::errorMessage(SqlRestore::RestoreError error)
{
	switch (error)
	{
		case ErrorNoError:
			return tr("No error.");
		case ErrorSqlite3NotExecutable:
			return tr("sqlite3 executable not found.");
		case ErrorInvalidParameters:
			return tr("Invalid invocation of recovery script.");
		case ErrorUnreadableCorruptedDatabase:
		case ErrorInvalidDirectory:
			return tr("Unable to read corrupted database.");
		case ErrorUnableToCreateBackup:
			return tr("Unable to create backup file. Disc may be full.");
		case ErrorNoRestoreScriptExecutable:
			return tr("Recovery script not found or not executable.");
		default:
			return tr("Unknown error during database recovery.");
	}
}

SqlRestore::RestoreError SqlRestore::performRestore(const QString &databaseFilePath)
{
	QString recoveryScriptPath = KaduPaths::instance()->dataPath() + QLatin1String(RECOVERY_SCRIPT);

	QFileInfo recoveryScriptFileInfo(recoveryScriptPath);
	if (!recoveryScriptFileInfo.exists())
		return ErrorNoRestoreScriptExecutable;

	QProcess restoreProcess;
	restoreProcess.execute("/bin/bash", QStringList() << recoveryScriptPath << databaseFilePath);
	restoreProcess.waitForFinished(-1);

	if (restoreProcess.exitCode() < 0 || restoreProcess.exitCode() > ErrorRecovering)
		return ErrorRecovering;

	return static_cast<RestoreError>(restoreProcess.exitCode());
}

#include "moc_sql-restore.cpp"
