/*
 * %kadu copyright begin%
 * Copyright 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "configuration/configuration-file.h"
#include "misc/kadu-paths.h"

#include "storage/history-sql-storage.h"
#include "storage/sql-import.h"
#include "storage/sql-restore.h"

#include "sql-initializer.h"

#define HISTORY_FILE_0 "history/history.db"
#define HISTORY_FILE_1 "history1.db"
#define HISTORY_FILE_CURRENT "history2.db"

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

	bool ok = Database.isOpen() && !Database.isOpenError();
	Database.close();

	emit databaseReady(ok);

	deleteLater();
}

bool SqlInitializer::oldHistoryFileExists()
{
	QFileInfo scheme0FileInfo(KaduPaths::instance()->profilePath() + QLatin1String(HISTORY_FILE_0));
	QFileInfo scheme1FileInfo(KaduPaths::instance()->profilePath() + QLatin1String(HISTORY_FILE_1));
	return scheme0FileInfo.exists() || scheme1FileInfo.exists();
}

bool SqlInitializer::currentHistoryFileExists()
{
	QFileInfo schemeCurrentFileInfo(KaduPaths::instance()->profilePath() + QLatin1String(HISTORY_FILE_CURRENT));
	return schemeCurrentFileInfo.exists();
}

bool SqlInitializer::copyHistoryFile()
{
	QFileInfo schemeCurrentFileInfo(KaduPaths::instance()->profilePath() + QLatin1String(HISTORY_FILE_CURRENT));
	if (schemeCurrentFileInfo.exists())
		return true;

	QFileInfo scheme1FileInfo(KaduPaths::instance()->profilePath() + QLatin1String(HISTORY_FILE_1));
	if (scheme1FileInfo.exists())
		return QFile::copy(scheme1FileInfo.absoluteFilePath(), schemeCurrentFileInfo.absoluteFilePath());

	QFileInfo scheme0FileInfo(KaduPaths::instance()->profilePath() + QLatin1String(HISTORY_FILE_0));
	if (scheme0FileInfo.exists())
		return QFile::copy(scheme0FileInfo.absoluteFilePath(), schemeCurrentFileInfo.absoluteFilePath());

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

	bool currentFileExists = currentHistoryFileExists();
	bool anyHistoryFileExists = currentFileExists || oldHistoryFileExists();

	if (!currentFileExists && oldHistoryFileExists())
	{
		emit progressMessage("dialog-information", tr("Copying history file to new location: %1 ...").arg(KaduPaths::instance()->profilePath() + QLatin1String(HISTORY_FILE_CURRENT)));
		if (!copyHistoryFile())
		{
			emit progressFinished(false, "dialog-error", tr("Unable to copy history file to new location. Check if disk is full."));
			return;
		}
	}

	QString historyFilePath = KaduPaths::instance()->profilePath() + QLatin1String(HISTORY_FILE_CURRENT);

	Database = QSqlDatabase::addDatabase("QSQLITE", "kadu-history");
	Database.setDatabaseName(historyFilePath);

	if (!Database.open())
	{
		emit progressFinished(false, "dialog-error", tr("Unable to open database: %1").arg(Database.lastError().text()));
		return;
	}

	if (anyHistoryFileExists && SqlRestore::isCorrupted(Database)) // this is not new database
	{
		Database.close();

		emit progressMessage("dialog-warning", tr("History file is corrupted, performing recovery..."));

		SqlRestore sqlRestore;
		SqlRestore::RestoreError error = sqlRestore.performRestore(historyFilePath);
		if (SqlRestore::ErrorNoError == error)
			emit progressMessage("dialog-information", tr("Recovery completed."));
		else
			emit progressMessage("dialog-error", tr("Recovery failed: %s").arg(SqlRestore::errorMessage(error)));

		if (!Database.open())
		{
			emit progressFinished(false, "dialog-error", tr("Unable to open database: %1").arg(Database.lastError().text()));
			return;
		}
	}

	if (SqlImport::importNeeded(Database))
	{
		if (anyHistoryFileExists)
			emit progressMessage("dialog-warning", tr("History file is outdated, performing import..."));

		SqlImport sqlImport;
		sqlImport.performImport(Database);

		if (anyHistoryFileExists)
			emit progressFinished(true, "dialog-information", tr("Import completed."));
	}
	else
	{
		config_file.writeEntry("History", "Schema", SqlImport::databaseSchemaVersion(Database));
		emit progressFinished(true, "dialog-information", tr("Copying completed."));
	}
}

#include "moc_sql-initializer.cpp"
