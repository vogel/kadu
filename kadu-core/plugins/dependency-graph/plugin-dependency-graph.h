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

#include <QtCore/QSet>
#include <QtCore/QVector>

class KADUAPI PluginDependencyGraph
{
	struct PluginDependencyTag {};
	struct PluginDependentTag {};

public:
	PluginDependencyGraph();
	PluginDependencyGraph(const PluginDependencyGraph &copyMe) = delete;
	PluginDependencyGraph(PluginDependencyGraph &&moveMe);

	PluginDependencyGraph & operator = (const PluginDependencyGraph &copyMe) = delete;
	PluginDependencyGraph & operator = (PluginDependencyGraph &&moveMe);

	void addPlugin(const QString &pluginName);
	void addDependency(const QString &dependentPluginName, const QString &dependencyPluginName);

	int size() const;
	QSet<QString> directDependencies(const QString &pluginName) const;
	QSet<QString> directDependents(const QString &pluginName) const;

	QSet<QString> findPluginsInDependencyCycle() const;
	QVector<QString> findDependencies(const QString &pluginName) const noexcept(false);
	QVector<QString> findDependents(const QString &pluginName) const noexcept(false);

private:
	Graph<QString, PluginDependencyTag, PluginDependentTag> m_graph;

	template<typename SuccessorTypeTag>
	QSet<QString> directSuccessors(const QString &pluginName) const;

	template<typename SuccessorTypeTag>
	QVector<QString> findSuccessors(const QString &pluginName) const noexcept(false);

};
