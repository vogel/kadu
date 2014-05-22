/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration-storage.h"

#include "debug.h"

#include <QtCore/QDir>
#include <QtCore/QFile>

ConfigurationStorage::ConfigurationStorage(QObject *parent) :
		QObject{parent}
{
}

ConfigurationStorage::~ConfigurationStorage()
{
}

QStringList ConfigurationStorage::possibleConfigurationFiles(const QString &profilePath) const
{
	auto backups_0_12 = QDir{profilePath, "kadu-0.12.conf.xml.backup.*", QDir::Name, QDir::Files};
	auto backups_0_6_6 = QDir{profilePath, "kadu-0.6.6.conf.xml.backup.*", QDir::Name, QDir::Files};
	auto backups_0_6_5 = QDir{profilePath, "kadu.conf.xml.backup.*", QDir::Name, QDir::Files};

	auto files = QStringList{};

	files += "kadu-0.12.conf.xml";
	files += backups_0_12.entryList();
	files += "kadu-0.6.6.conf.xml";
	files += backups_0_6_6.entryList();
	files += "kadu.conf.xml";
	files += backups_0_6_5.entryList();

	return files;
}

QString ConfigurationStorage::readConfiguration(const QString &profilePath) const
{
	for (auto const &fileName : possibleConfigurationFiles(profilePath))
	{
		QFile file{profilePath + fileName};
		if (!file.open(QIODevice::ReadOnly))
		{
			kdebugm(KDEBUG_INFO, "config file (%s) not opened, looking for backup\n", qPrintable(file.fileName()));
			continue;
		}

		auto content = QString::fromUtf8(file.readAll());
		if (content.length() > 0)
			return content;
	}

	return {};
}

void ConfigurationStorage::writeConfiguration(const QString &profilePath, const QString &configuration) const
{
	auto fileName = profilePath + QLatin1String("kadu-0.12.conf.xml");
	// saving to another file to avoid truncation of output file when segfault occurs :|
	auto tmpFileName = fileName + ".tmp";
	QFile file{tmpFileName};

	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		fprintf(stderr, "cannot open '%s': %s\n", qPrintable(file.fileName()), qPrintable(file.errorString()));
		fflush(stderr);
		return;
	}

	kdebugm(KDEBUG_INFO, "file opened '%s'\n", qPrintable(file.fileName()));

	file.write(configuration.toUtf8());
	file.close();

	// remove old file (win32)
	QFile::remove(fileName);
	if (!QFile::rename(tmpFileName, fileName))
	{
		fprintf(stderr, "cannot rename '%s' to '%s': %s\n", qPrintable(tmpFileName), qPrintable(fileName), strerror(errno));
		fflush(stderr);
	}
}

#include "moc_configuration-storage.cpp"
