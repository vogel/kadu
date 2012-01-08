/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef PLUGIN_H
#define PLUGIN_H

#include <QtCore/QObject>

#include "plugins/plugins-common.h"
#include "plugins/plugins-manager.h"

#include "storage/named-storable-object.h"

class QLibrary;
class QPluginLoader;
class QTranslator;

class GenericPlugin;
class PluginInfo;

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
 * contain object that inherits from GenericPlugin and is registered by Q_EXPORT_PLUGIN2. Libraries
 * without that objects are invalid and will not be loaded.
 *
 * Plugin without .desc file is considered invalid or depreeated. That plugin wont be loaded or
 * activated, but its data is stored for use with PluginInfo::replaces() for replacement plugins.
 *
 * Each plugin can be in three states - PluginStateNew, PluginStateDisabled and PluginStateEnabled
 * Plugin is PluginStateNew, when it was not  known to Kadu at previous run (i.e.: installed just
 * before current run). New plugin can be loaded and activated without user interaction if it has
 * PluginInfo::loadByDefault() set to true.
 *
 * PluginStateEnabled plugins are activated (with dependencies) at Kadu start.
 *
 * PluginStateDisabled plugins are activated only as dependencies of other Plugins.
 *
 * Data of plugin (including state) is stored under /Plugins/Plugin[\@name=pluginName]/ node.
 *
 * Plugin can be activated manually by calling its activate() method, but wont be activated automatically
 * until its state is changed to PluginStateEnabled.
 *
 * Note: this class is not responsible for activating dependencies, see PluginsManager::activateDependencies()
 */
class Plugin : public QObject, public NamedStorableObject
{
	Q_OBJECT

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Plugin state.
	 *
	 * Plugin state
	 */
	enum PluginState
	{
		/**
		 * This plugin has been never successfully activated before. May be activated automatically
		 * if has PluginInfo::loadByDefault() set to true.
		 */
		PluginStateNew,
		/**
		 * This plugin is normally disabled and will be activated only as dependency of other plugin
		 * or at user request.
		 */
		PluginStateDisabled,
		/**
		 * This plugin is enabled and will be activated at Kadu start automatically.
		 */
		PluginStateEnabled
	};

private:
	QString Name;
	bool Active;
	PluginState State;
	bool FirstLoad;

	QPluginLoader *PluginLoader;
	GenericPlugin *PluginObject;

	QTranslator *Translator;
	PluginInfo *Info;
	int UsageCounter;

	void loadTranslations();
	void unloadTranslations();

private slots:
	void setStateEnabledIfInactive(bool enable);

protected:
	virtual void load();
	virtual void store();
	virtual bool shouldStore();

public:
	Plugin(const QString &name, QObject *parent = 0);
	virtual ~Plugin();

	// storage implementation
	virtual StorableObject* storageParent() { return PluginsManager::instance(); }
	virtual QString storageNodeName() { return QLatin1String("Plugin"); }
	virtual QString name() const { return Name; }

	bool shouldBeActivated();

	bool activate(PluginActivationReason reason);
	void deactivate(PluginDeactivationReason reason);

	void activationError(const QString &errorMessage, PluginActivationReason activationReason);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns true if plugin is valid.
	 * @return true if plugin is valid
	 *
	 * Returns true if plugin is valid. Plugin is valid when it has a .desc file.
	 */
	bool isValid() const { return 0 != Info; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns true if plugin is active.
	 * @return true if plugin is active
	 *
	 * Returns true if plugin is active. Plugin is active when its library file is loaded
	 * and object is created and available.
	 */
	bool isActive() const { return Active; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns plugin state.
	 * @return plugin state
	 *
	 * Returns plugin state.
	 *
	 * Note that plugin state describes only its state on configuration. All combinations
	 * of state() and isActive() are possible, except state PluginStateNew and plugin active.
	 */
	PluginState state() { ensureLoaded(); return State; }
	void setState(PluginState state);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns PluginInfo object for this plugin.
	 * @return PluginInfo object for this plugin
	 *
	 * Returns PluginInfo object for this plugin.
	 */
	PluginInfo * info() const { return Info; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns usage counter for this plugin.
	 * @return usage counter for this plugin
	 *
	 * Returns usage counter for this plugin. If usage counter is greater than zero, this plugin
	 * cannot be safely deactivated, because it is either dependency of another active plugin, of
	 * its code is currenly in use.
	 */
	int usageCounter() const { return UsageCounter; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Increases usage counter for this plugin.
	 *
	 * Increases usage counter for this plugin.
	 */
	void incUsage() { UsageCounter++; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Decreases usage counter for this plugin.
	 *
	 * Decreases usage counter for this plugin.
	 */
	void decUsage() { UsageCounter--; }

};

/**
 * @}
 */

#endif // PLUGIN_H
