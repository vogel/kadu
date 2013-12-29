/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "core/core.h"
#include "plugins/plugins-manager.h"
#include "exports.h"

#include "storage/named-storable-object.h"

#include <QtCore/QObject>

enum class PluginActivationReason;
enum class PluginDeactivationReason;
enum class PluginState;

/**
 * @addtogroup Plugins
 * @{
 */

/**
 * @class Plugin
 * @author Rafał 'Vogel' Malinowski
 * @short Class responsible for activating, deactivating and managing single plugin.
 * @todo Rewrite to use .desktop files
 *
 * This class manages single plugin, loads and stored its state data from configuration.
 *
 * Plugin is represented by two files in file system: one is dataDir/kadu/plugins/[pluginName].desc
 * that contains all required information for plugin (loaded by PluginInfo class) and
 * libDir/kadu/plugins/[pluginName].so/dll that contains plugin code and objects. Library file must
 * contain object that inherits from PluginRootComponent and is registered by Q_EXPORT_PLUGIN2. Libraries
 * without that objects are invalid and will not be loaded.
 *
 * Plugin without .desc file is considered invalid or depreeated. That plugin wont be loaded or
 * activated, but its data is stored for use with PluginInfo::replaces() for replacement plugins.
 *
 * Each plugin can be in three states - PluginState::New, PluginState::Disabled and PluginState::Enabled
 * Plugin is PluginState::New, when it was not  known to Kadu at previous run (i.e.: installed just
 * before current run). New plugin can be loaded and activated without user interaction if it has
 * PluginInfo::loadByDefault() set to true.
 *
 * PluginState::Enabled plugins are activated (with dependencies) at Kadu start.
 *
 * PluginState::Disabled plugins are activated only as dependencies of other Plugins.
 *
 * Data of plugin (including state) is stored under /Plugins/Plugin[\@name=pluginName]/ node.
 *
 * Plugin can be activated manually by calling its activate() method, but wont be activated automatically
 * until its state is changed to PluginState::Enabled.
 *
 * Note: this class is not responsible for activating dependencies, see PluginsManager::activateDependencies()
 */
class KADUAPI Plugin : public QObject, public NamedStorableObject
{
	Q_OBJECT

public:
	explicit Plugin(QString name, QObject *parent = nullptr);
	virtual ~Plugin();

	// storage implementation
	virtual StorableObject* storageParent() override { return Core::instance()->pluginsManager(); }
	virtual QString storageNodeName() override { return QLatin1String{"Plugin"}; }
	virtual QString name() const override { return m_name; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns plugin state.
	 * @return plugin state
	 *
	 * Returns plugin state.
	 *
	 * Note that plugin state describes only its state on configuration. All combinations
	 * of state() and isActive() are possible, except state PluginState::New and plugin active.
	 */
	PluginState state() { ensureLoaded(); return m_state; }
	void setState(PluginState state);

protected:
	virtual void load();
	virtual void store();
	virtual bool shouldStore();

private:
	QString m_name;
	PluginState m_state;

};

/**
 * @}
 */
