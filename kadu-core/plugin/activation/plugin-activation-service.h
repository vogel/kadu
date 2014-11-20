/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "exports.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QSet>
#include <map>
#include <memory>
#include <injeqt/injeqt.h>

class ActivePlugin;
class PluginActivationErrorHandler;
class PluginDependencyHandler;
class PluginRootComponent;
class PluginStateService;

/**
 * @addtogroup Plugin
 * @{
 */

/**
 * @class PluginActivationService
 * @short Service for activating and deactivating plugin.
 * @note This class is not thread safe.
 *
 * Service used for activating and deactivating plugins. It holds all active plugins as
 * instances of ActivePlugin and allows for quering the list. Note that this service
 * perform full dependency resolving, so it also loads/unloads dependencies/dependents.
 */
class KADUAPI PluginActivationService : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit PluginActivationService(QObject *parent = nullptr);
	virtual ~PluginActivationService();

	/**
	 * @short Activates given plugin and all its dependencies.
	 * @param pluginName plugin to activate
	 * @return list of activated plugins
	 *
	 * This method activates given plugin and all its dependencies. Plugin can be activated only when no conflict
	 * is found and all dependencies can be activated. Returned vector contains list of all plugins that were
	 * in dependency set of given plugin (including this plugin) and were either already active or were successfully
	 * activated.
	 */
	QVector<QString> activatePluginWithDependencies(const QString &pluginName);

	/**
	 * @short Deactivates given plugin and all its dependents.
	 * @param pluginName plugin to deactivate
	 * @return list of deactivated plugins
	 *
	 * This method deactivates given plugin and all its dependents. Returned vector contains list of all plugins that were
	 * in dependents set of given plugin (including this plugin) and were either already inactive or were deactivated.
	 */
	QVector<QString> deactivatePluginWithDependents(const QString &pluginName);

	/**
	 * @param pluginName name of plugin to check
	 * @short True if plugin with \p pluginName is active.
	 */
	bool isActive(const QString &pluginName) const;

	/**
	* @return Names of all currently active plugins.
	*/
	QSet<QString> activePlugins() const;

	/**
	 * @param pluginName name of plugin
	 * @return root object from plugin with name pluginName
	 */
	PluginRootComponent * pluginRootComponent(const QString &pluginName) const;

private:
	using map = std::map<QString, std::unique_ptr<ActivePlugin>>;

	QPointer<PluginActivationErrorHandler> m_pluginActivationErrorHandler;
	QPointer<PluginDependencyHandler> m_pluginDependencyHandler;
	QPointer<PluginStateService> m_pluginStateService;

	map m_activePlugins;

	/**
	 * @short Activates plugin.
	 * @param pluginName name of plugin to activate
	 * @param firstTime if this plugin is activated for first firstTime
	 * @throws PluginActivationErrorException
	 *
	 * Creates new instance of ActivePlugin and adds it to set of active plugins.
	 * Throws PluginActivationErrorException. For more information about activation
	 * process see ActivePlugin.
	 */
	void activatePlugin(const QString &pluginName, bool firstTime);

	/**
	 * @short Deactivates plugin.
	 * @param pluginName name of plugin to deactivate
	 *
	 * Removes instance of ActivePlugin from set of active plugins. This triggers destruction
	 * of ActivePlugin and unloads plugin from memory. For more information about deactivation
	 * process see ActivePlugin.
	 */
	void deactivatePlugin(const QString &pluginName);

	/**
	 * @short Returns name of active plugin that provides given feature.
	 * @param feature feature to search
	 * @return name of active plugins that conflicts provides given feature.
	 */
	QString findActiveProviding(const QString &feature) const;

private slots:
	INJEQT_SETTER void setPluginActivationErrorHandler(PluginActivationErrorHandler *pluginActivationErrorHandler);
	INJEQT_SETTER void setPluginDependencyHandler(PluginDependencyHandler *pluginDependencyHandler);
	INJEQT_SETTER void setPluginStateService(PluginStateService *pluginStateService);

};

/**
 * @}
 */
