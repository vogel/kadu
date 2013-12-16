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

#include "misc/memory.h"
#include "plugins/dependency-graph/plugin-dependency-graph.h"
#include "plugins/plugin.h"
#include "plugins/plugin-repository.h"

PluginDependencyGraphBuilder::PluginDependencyGraphBuilder(QObject *parent) :
		QObject{parent}
{
}

PluginDependencyGraphBuilder::~PluginDependencyGraphBuilder()
{
}

std::unique_ptr<PluginDependencyGraph> PluginDependencyGraphBuilder::buildGraph(PluginRepository &pluginRepository) const
{
	auto result = make_unique<PluginDependencyGraph>();

	auto pluginNames = getPluginNames(pluginRepository);
	for (auto pluginName : pluginNames)
		result.get()->addPlugin(pluginName);

	for (auto pluginName : pluginNames)
	{
		auto plugin = pluginRepository.plugin(pluginName);
		if (!plugin)
			continue;

		for (auto const &dependency : plugin->info().dependencies())
			result.get()->addDependency(pluginName, dependency);
	}

	return std::move(result);
}

std::set<QString> PluginDependencyGraphBuilder::getPluginNames(PluginRepository &pluginRepository) const
{
	auto pluginNames = std::set<QString>{};
	for (auto plugin : pluginRepository)
	{
		pluginNames.insert(plugin->info().name());
		for (auto dependency : plugin->info().dependencies())
			pluginNames.insert(dependency);
	}
	return pluginNames;
}
