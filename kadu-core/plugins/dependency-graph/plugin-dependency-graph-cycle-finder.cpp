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

#include "plugin-dependency-graph-cycle-finder.h"

#include "misc/graph/graph-algotithm.h"

PluginDependencyGraphCycleFinder::PluginDependencyGraphCycleFinder(QObject *parent) :
		QObject{parent}
{
}

PluginDependencyGraphCycleFinder::~PluginDependencyGraphCycleFinder()
{
}

std::set<PluginDependencyGraphNode *> PluginDependencyGraphCycleFinder::findNodesInCycle(PluginDependencyGraph *pluginDependencyGraph)
{
	if (!pluginDependencyGraph)
		return {};
	else
		return graph_find_cycles<PluginDependencyTag>(*pluginDependencyGraph);
}
