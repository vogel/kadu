/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QThread>
#include <QtSql/QSqlError>

#include "misc/path-conversion.h"

#include "storage/history-sql-storage.h"
#include "storage/sql-import.h"
#include "storage/sql-restore.h"

#include "sql-initializer.h"

#define OLD_HISTORY_FILE "history/history.db"
#define HISTORY_FILE "history1.db"

SqlInitializer::SqlInitializer(QObject *parent) :
		QObject(parent)
{
}

SqlInitializer::~SqlInitializer()
{
}

void SqlInitializer::initialize()
{
	initDatabase();

	bool ok = Database.isOpen();
	Database.close();

	emit databaseReady(ok);
	emit initialized();

	deleteLater();
}

bool SqlInitializer::isCopyingNeeded()
{
	QFileInfo scheme1FileInfo(profilePath(HISTORY_FILE));
	if (scheme1FileInfo.exists())
		return false;

	QFileInfo scheme0FileInfo(profilePath(OLD_HISTORY_FILE));
	return scheme0FileInfo.exists();
}

bool SqlInitializer::copyHistoryFile()
{
	QFileInfo scheme1FileInfo(profilePath(HISTORY_FILE));
	if (scheme1FileInfo.exists())
		return true;

	QFileInfo scheme0FileInfo(profilePath(OLD_HISTORY_FILE));
	if (scheme0FileInfo.exists())
		return QFile::copy(scheme0FileInfo.absoluteFilePath(), scheme1FileInfo.absoluteFilePath());

	return false;
}

void SqlInitializer::initDatabase()
{
	if (QSqlDatabase::contains("kadu-history"))
	{
		if (Database.isOpen())
			Database.close();
		QSqlDatabase::removeDatabase("kadu-history");
	}

	bool history1FileExists = !isCopyingNeeded();
	bool importStartedEmitted = false;
	if (!history1FileExists)
	{
		emit importStarted();
		importStartedEmitted = true;
		history1FileExists = copyHistoryFile();
	}

	QString historyFilePath = profilePath(HISTORY_FILE);

	Database = QSqlDatabase::addDatabase("QSQLITE", "kadu-history");
	Database.setDatabaseName(historyFilePath);

	if (!Database.open())
	{
		emit databaseOpenFailed(Database.lastError());
		return;
	}

	if (history1FileExists && SqlRestore::isCorrupted(Database)) // this is not new database
	{
		Database.close();

		SqlRestore sqlRestore;
		printf("restore error: %d\n", sqlRestore.performRestore(historyFilePath));

		if (!Database.open())
		{
			emit databaseOpenFailed(Database.lastError());
			return;
		}
	}

	if (SqlImport::importNeeded(Database))
	{
		SqlImport sqlImport;
		sqlImport.performImport(Database);
	}

	if (importStartedEmitted)
		emit importFinished();
}
