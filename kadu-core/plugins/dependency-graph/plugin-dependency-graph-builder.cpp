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

	auto pluginNames = getPluginNames();
	auto nodes = createNodes(pluginNames);
	connectNodes(nodes);
	return make_unique<PluginDependencyGraph>(convertToVector(nodes));
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

std::map<QString, std::unique_ptr<PluginDependencyGraphNode>> PluginDependencyGraphBuilder::createNodes(std::set<QString> pluginNames) const
{
	std::map<QString, std::unique_ptr<PluginDependencyGraphNode>> nodes;
	for (auto pluginName : pluginNames)
		nodes.insert(std::make_pair(pluginName, make_unique<PluginDependencyGraphNode>(pluginName)));
	return nodes;
}

void PluginDependencyGraphBuilder::connectNodes(std::map<QString, std::unique_ptr<PluginDependencyGraphNode>> &nodes) const
{
	for (auto const &item : nodes)
	{
		auto node = item.second.get();
		auto plugin = m_pluginRepository.data()->plugin(node->payload());
		if (!plugin)
			continue;

		for (auto const &dependency : plugin->info().dependencies())
		{
			Q_ASSERT(nodes.find(dependency) != nodes.end());
			auto dependencyNode = nodes.at(dependency).get();
			if (node != dependencyNode)
			{
				node->addSuccessor<PluginDependencyTag>(dependencyNode);
				dependencyNode->addSuccessor<PluginDependentTag>(node);
			}
		}
	}
}

std::vector<std::unique_ptr<PluginDependencyGraphNode>> PluginDependencyGraphBuilder::convertToVector(std::map<QString, std::unique_ptr<PluginDependencyGraphNode>> &nodes) const
{
	auto nodesVector = std::vector<std::unique_ptr<PluginDependencyGraphNode>>{};
	for (auto &item : nodes)
		nodesVector.push_back(std::move(item.second));
	return std::move(nodesVector);
}
