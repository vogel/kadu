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

#include "misc/change-notifier.h"
#include "exports.h"

#include <QtCore/QMap>
#include <QtCore/QObject>

enum class PluginState;

/**
 * @addtogroup Plugin
 * @{
 */

/**
 * @class PluginStateService
 * @short Handles states of all known plugins.
 *
 * Each plugin name has a assigned plugin state (see PluginState) As no valid plugin
 * metadata object is required this service can also remember states of old and
 * uninstalled plugins (that helps with configuration migration).
 *
 * Use pluginStates() and setPluginStates() to manipulate all plugin states.
 * Use pluginState() and setPluginState() to manipulate one plugin state.
 *
 * In addition pluginsWithState() method is available to return all plugin name with
 * given state.
 */
class KADUAPI PluginStateService : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit PluginStateService(QObject *parent = nullptr) noexcept;
	virtual ~PluginStateService() noexcept;

	/**
	 * @return Plugin states for all known plugin names.
	 */
	QMap<QString, PluginState> pluginStates() const noexcept;

	/**
	 * @short Change map of known plugin states.
	 * @param pluginState new map of known plugin states
	 *
	 * This method removes all currently stored data and replaces it with \p pluginStates value.
	 * Signal pluginStateChanged is emited for each plugin with new state different than previous.
	 * It is also emited for removed plugins (with PluginState::New). In such cases @see changeNotifier()
	 * is also notified.
	 */
	void setPluginStates(QMap<QString, PluginState> pluginStates) noexcept;

	/**
	 * @param pluginName name of plugin
	 * @return Plugin state for given plugin name.
	 *
	 * If no plugin state is known for this plugin name, PluginState::New is returned.
	 */
	PluginState pluginState(const QString &pluginName) const noexcept;

	/**
	 * @short Set new state for plugin.
	 * @param pluginName name of plugin
	 * @param state new state of plugin
	 *
	 * If new state is PluginState::New, entry is removed. Otherwise, if no state was known for this plugin
	 * before, a new entry is added. Otherwise, existing entry is updated to match new value.
	 *
	 * If new state is different than previous, signal pluginStateChanged is emited and @see changeNotifier()
	 * it notified.
	 */
	void setPluginState(const QString &pluginName, PluginState state) noexcept;

	/**
	 * @return All plugin with state PluginState::Enabled.
	 * @param state state
	 */
	QList<QString> enabledPlugins() noexcept;

	/**
	 * @return Change notifier called each time a plugin state changes.
	 */
	ChangeNotifier & changeNotifier();

signals:
	/**
	 * @short Signal emited when plugin state changes for one plugin.
	 * @param pluginName name of plugin with changed state
	 * @param state new state of plugin
	 *
	 * This signal is not invoked after calling setPluginState with state equal to current state of plugin.
	 */
	void pluginStateChanged(const QString &pluginName, PluginState state);

private:
	QMap<QString, PluginState> m_pluginStates;
	ChangeNotifier m_changeNotifier;

};

/**
 * @}
 */
