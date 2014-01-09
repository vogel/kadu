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

#include "exports.h"

#include <QtCore/QObject>
#include <QtCore/QSet>
#include <map>
#include <memory>

class ActivePlugin;

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
 * instances of @see ActivePlugin and allows for quering the list. Note that this service
 * does not perform any dependency resolving, so it is only able to load plugins for which
 * all dependencies were already loaded. For dependency resolving loading see @see PluginManager.
 */
class KADUAPI PluginActivationService : public QObject
{
	Q_OBJECT

public:
	explicit PluginActivationService(QObject *parent = nullptr);
	virtual ~PluginActivationService();

	/**
	 * @short Activates plugin.
	 * @param pluginName name of plugin to activate
	 * @param firstTime if this plugin is activated for first firstTime
	 * @throws PluginActivationErrorException
	 *
	 * Creates new instance of @see ActivePlugin and adds it to set of active plugins.
	 * Throws @see PluginActivationErrorException. For more information about activation
	 * process see @see ActivePlugin.
	 */
	void activatePlugin(const QString &pluginName, bool firstTime) noexcept(false);

	/**
	 * @short Deactivates plugin.
	 * @param pluginName name of plugin to deactivate
	 *
	 * Removes instance of @see ActivePlugin from set of active plugins. This triggers destruction
	 * of ActivePlugin and unloads plugin from memory. For more information about deactivation
	 * process see @see ActivePlugin.
	 */
	void deactivatePlugin(const QString &pluginName) noexcept;

	/**
	 * @param pluginName name of plugin to check
	 * @short True if plugin with \p pluginName is active.
	 */
	bool isActive(const QString &pluginName) const noexcept;

	/**
	* @return Names of all currently active plugins.
	*/
	QSet<QString> activePlugins() const noexcept;

private:
	using map = std::map<QString, std::unique_ptr<ActivePlugin>>;
	map m_activePlugins;

};

/**
 * @}
 */
