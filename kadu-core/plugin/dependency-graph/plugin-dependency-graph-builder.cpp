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
#include "misc/graph/graph-algorithm.h"
#include "plugin/dependency-graph/plugin-dependency-graph.h"
#include "plugin/metadata/plugin-metadata.h"

PluginDependencyGraphBuilder::PluginDependencyGraphBuilder(QObject *parent) :
		QObject{parent}
{
}

PluginDependencyGraphBuilder::~PluginDependencyGraphBuilder()
{
}

PluginDependencyGraph PluginDependencyGraphBuilder::buildValidGraph(const std::map<QString, PluginMetadata> &plugins) const
{
	auto fullGraph = buildGraph(plugins);
	auto pluginsInCycles = graph_find_cycles<PluginDependencyGraph::PluginDependencyTag>(fullGraph.m_graph);

	std::map<QString, PluginMetadata> pluginsWithoutCycles;
	std::copy_if(std::begin(plugins), std::end(plugins), std::inserter(pluginsWithoutCycles, pluginsWithoutCycles.begin()),
		[&pluginsInCycles](const std::map<QString, PluginMetadata>::value_type &v){ return !contains(pluginsInCycles, v.first); });

	auto noCyclesGraph = buildGraph(pluginsWithoutCycles);
	auto pluginsToRemove = invalidPlugins(fullGraph, plugins);

	std::map<QString, PluginMetadata> validPlugins;
	std::copy_if(std::begin(pluginsWithoutCycles), std::end(pluginsWithoutCycles), std::inserter(validPlugins, validPlugins.begin()),
		[&pluginsToRemove](const std::map<QString, PluginMetadata>::value_type &v){ return !contains(pluginsToRemove, v.first); });

	return buildGraph(validPlugins);
}

PluginDependencyGraph PluginDependencyGraphBuilder::buildGraph(const std::map<QString, PluginMetadata> &plugins) const
{
	auto result = PluginDependencyGraph{};

	for (auto const &plugin : plugins)
	{
		result.addPlugin(plugin.first);
		for (auto const &dependency : plugin.second.dependencies())
			result.addDependency(plugin.first, dependency);
	}

	return std::move(result);
}

std::set<QString> PluginDependencyGraphBuilder::invalidPlugins(const PluginDependencyGraph &graph, const std::map<QString, PluginMetadata> &plugins) const
{
	auto pluginInGraph = graph.plugins();
	auto pluginsWithMetadata = std::set<QString>{};
	std::transform(std::begin(plugins), std::end(plugins), std::inserter(pluginsWithMetadata, pluginsWithMetadata.begin()),
		[](const std::map<QString, PluginMetadata>::value_type &v){ return v.first; }
	);

	auto result = std::set<QString>{};
	std::set_difference(std::begin(pluginInGraph), std::end(pluginInGraph), std::begin(pluginsWithMetadata), std::end(pluginsWithMetadata),
		std::inserter(result, result.begin()));
	return result;
}
