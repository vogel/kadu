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

ConfigurationStorage::ConfigurationStorage(QString profilePath, QObject *parent) :
		QObject{parent},
		m_profilePath{std::move(profilePath)}
{
}

ConfigurationStorage::~ConfigurationStorage()
{
}

void ConfigurationStorage::writeConfiguration(const QString &fileName, const QString &configuration) const
{
	auto fullPath = m_profilePath + fileName;
	// saving to another file to avoid truncation of output file when segfault occurs :|
	auto fullPathTmp = fullPath + ".tmp";
	QFile file{fullPathTmp};

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
	QFile::remove(fullPath);
	if (!QFile::rename(fullPathTmp, fullPath))
	{
		fprintf(stderr, "cannot rename '%s' to '%s': %s\n", qPrintable(fullPathTmp), qPrintable(fullPath), strerror(errno));
		fflush(stderr);
	}
}

#include "moc_configuration-storage.cpp"
