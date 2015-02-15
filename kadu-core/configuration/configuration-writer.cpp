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

#include "configuration-writer.h"

#include "configuration/configuration-path-provider.h"
#include "configuration/configuration.h"
#include "file-system/atomic-file-write-exception.h"
#include "file-system/atomic-file-writer.h"
#include "debug.h"

#include <QtCore/QDir>

ConfigurationWriter::ConfigurationWriter() :
		m_configuration{nullptr},
		m_configurationPathProvider{nullptr}
{
}

ConfigurationWriter::~ConfigurationWriter()
{
}

void ConfigurationWriter::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void ConfigurationWriter::setConfigurationPathProvider(ConfigurationPathProvider *configurationPathProvider)
{
	m_configurationPathProvider = configurationPathProvider;
}

void ConfigurationWriter::write() const
{
	write(m_configurationPathProvider->configurationFilePath());
}

void ConfigurationWriter::backup() const
{
	write(m_configurationPathProvider->backupFilePath());
}

void ConfigurationWriter::write(const QString &fileName) const
{
	auto atomicFileWriter = AtomicFileWriter{};

	try
	{
		m_configuration->touch();
		atomicFileWriter.write(fileName, m_configuration->content());
	}
	catch (AtomicFileWriteException &)
	{
		kdebugm(KDEBUG_INFO, "error during saving of '%s'\n", qPrintable(fileName));
	}
}

#include "moc_configuration-writer.cpp"
