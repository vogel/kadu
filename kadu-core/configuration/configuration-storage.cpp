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

#include "file-system/atomic-file-write-exception.h"
#include "file-system/atomic-file-writer.h"
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
	auto atomicFileWriter = AtomicFileWriter{};

	try
	{
		auto fullPath = m_profilePath + fileName;
		atomicFileWriter.write(fullPath, configuration);
	}
	catch (AtomicFileWriteException &)
	{
		kdebugm(KDEBUG_INFO, "error during saving of '%s'\n", qPrintable(fileName));
	}
}

#include "moc_configuration-storage.cpp"
