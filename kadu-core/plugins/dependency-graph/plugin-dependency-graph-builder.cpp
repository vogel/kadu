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
#include "plugins/plugin-info.h"
#include "plugins/plugin-info-repository.h"

PluginDependencyGraphBuilder::PluginDependencyGraphBuilder(QObject *parent) :
		QObject{parent}
{
}

PluginDependencyGraphBuilder::~PluginDependencyGraphBuilder()
{
}

std::unique_ptr<PluginDependencyGraph> PluginDependencyGraphBuilder::buildGraph(PluginInfoRepository &pluginInfoRepository) const
{
	auto result = make_unique<PluginDependencyGraph>();

	auto pluginNames = getPluginNames(pluginInfoRepository);
	for (auto pluginName : pluginNames)
		result.get()->addPlugin(pluginName);

	for (auto pluginName : pluginNames)
	{
		if (!pluginInfoRepository.hasPluginInfo(pluginName))
			continue;

		auto pluginInfo = pluginInfoRepository.pluginInfo(pluginName);
		for (auto const &dependency : pluginInfo.dependencies())
			result.get()->addDependency(pluginName, dependency);
	}

	return std::move(result);
}

std::set<QString> PluginDependencyGraphBuilder::getPluginNames(PluginInfoRepository &pluginInfoRepository) const
{
	auto pluginNames = std::set<QString>{};
	for (auto pluginInfo : pluginInfoRepository)
	{
		pluginNames.insert(pluginInfo.name());
		for (auto dependency : pluginInfo.dependencies())
			pluginNames.insert(dependency);
	}
	return pluginNames;
}
