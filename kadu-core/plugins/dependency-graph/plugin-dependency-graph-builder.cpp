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

void PluginDependencyGraphBuilder::setPluginRepository(PluginRepository *pluginRepository)
{
	m_pluginRepository = pluginRepository;
}

std::unique_ptr<PluginDependencyGraph> PluginDependencyGraphBuilder::buildGraph() const
{
	if (!m_pluginRepository)
		return {};

	auto result = make_unique<PluginDependencyGraph>();

	auto pluginNames = getPluginNames();
	for (auto pluginName : pluginNames)
		result.get()->addNode(pluginName);

	for (auto pluginName : pluginNames)
	{
		auto plugin = m_pluginRepository.data()->plugin(pluginName);
		if (!plugin)
			continue;

		for (auto const &dependency : plugin->info().dependencies())
			if (dependency != pluginName)
			{
				result.get()->addEdge<PluginDependencyTag>(pluginName, dependency);
				result.get()->addEdge<PluginDependentTag>(dependency, pluginName);
			}
	}

	return std::move(result);
}

std::set< QString > PluginDependencyGraphBuilder::getPluginNames() const
{
	auto pluginNames = std::set<QString>{};
	for (auto plugin : m_pluginRepository.data())
	{
		pluginNames.insert(plugin->info().name());
		for (auto dependency : plugin->info().dependencies())
			pluginNames.insert(dependency);
	}
	return pluginNames;
}
