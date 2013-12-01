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

#pragma once

#include <deque>
#include <map>
#include <set>
#include <utility>

#include "exports.h"

class PluginDependencyGraph;
class PluginDependencyGraphNode;

class KADUAPI PluginDependencyGraphCycleFinderJob
{

public:
	explicit PluginDependencyGraphCycleFinderJob(PluginDependencyGraph *pluginDependencyGraph);

	std::set<PluginDependencyGraphNode *> result() const;

private:
	std::map<PluginDependencyGraphNode *, std::pair<int, int>> m_indexes;
	std::deque<PluginDependencyGraphNode *> m_currnentPath;
	std::set<PluginDependencyGraphNode *> m_result;
	int m_index;

	bool isVisited(PluginDependencyGraphNode *node) const;
	bool isOnCurrentPath(PluginDependencyGraphNode *node) const;
	bool isRootNode(PluginDependencyGraphNode *node) const;
	std::vector<PluginDependencyGraphNode *> getCurrentCycle(PluginDependencyGraphNode *node);
	void strongConnect(PluginDependencyGraphNode *node);

};
