/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration-path-provider.h"

#include "misc/paths-provider.h"

#include <QtCore/QDateTime>
#include <QtCore/QDir>

ConfigurationPathProvider::ConfigurationPathProvider(QObject *parent) :
		QObject{parent},
		m_pathsProvider{nullptr}
{
}

ConfigurationPathProvider::~ConfigurationPathProvider()
{
}

void ConfigurationPathProvider::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

QString ConfigurationPathProvider::configurationDirectoryPath() const
{
	return m_pathsProvider->profilePath();
}

QStringList ConfigurationPathProvider::possibleConfigurationFilePaths() const
{
	auto profilePath = m_pathsProvider->profilePath();
	auto backups_0_12 = QDir{profilePath, "kadu-0.12.conf.xml.backup.*", QDir::Name, QDir::Files};
	auto backups_0_6_6 = QDir{profilePath, "kadu-0.6.6.conf.xml.backup.*", QDir::Name, QDir::Files};

	auto files = QStringList{};

	files += "kadu-0.12.conf.xml";
	files += backups_0_12.entryList();
	files += "kadu-0.6.6.conf.xml";
	files += backups_0_6_6.entryList();

	return files;
}

QString ConfigurationPathProvider::configurationFilePath() const
{
	return m_pathsProvider->profilePath() + QLatin1String("/kadu-0.12.conf.xml");
}

QString ConfigurationPathProvider::backupFilePath() const
{
	return m_pathsProvider->profilePath() + QString("kadu-0.12.conf.xml.backup.%1").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd.hh.mm.ss"));
}

#include "moc_configuration-path-provider.cpp"
