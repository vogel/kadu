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

#include "plugin-info-finder.h"

#include "plugin/plugin-info.h"
#include "plugin/plugin-info-reader.h"

#include <QtCore/QDir>
#include <QtCore/QVector>

PluginInfoFinder::PluginInfoFinder(QObject *parent) noexcept :
		QObject{parent}
{
}

PluginInfoFinder::~PluginInfoFinder() noexcept
{
}

void PluginInfoFinder::setPluginInfoReader(PluginInfoReader *pluginInfoReader) noexcept
{
	m_pluginInfoReader = pluginInfoReader;
}

std::map<QString, PluginInfo> PluginInfoFinder::readPluginInfos(const QString &directory) noexcept
{
	if (!m_pluginInfoReader)
		return {};

	auto result = std::map<QString, PluginInfo>{};

	auto dir = QDir{directory, "*.desc"};
	dir.setFilter(QDir::Files);

	for (auto const &entry : dir.entryList())
	{
		try
		{
			auto pluginName = entry.left(entry.length() - static_cast<int>(qstrlen(".desc")));
			result.insert({pluginName, m_pluginInfoReader.data()->readPluginInfo(pluginName, QString{"%1/%2"}.arg(directory).arg(entry))});
		}
		catch (...)
		{
			// I don't think we can recover from this error...
		}
	}

	return result;
}
