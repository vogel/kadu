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

#include <map>
#include <set>
#include <QtCore/QObject>

#include "plugin/dependency-graph/plugin-dependency-graph.h"
#include "exports.h"

class PluginMetadata;

/**
 * @addtogroup Plugin
 * @{
 */

/**
 * @class PluginDependencyGraphBuilder
 * @short Build plugin dependency graph from @see PluginMetadata objects.
 */
class KADUAPI PluginDependencyGraphBuilder : public QObject
{
	Q_OBJECT

public:
	explicit PluginDependencyGraphBuilder(QObject *parent = nullptr);
	virtual ~PluginDependencyGraphBuilder();

	/**
	 * @short Build plugin dependency graph from @see PluginMetadata objects.
	 * @param plugins List of @see PluginMetadata objects.
	 *
	 * Plugin dependency graph is build from all all relations that could be read from \p plugins
	 * parameter. In second phase all plugins that have to entry in \p plugins parameter are removed
	 * from graph with all of their dependents. So resulting graph contains only valid entries with
	 * associated metadata.
	 */
	PluginDependencyGraph buildValidGraph(const std::map<QString, PluginMetadata> &plugins) const;

private:
	PluginDependencyGraph buildGraph(const std::map<QString, PluginMetadata> &plugins) const;
	std::set<QString> invalidPlugins(const PluginDependencyGraph &graph, const std::map<QString, PluginMetadata> &plugins) const;
	std::map<QString, PluginMetadata> filtered(const std::map<QString, PluginMetadata> &original, const std::set<QString> &invalid) const;

};

/**
 * @}
 */
