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

#include "configuration-factory.h"

#include "configuration/configuration-api.h"
#include "configuration/configuration-path-provider.h"
#include "configuration/configuration-read-error-exception.h"
#include "configuration/configuration-unusable-exception.h"
#include "configuration/configuration.h"

#include <QtCore/QDir>
#include <QtCore/QFile>

ConfigurationFactory::ConfigurationFactory(QObject *parent) :
		QObject{parent},
		m_configurationPathProvider{nullptr}
{
}

ConfigurationFactory::~ConfigurationFactory()
{
}

void ConfigurationFactory::setConfigurationPathProvider(ConfigurationPathProvider *configurationPathProvider)
{
	m_configurationPathProvider = configurationPathProvider;
}

Configuration * ConfigurationFactory::createConfiguration() const
{
	auto result = readConfiguration();
	if (result)
		return result.release();

	return createEmptyConfiguration().release();
}

not_owned_qptr<Configuration> ConfigurationFactory::readConfiguration() const
{
	auto dir = m_configurationPathProvider->configurationDirectoryPath();
	for (auto const &fileName : m_configurationPathProvider->possibleConfigurationFilePaths())
	{
		QFile file{dir + "/" + fileName};
		if (!file.open(QIODevice::ReadOnly))
			continue;

		auto content = QString::fromUtf8(file.readAll());
		if (content.length() == 0)
			continue;

		try
		{
			auto configurationApi = make_unique<ConfigurationApi>(content);
			return make_not_owned<Configuration>(std::move(configurationApi));
		}
		catch (ConfigurationReadErrorException &)
		{
			continue; // try next file
		}
	}

	return {};
}

not_owned_qptr<Configuration> ConfigurationFactory::createEmptyConfiguration() const
{
	if (!isConfigurationPathUsable())
		throw ConfigurationUnusableException();

	auto configurationApi = make_unique<ConfigurationApi>();
	return make_not_owned<Configuration>(std::move(configurationApi));
}

bool ConfigurationFactory::isConfigurationPathUsable() const
{
	auto directory = m_configurationPathProvider->configurationDirectoryPath();
	if (directory.isEmpty())
		return false;

	if (!QDir(directory).isReadable())
		return false;

	if (!QFile(m_configurationPathProvider->configurationFilePath()).open(QIODevice::ReadWrite))
		return false;

	return true;
}

#include "moc_configuration-factory.cpp"
