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

#include "plugin-metadata-finder.h"

#include "plugin/metadata/plugin-metadata.h"
#include "plugin/metadata/plugin-metadata-reader.h"

#include <QtCore/QDir>
#include <QtCore/QVector>

PluginMetadataFinder::PluginMetadataFinder(QObject *parent) noexcept :
		PluginMetadataProvider{parent}
{
}

PluginMetadataFinder::~PluginMetadataFinder() noexcept
{
}

void PluginMetadataFinder::setDirectory(QString directory)
{
	m_directory = std::move(directory);
}

void PluginMetadataFinder::setPluginMetadataReader(PluginMetadataReader *pluginMetadataReader) noexcept
{
	m_pluginMetadataReader = pluginMetadataReader;
}

std::map<QString, PluginMetadata> PluginMetadataFinder::provide() noexcept
{
	if (m_directory.isEmpty() || !m_pluginMetadataReader)
		return {};

	auto result = std::map<QString, PluginMetadata>{};

	auto dir = QDir{m_directory, "*.desc"};
	dir.setFilter(QDir::Files);

	for (auto const &entry : dir.entryList())
	{
		try
		{
			auto pluginName = entry.left(entry.length() - static_cast<int>(qstrlen(".desc")));
			result.insert({pluginName, m_pluginMetadataReader->readPluginMetadata(pluginName, QString{"%1/%2"}.arg(m_directory).arg(entry))});
		}
		catch (...)
		{
			// I don't think we can recover from this error...
		}
	}

	return result;
}
