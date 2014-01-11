/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "plugin-dependency-graph-builder.h"

#include "misc/algorithm.h"
#include "misc/memory.h"
#include "plugin/dependency-graph/plugin-dependency-graph.h"
#include "plugin/metadata/plugin-metadata.h"

PluginDependencyGraphBuilder::PluginDependencyGraphBuilder(QObject *parent) :
		QObject{parent}
{
}

PluginDependencyGraphBuilder::~PluginDependencyGraphBuilder()
{
}

std::unique_ptr<PluginDependencyGraph> PluginDependencyGraphBuilder::buildGraph(const ::std::map<QString, PluginMetadata> &plugins) const
{
	auto result = make_unique<PluginDependencyGraph>();

	auto pluginNames = getPluginNames(plugins);
	for (auto const &pluginName : pluginNames)
		result.get()->addPlugin(pluginName);

	for (auto const &pluginName : pluginNames)
	{
		if (!contains(plugins, pluginName))
			continue;

		auto const &pluginMetadata = plugins.at(pluginName);
		for (auto const &dependency : pluginMetadata.dependencies())
			result.get()->addDependency(pluginName, dependency);
	}

	return std::move(result);
}

std::set<QString> PluginDependencyGraphBuilder::getPluginNames(const ::std::map<QString, PluginMetadata> &plugins) const
{
	auto pluginNames = std::set<QString>{};
	for (auto const &plugin : plugins)
	{
		auto const &pluginMetadata = plugin.second;
		pluginNames.insert(pluginMetadata.name());
		for (auto dependency : pluginMetadata.dependencies())
			pluginNames.insert(dependency);
	}
	return pluginNames;
}
