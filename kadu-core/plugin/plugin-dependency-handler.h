/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "misc/iterator.h"

#include <map>
#include <memory>
#include <set>
#include <QtCore/QObject>
#include <QtCore/QPointer>

class PluginDependencyGraph;
class PluginDependencyGraphBuilder;
class PluginMetadata;
class PluginMetadataFinder;

class PluginDependencyHandler : public QObject
{
	Q_OBJECT

	using Storage = std::map<QString, PluginMetadata>;
	using WrappedIterator = Storage::iterator;

public:
	using Iterator = IteratorWrapper<WrappedIterator, PluginMetadata>;

	explicit PluginDependencyHandler(QObject *parent = nullptr);
	virtual ~PluginDependencyHandler();

	void setPluginDependencyGraphBuilder(PluginDependencyGraphBuilder *pluginDependencyGraphBuilder);
	void setPluginMetadataFinder(PluginMetadataFinder *pluginMetadataFinder);

	Iterator begin();
	Iterator end();

	void initialize();

	std::set<QString> pluginNames() const;
	bool hasPluginMetadata(const QString &pluginName) const;
	PluginMetadata pluginMetadata(const QString &pluginName) const;

	QVector<QString> withDependencies(const QString &pluginName) noexcept;
	QVector<QString> withDependents(const QString &pluginName) noexcept;

private:
	static PluginMetadata converter(WrappedIterator iterator);

	QPointer<PluginDependencyGraphBuilder> m_pluginDependencyGraphBuilder;
	QPointer<PluginMetadataFinder> m_pluginMetadataFinder;

	std::map<QString, PluginMetadata> m_allPluginMetadata;
	std::unique_ptr<PluginDependencyGraph> m_pluginDependencyDAG;

	void loadPluginMetadata();
	void prepareDependencyGraph();

};

inline PluginDependencyHandler::Iterator begin(PluginDependencyHandler *pluginDependencyHandler)
{
	return pluginDependencyHandler->begin();
}

inline PluginDependencyHandler::Iterator end(PluginDependencyHandler *pluginDependencyHandler)
{
	return pluginDependencyHandler->end();
}
