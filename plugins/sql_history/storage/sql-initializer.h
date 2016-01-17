/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtSql/QSqlDatabase>
#include <injeqt/injeqt.h>

class Configuration;
class InjectedFactory;
class PathsProvider;

class QSqlError;

class SqlInitializer : public QObject
{
	Q_OBJECT

	QPointer<Configuration> m_configuration;
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<PathsProvider> m_pathsProvider;

	QSqlDatabase Database;

	bool oldHistoryFileExists();
	bool currentHistoryFileExists();
	bool copyHistoryFile();

	void initDatabaseFile();
	void initDatabase();

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setPathsProvider(PathsProvider *pathsProvider);

public:
	explicit SqlInitializer(QObject *parent = nullptr);
	virtual ~SqlInitializer();

public slots:
	void initialize();

signals:
	void databaseReady(bool ok);

	void progressMessage(const QString &iconName, const QString &message);
	void progressFinished(bool ok, const QString &iconName, const QString &message);

};
