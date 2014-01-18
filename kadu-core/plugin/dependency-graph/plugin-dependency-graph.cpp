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

#include "plugin-dependency-graph.h"

#include "misc/graph/graph-algorithm.h"
#include "plugin/dependency-graph/plugin-dependency-cycle-exception.h"

#include <QtCore/QString>

void PluginDependencyGraph::addPlugin(const QString &pluginName)
{
	m_graph.addNode(pluginName);
}

void PluginDependencyGraph::addDependency(const QString &dependentPluginName, const QString &dependencyPluginName)
{
	if (dependentPluginName == dependencyPluginName)
		return;

	m_graph.addEdge<PluginDependencyTag>(dependentPluginName, dependencyPluginName);
	m_graph.addEdge<PluginDependentTag>(dependencyPluginName, dependentPluginName);
}

int PluginDependencyGraph::size() const
{
	return m_graph.nodes().size();
}

std::set<QString> PluginDependencyGraph::plugins() const
{
	auto result = std::set<QString>{};
	std::transform(std::begin(m_graph.nodes()), std::end(m_graph.nodes()), std::inserter(result, result.begin()),
		[](const PluginGraph::Storage::value_type &value) { return value.first; });
	return result;
}

QSet<QString> PluginDependencyGraph::directDependencies(const QString &pluginName) const
{
	return directSuccessors<PluginDependencyTag>(pluginName);
}

QSet<QString> PluginDependencyGraph::directDependents(const QString &pluginName) const
{
	return directSuccessors<PluginDependentTag>(pluginName);
}

template<typename SuccessorTypeTag>
QSet<QString> PluginDependencyGraph::directSuccessors(const QString &pluginName) const
{
	auto node = m_graph.node(pluginName);
	if (!node)
		return {};

	auto result = QSet<QString>{};
	for (auto successor : node->successors<SuccessorTypeTag>())
		result.insert(successor->payload());
	return result;
}

QSet<QString> PluginDependencyGraph::findPluginsInDependencyCycle() const
{
	auto result = QSet<QString>{};
	for (auto successor : graph_find_cycles<PluginDependencyTag>(m_graph))
		result.insert(successor);
	return result;
}

QVector<QString> PluginDependencyGraph::findDependencies(const QString &pluginName) const
{
	return findSuccessors<PluginDependencyTag>(pluginName);
}

QVector<QString> PluginDependencyGraph::findDependents(const QString &pluginName) const
{
	return findSuccessors<PluginDependentTag>(pluginName);
}

template<typename SuccessorTypeTag>
QVector<QString> PluginDependencyGraph::findSuccessors(const QString &pluginName) const
{
	try
	{
		auto sortedSuccessors = graph_sort_successors<SuccessorTypeTag>(m_graph, pluginName);
		auto result = QVector<QString>{};

		auto extractPayload = [](decltype(sortedSuccessors[0]) &v){ return v->payload(); };
		std::transform(sortedSuccessors.begin(), sortedSuccessors.end(), std::back_inserter(result), extractPayload);
		return result;
	}
	catch (GraphCycleException &e)
	{
		throw PluginDependencyCycleException();
	}
}
