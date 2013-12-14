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

#include "plugin-dependency-graph-cycle-finder-job.h"

#include "plugin-dependency-graph.h"

#include <algorithm>
#include <map>
#include <stack>
#include <vector>
#include <QtCore/QString>

PluginDependencyGraphCycleFinderJob::PluginDependencyGraphCycleFinderJob(PluginDependencyGraph *pluginDependencyGraph) :
		m_result{}
{
	for (auto &node : pluginDependencyGraph->nodes())
		if (!isVisited(node.get()))
			strongConnect(node.get());
}

std::set<PluginDependencyGraphNode *> PluginDependencyGraphCycleFinderJob::result() const
{
	return m_result;
}

void PluginDependencyGraphCycleFinderJob::strongConnect(PluginDependencyGraphNode *node)
{
	m_indexes.insert({node, std::make_pair(m_index, m_index)});
	m_index++;
	m_currnentPath.push_back(node);

	for (auto dependencyNode : node->successors<PluginDependencyTag>())
	{
		if (!isVisited(dependencyNode))
		{
			strongConnect(dependencyNode);
			m_indexes.at(node).second = std::min(m_indexes.at(node).second, m_indexes.at(dependencyNode).second);
		}
		else if (isOnCurrentPath(dependencyNode))
			m_indexes.at(node).second = std::min(m_indexes.at(node).second, m_indexes.at(dependencyNode).first);
	}

	if (isRootNode(node))
	{
		auto cycle = getCurrentCycle(node);
		if (cycle.size() > 1)
			std::copy(cycle.begin(), cycle.end(), std::inserter(m_result, m_result.end()));
	}
}

bool PluginDependencyGraphCycleFinderJob::isVisited(PluginDependencyGraphNode *node) const
{
	return m_indexes.count(node) > 0;
}

bool PluginDependencyGraphCycleFinderJob::isOnCurrentPath(PluginDependencyGraphNode *node) const
{
	return std::find(m_currnentPath.begin(), m_currnentPath.end(), node) != m_currnentPath.end();
}

bool PluginDependencyGraphCycleFinderJob::isRootNode(PluginDependencyGraphNode *node) const
{
	Q_ASSERT(isVisited(node));
	return m_indexes.at(node).first == m_indexes.at(node).second;
}

std::vector<PluginDependencyGraphNode *> PluginDependencyGraphCycleFinderJob::getCurrentCycle(PluginDependencyGraphNode *node)
{
	auto result = std::vector<PluginDependencyGraphNode *>{};
	while (!m_currnentPath.empty())
	{
		auto stackNode = m_currnentPath.back();

		if (m_indexes.at(node).second == m_indexes.at(stackNode).second)
		{
			m_currnentPath.pop_back();
			result.push_back(stackNode);
		}
		else
			return result;
	}

	return result;
}
