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

#include "misc/graph/graph.h"
#include "exports.h"

#include <set>
#include <QtCore/QSet>
#include <QtCore/QVector>

/**
 * @addtogroup Plugin
 * @{
 */

/**
 * @class PluginDependencyGraph
 * @short Graph of dependencies between plugins.
 *
 * Dependencies are two directional - each plugin has list of dependencies and list of
 * dependents. For each 'dependency' relation there is corresponding 'dependent' one.
 *
 * Dependency graph is created by adding plugins and relations between them. If dependency
 * relation is created for plugins that are not yet in the graph - they are added to it.
 *
 * All plugin that have dependency cycle can be obtained by calling findPluginsInDependencyCycle.
 */
class KADUAPI PluginDependencyGraph
{
	struct PluginDependencyTag {};
	struct PluginDependentTag {};

public:
	PluginDependencyGraph() = default;
	PluginDependencyGraph(const PluginDependencyGraph &) = delete;
	PluginDependencyGraph(PluginDependencyGraph &&) = default;

	PluginDependencyGraph & operator = (const PluginDependencyGraph &) = delete;
	PluginDependencyGraph & operator = (PluginDependencyGraph &&) = default;

	/**
	 * @short Add plugin to graph.
	 * @param pluginName name of plugin to add
	 *
	 * If plugin is already in dependency graph, nothing is done. In other case it is added
	 * to it without any dependencies.
	 */
	void addPlugin(const QString &pluginName);

	/**
	 * @short Add dependency relation to graph.
	 * @param dependentPluginName name of dependent plugin
	 * @param dependencyPluginName name of dependency plugin
	 *
	 * Add dependency relation fo two given plugins. Also adds dependent relation in the other way.
	 * If given plugins were not in the graph already then they are added.
	 */
	void addDependency(const QString &dependentPluginName, const QString &dependencyPluginName);

	/**
	 * @return Number of plugins in graph.
	 */
	int size() const;

	/**
	 * @return List of plugins in graph.
	 */
	std::set<QString> plugins() const;

	/**
	 * @param pluginName plugin to get dependencies for
	 * @return Set of direct dependencies of given plugin.
	 */
	QSet<QString> directDependencies(const QString &pluginName) const;

	/**
	 * @param pluginName plugin to get dependents for
	 * @return Set of direct dependents of given plugin.
	 */
	QSet<QString> directDependents(const QString &pluginName) const;

	/**
	 * @return set of plugins that have dependency cycle
	 */
	QSet<QString> findPluginsInDependencyCycle() const;

	/**
	 * @param pluginName name of plugin to find all dependencies for
	 * @return return sorted list of dependencies for given plugin
	 * @throws PluginDependencyCycleException
	 *
	 * This method returns topological-sorted list of dependencies for given plugin. It means that
	 * plugins can be properly activated in order of this list. If such list could not be computed
	 * due to cycle in graph, PluginDependencyCycleException is thrown.
	 */
	QVector<QString> findDependencies(const QString &pluginName) const noexcept(false);

	/**
	 * @param pluginName name of plugin to find all dependents for
	 * @return return sorted list of dependents for given plugin
	 * @throws PluginDependencyCycleException
	 *
	 * This method returns topological-sorted list of dependents for given plugin. It means that
	 * plugins can be properly deactivated in order of this list. If such list could not be computed
	 * due to cycle in graph, PluginDependencyCycleException is thrown.
	 */
	QVector<QString> findDependents(const QString &pluginName) const noexcept(false);

private:
	using PluginGraph = Graph<QString, PluginDependencyTag, PluginDependentTag>;
	PluginGraph m_graph;

	template<typename SuccessorTypeTag>
	QSet<QString> directSuccessors(const QString &pluginName) const;

	template<typename SuccessorTypeTag>
	QVector<QString> findSuccessors(const QString &pluginName) const noexcept(false);

};

/**
 * @}
 */
