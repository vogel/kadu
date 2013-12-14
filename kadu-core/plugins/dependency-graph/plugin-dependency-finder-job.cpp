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

#include "plugin-dependency-finder-job.h"

#include "plugins/dependency-graph//plugin-dependency-cycle-exception.h"
#include "plugins/dependency-graph/plugin-dependency-graph.h"

#include <algorithm>
#include <vector>
#include <QtCore/QString>

PluginDependencyFinderJob::PluginDependencyFinderJob(PluginDependencyGraph *pluginDependencyGraph, const QString &pluginName)
{
	visitNode(pluginDependencyGraph->node(pluginName));
}

std::vector<PluginDependencyGraphNode *> PluginDependencyFinderJob::result() const
{
	return m_result;
}

void PluginDependencyFinderJob::visitNode(PluginDependencyGraphNode *node)
{
	m_visitStates.insert({node, VisitState::BeingVisited});
	for (auto dependency : node->successors<PluginDependencyTag>())
		findDependencies(dependency);
	m_visitStates.at(node) = VisitState::Visited;
}

void PluginDependencyFinderJob::findDependencies(PluginDependencyGraphNode *node)
{
	auto visitState = m_visitStates.find(node);
	if (visitState == m_visitStates.end())
	{
		visitNode(node);
		m_result.push_back(node);
	}
	else if ((*visitState).second == VisitState::BeingVisited)
		throw PluginDependencyCycleException();
}
