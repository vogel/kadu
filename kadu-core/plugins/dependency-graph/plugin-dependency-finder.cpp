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

#include "plugin-dependency-finder.h"

#include "misc/graph/graph-algorithm.h"
#include "plugins/dependency-graph/plugin-dependency-cycle-exception.h"

PluginDependencyFinder::PluginDependencyFinder(QObject *parent) :
		QObject{parent}
{
}

PluginDependencyFinder::~PluginDependencyFinder()
{
}

QVector<QString> PluginDependencyFinder::findDependencies(PluginDependencyGraph *pluginDependencyGraph, const QString &pluginName)
{
	return findSuccessors<PluginDependencyTag>(pluginDependencyGraph, pluginName);
}

template<typename SuccessorTypeTag>
QVector<QString> PluginDependencyFinder::findSuccessors(PluginDependencyGraph *pluginDependencyGraph, const QString &pluginName)
{
	if (!pluginDependencyGraph)
		return {};

	try
	{
		auto sortedSuccessors = graph_sort_successors<SuccessorTypeTag>(*pluginDependencyGraph, pluginName);
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
