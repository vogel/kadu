/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "plugin/dependency-graph/plugin-dependency-graph.h"
#include "plugin/metadata/plugin-metadata.h"
#include "exports.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <map>
#include <memory>
#include <set>
#include <injeqt/injeqt.h>

class PluginDependencyGraphBuilder;
class PluginMetadataProvider;

/**
 * @addtogroup Plugin
 * @{
 */

/**
 * @class PluginDependencyHandler
 * @short Handles dependencies of installed plugins.
 * @todo Think if something like PluginMetadataRepository could be extracted from it.
 *
 * This service is used to compute dependency lists of all installed plugins. List of plugins
 * is fetched from PluginMetadataProvider services. Then all plugins that form dependency cycles
 * are removed from list and dependency graph is created from remaining ones.  Metadata for these
 * plugins can be retrieived using for-range loop (as begin() and end() methods are implemented).
 *
 * Plugin list with all of its dependencies (or dependents) can be easily obrained by withDependencies
 * and withDependents methods.
 */
class KADUAPI PluginDependencyHandler : public QObject
{
	Q_OBJECT

	using Storage = std::map<QString, PluginMetadata>;
	using WrappedIterator = Storage::iterator;

public:
	using Iterator = IteratorWrapper<WrappedIterator, PluginMetadata>;

	Q_INVOKABLE explicit PluginDependencyHandler(QObject *parent = nullptr);
	virtual ~PluginDependencyHandler();

	/**
	 * @return Return begin iterator for PluginMetadata list.
	 */
	Iterator begin();

	/**
	 * @return Return end iterator for PluginMetadata list.
	 */
	Iterator end();

	/**
	 * @short Initialize service data.
	 *
	 * This method must be called after all services were set.
	 * List of plugin metadata is read and dependency graph is created.
	 */
	void initialize();

	/**
	 * @return Set of names of plugins that are not in dependency cycle.
	 */
	std::set<QString> pluginNames() const;

	/**
	 * @param pluginName Plugin name to check.
	 * @return True if metadata for given plugin was loaded and this plugin is not in dependency cycle.
	 */
	bool hasPluginMetadata(const QString &pluginName) const;

	/**
	 * @param pluginName Plugin name to get metadata for.
	 * @return Metadata for plugin with given name.
	 * @throws std::out_of_range if plugin metadata not available.
	 *
	 * Before calling this method check that plugin metadata exists with @see hasPluginMetadata.
	 */
	PluginMetadata pluginMetadata(const QString &pluginName) const;

	/**
	 * @param pluginName Plugin name to get list of dependencies for.
	 * @return Tophographically sorted list of plugin with dependencies.
	 *
	 * If dependency graph was not created or plugin is not known an empty list is returned. Else a list
	 * of tophographically sorted dependencies for plugin is returned (with provided plugin name
	 * at the end).
	 */
	QVector<QString> withDependencies(const QString &pluginName) const;

	/**
	 * @param pluginName Plugin name to get list of dependencies for.
	 * @return Tophographically sorted list of dependencies of plugin.
	 *
	 * If dependency graph was not created or plugin is not known an empty list is returned. Else a list
	 * of tophographically sorted dependencies for plugin is returned (without provided plugin name).
	 */
	QVector<QString> findDependencies(const QString &pluginName) const;

	/**
	 * @param pluginName Plugin name to get list of dependents for.
	 * @return Tophographically sorted list of plugin with dependents.
	 *
	 * If dependency graph was not created or plugin is not known an empty list is returned. Else a list
	 * of tophographically sorted dependents for plugin is returned (with provided plugin name
	 * at the end).
	 */
	QVector<QString> withDependents(const QString &pluginName) const;

	/**
	 * @param pluginName Plugin name to get list of dependents for.
	 * @return Tophographically sorted list of dependents of plugin.
	 *
	 * If dependency graph was not created or plugin is not known an empty list is returned. Else a list
	 * of tophographically sorted dependents for plugin is returned (without provided plugin name).
	 */
	QVector<QString> findDependents(const QString &pluginName) const;

private:
	static PluginMetadata converter(WrappedIterator iterator);

	QPointer<PluginDependencyGraphBuilder> m_pluginDependencyGraphBuilder;
	QPointer<PluginMetadataProvider> m_pluginMetadataProvider;

	std::map<QString, PluginMetadata> m_allPluginMetadata;
	PluginDependencyGraph m_pluginDependencyDAG;

public slots: // TODO: make private, public only because of tests, should be done by injector
	/**
	 * @short Set PluginDependencyGraphBuilder service.
	 *
	 * PluginDependencyGraphBuilder is used to create dependency graph from metadata loaded by
	 * PluginMetadataProvider.
	 */
	INJEQT_SETTER void setPluginDependencyGraphBuilder(PluginDependencyGraphBuilder *pluginDependencyGraphBuilder);

	/**
	 * @short Set PluginMetadataProvider service.
	 *
	 * PluginMetadataProvider is used to get list of all plugins available for application.
	 */
	INJEQT_SETTER void setPluginMetadataProvider(PluginMetadataProvider *pluginMetadataProvider);

};

/**
 * @return Return begin iterator for PluginMetadata list stored in \p pluginDependencyHandler.
 */
inline PluginDependencyHandler::Iterator begin(PluginDependencyHandler *pluginDependencyHandler)
{
	return pluginDependencyHandler->begin();
}

/**
 * @return Return end iterator for PluginMetadata list stored in \p pluginDependencyHandler.
 */
inline PluginDependencyHandler::Iterator end(PluginDependencyHandler *pluginDependencyHandler)
{
	return pluginDependencyHandler->end();
}

/**
 * @}
 */
