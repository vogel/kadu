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

#include "plugin-dependency-graph-builder.h"

#include "plugin/dependency-graph/plugin-dependency-graph.h"
#include "plugin/metadata/plugin-metadata.h"

#include <QtCore/QSet>
#include <queue>

PluginDependencyGraphBuilder::PluginDependencyGraphBuilder(QObject *parent) :
		QObject{parent}
{
}

PluginDependencyGraphBuilder::~PluginDependencyGraphBuilder()
{
}

PluginDependencyGraph PluginDependencyGraphBuilder::buildValidGraph(const std::map<QString, PluginMetadata> &plugins) const
{
	auto findCycles = [](PluginDependencyGraph &graph)
	{
		return graph.findPluginsInDependencyCycle();
	};

	auto findInvalidPlugins = [this,&plugins](PluginDependencyGraph &graph)
	{
		auto pluginsToRemove = QSet<QString>{};
		for (auto const &invalidPlugin : invalidPlugins(graph, plugins))
			pluginsToRemove = setWithDependents(pluginsToRemove, graph, invalidPlugin);
		return pluginsToRemove;
	};

	auto findInvalidProvides = [this,&plugins](PluginDependencyGraph &graph)
	{
		auto pluginsToRemove = QSet<QString>{};
		auto data = std::map<QString, std::tuple<int, std::set<QString>>>{};
		for (auto const &plugin : graph.plugins())
			data.insert({plugin, std::make_tuple(graph.directDependencies(plugin).size(), std::set<QString>{})});

		auto readyToCompute = std::queue<QString>{};
		for (auto const &entry : data)
			if (std::get<0>(entry.second) == 0)
				readyToCompute.push(entry.first);

		while (!readyToCompute.empty())
		{
			auto plugin = readyToCompute.front();
			readyToCompute.pop();

			auto &entry = data.at(plugin);
			auto &metadata = plugins.at(plugin);
			auto provides = metadata.provides();
			auto dependents = graph.directDependents(plugin);

			if (!provides.isEmpty())
			{
				if (contains(std::get<1>(entry), provides))
					pluginsToRemove = setWithDependents(pluginsToRemove, graph, plugin);
				for (auto dependent : dependents)
					std::get<1>(data.at(dependent)).insert(provides);
			}

			for (auto dependent : dependents)
			{
				auto &currentProvides = std::get<1>(entry);
				auto &dependentEntry = data.at(dependent);
				auto &dependentProvides = std::get<1>(dependentEntry);
				std::copy(std::begin(currentProvides), std::end(currentProvides), std::inserter(dependentProvides, dependentProvides.begin()));
				if (--std::get<0>(dependentEntry) == 0)
					readyToCompute.push(dependent);
			}
		}

		return pluginsToRemove;
	};

	return applyFilters(plugins, {findCycles, findInvalidPlugins, findInvalidProvides});
}

QSet<QString> PluginDependencyGraphBuilder::setWithDependents(QSet<QString> set, const PluginDependencyGraph &graph, const QString &pluginName) const
{
	set.insert(pluginName);
	for (auto dependent : graph.findDependents(pluginName))
		set.insert(dependent);
	return std::move(set);
}

PluginDependencyGraph PluginDependencyGraphBuilder::applyFilters(const std::map<QString, PluginMetadata> &plugins, std::vector<PluginFilter> filters) const
{
	auto currentPlugins = plugins;
	for (auto filter : filters)
		currentPlugins = applyFilter(currentPlugins, filter);
	return buildGraph(currentPlugins);
}

std::map<QString, PluginMetadata> PluginDependencyGraphBuilder::applyFilter(const std::map<QString, PluginMetadata> &plugins, const PluginFilter &filter) const
{
	auto graph = buildGraph(plugins);
	auto invalid = filter(graph);

	std::map<QString, PluginMetadata> result;
	std::copy_if(std::begin(plugins), std::end(plugins), std::inserter(result, result.begin()),
		[&invalid](const std::map<QString, PluginMetadata>::value_type &v){ return !contains(invalid, v.first); });
	return result;
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

std::set<QString> PluginDependencyGraphBuilder::invalidPlugins(const PluginDependencyGraph &graph,
		const std::map<QString, PluginMetadata> &plugins) const
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
