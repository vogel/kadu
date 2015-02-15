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

#include "plugin-conflict-resolver.h"

#include "plugin/plugin-dependency-handler.h"

#include <QtCore/QMap>

PluginConflictResolver::PluginConflictResolver(QObject *parent) :
		QObject{parent}
{
}

PluginConflictResolver::~PluginConflictResolver()
{
}

void PluginConflictResolver::setPluginDependencyHandler(PluginDependencyHandler *pluginDependencyHandler)
{
	m_pluginDependencyHandler = pluginDependencyHandler;
}

std::set<QString> PluginConflictResolver::conflictingPlugins(const std::set<QString> &activePluginSet, const QString &pluginName)
{
	if (!m_pluginDependencyHandler || contains(activePluginSet, pluginName))
		return std::set<QString>{}; 

	auto withDependencies = m_pluginDependencyHandler->withDependencies(pluginName);
	auto withoutActive = decltype(withDependencies){};
	std::copy_if(std::begin(withDependencies), std::end(withDependencies), std::back_inserter(withoutActive),
			[=,&pluginName](QString const &dependencyName){ return !contains(activePluginSet, dependencyName); });

	auto activeProvides = QMap<QString, QString>{};
	for (auto const &metadata : m_pluginDependencyHandler)
		if (contains(activePluginSet, metadata.name()) && !metadata.provides().isEmpty())
			activeProvides.insert(metadata.provides(), metadata.name());

	auto pluginsToDeactivate = std::set<QString>{};
	for (auto pluginName : withoutActive)
	{
		auto metadata = m_pluginDependencyHandler->pluginMetadata(pluginName);
		if (!metadata.provides().isEmpty() && activeProvides.contains(metadata.provides()))
		{
			auto pluginToDeactivate = activeProvides.value(metadata.provides());
			for (auto dependentPluginToDeactivate : m_pluginDependencyHandler->withDependents(pluginToDeactivate))
				if (contains(activePluginSet, dependentPluginToDeactivate))
					pluginsToDeactivate.insert(dependentPluginToDeactivate);
		}
	}

	return pluginsToDeactivate;
}
