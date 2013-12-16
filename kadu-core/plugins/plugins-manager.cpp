/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2008, 2009, 2010, 2010, 2011, 2012 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004, 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2004, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2010 Radosław Szymczyszyn (lavrin@gmail.com)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
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

#include "plugins-manager.h"

#include "configuration/configuration-file.h"
#include "configuration/configuration-manager.h"
#include "core/core.h"
#include "gui/hot-key.h"
#include "gui/windows/main-configuration-window.h"
#include "gui/windows/message-dialog.h"
#include "icons/icons-manager.h"
#include "misc/kadu-paths.h"
#include "plugin-repository.h"
#include "plugins/dependency-graph/plugin-dependency-cycle-exception.h"
#include "plugins/dependency-graph/plugin-dependency-graph.h"
#include "plugins/dependency-graph/plugin-dependency-graph-builder.h"
#include "plugins/generic-plugin.h"
#include "plugins/plugin-activation-action.h"
#include "plugins/plugin-activation-service.h"
#include "plugins/plugin-info-reader-exception.h"
#include "plugins/plugin-info-reader.h"
#include "plugins/plugin-info.h"
#include "plugins/plugin.h"
#include "plugins/plugins-common.h"
#include "plugin-info-reader.h"
#include "activate.h"
#include "debug.h"

#include <QtCore/QDir>
#include <QtCore/QLibrary>
#include <QtCore/QPluginLoader>
#include <QtCore/QTextCodec>
#include <QtGui/QApplication>
#include <QtGui/QCheckBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QScrollBar>
#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QVBoxLayout>

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Creates new PluginsManager and registers it in ConfigurationManager singleton.
 *
 * Creates new PluginsManager, registers it in ConfigurationManager singleton.
 * Storage status is set to Storage::StateNotLoaded.
 */
PluginsManager::PluginsManager(QObject *parent) :
		QObject{parent}
{
	ConfigurationManager::instance()->registerStorableObject(this);

	setState(StateNotLoaded);
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Destroys instance and unregisters it from ConfigurationManager singleton.
 *
 * Destroys instance and unregisters it from ConfigurationManager singleton.
 */
PluginsManager::~PluginsManager()
{
	ConfigurationManager::instance()->unregisterStorableObject(this);
}

void PluginsManager::setPluginActivationService(PluginActivationService *pluginActivationService)
{
	m_pluginActivationService = pluginActivationService;
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Loads PluginsManager and all Plugin configurations.
 *
 * This method loads PluginsManager configuration from storage node /root/Plugins and all sub nodes
 * /root/Plugins/Plugin. If attribute /root/Plugins/\@imported_from_09 is not present importFrom09()
 * method will be called to import depreceated configuration from 0.9.x and earlier versions.
 *
 * After reading all plugins configuration this method check for existence of new plugins that could
 * be recently installed. Check is done by searching datadir/kadu/plugins directory for new *.desc
 * files. All new plugins are set to have Plugin::PluginStateNew state.
 */
void PluginsManager::load()
{
	if (!isValidStorage())
		return;

	StorableObject::load();

	for (auto pluginName : installedPlugins())
	{
		auto plugin = loadPlugin(pluginName);
		if (plugin)
			Core::instance()->pluginRepository()->addPlugin(pluginName, plugin);
	}

	auto dependencyGraph = Core::instance()->pluginDependencyGraphBuilder()->buildGraph(*Core::instance()->pluginRepository());
	auto pluginsInDependencyCycle = dependencyGraph.get()->findPluginsInDependencyCycle();
	for (auto &pluginInDependency : pluginsInDependencyCycle)
		Core::instance()->pluginRepository()->removePlugin(pluginInDependency);

	m_pluginDependencyGraph = Core::instance()->pluginDependencyGraphBuilder()->buildGraph(*Core::instance()->pluginRepository());

	if (!loadAttribute<bool>("imported_from_09", false))
	{
		importFrom09();
		storeAttribute("imported_from_09", true);
	}
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Stores PluginsManager and all Plugin configurations.
 *
 * This method stores PluginsManager configuration to storage node /root/Plugins and all sub nodes to
 * /root/Plugins/Plugin. Attribute /root/Plugins/\@imported_from_09 is always stored as "true".
 */
void PluginsManager::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	StorableObject::store();

	for (auto plugin : Core::instance()->pluginRepository())
		plugin->ensureStored();
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Import 0.9.x configuration.
 *
 * This method loads old configuration from depreceated configuration entries: General/EverLaoded,
 * General/LoadedModules and General/UnloadedModules. Do not call it manually.
 */
void PluginsManager::importFrom09()
{
	auto everLoaded = config_file.readEntry("General", "EverLoaded").split(',', QString::SkipEmptyParts);
	auto loaded = config_file.readEntry("General", "LoadedModules");

	auto loadedPlugins = loaded.split(',', QString::SkipEmptyParts);
	everLoaded += loadedPlugins;
	auto unloaded_str = config_file.readEntry("General", "UnloadedModules");
	auto unloadedPlugins = unloaded_str.split(',', QString::SkipEmptyParts);

	auto allPlugins = everLoaded + unloadedPlugins; // just in case...
	QMap<QString, Plugin *> oldPlugins;
	for (auto pluginName : allPlugins)
		if (!Core::instance()->pluginRepository()->hasPlugin(pluginName) && !oldPlugins.contains(pluginName))
		{
			auto plugin = loadPlugin(pluginName);
			if (plugin)
				oldPlugins.insert(pluginName, plugin);
		}

	if (loadedPlugins.contains("encryption"))
	{
		loadedPlugins.removeAll("encryption");
		loadedPlugins.append("encryption_ng");
		loadedPlugins.append("encryption_ng_simlite");
	}
	if (loadedPlugins.contains("osd_hints"))
	{
		loadedPlugins.removeAll("osd_hints");
		if (!loadedPlugins.contains("hints"))
			loadedPlugins.append("hints");
	}

	for (auto plugin : Core::instance()->pluginRepository())
		if (allPlugins.contains(plugin->name()))
		{
			if (loadedPlugins.contains(plugin->name()))
				plugin->setState(Plugin::PluginStateEnabled);
			else if (everLoaded.contains(plugin->name()))
				plugin->setState(Plugin::PluginStateDisabled);
		}

	for (auto plugin : oldPlugins.values())
		if (allPlugins.contains(plugin->name()))
		{
			if (loadedPlugins.contains(plugin->name()))
				plugin->setState(Plugin::PluginStateEnabled);
			else if (everLoaded.contains(plugin->name()))
				plugin->setState(Plugin::PluginStateDisabled);
		}

	for (auto plugin : oldPlugins)
	{
		plugin->ensureStored();
		plugin->deleteLater();
	}
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Activate all protocols plugins that are enabled.
 *
 * This method activates all plugins with type "protocol" that are either enabled (Plugin::PluginStateEnabled)
 * or new (Plugin::PluginStateNew) with attribute "load by default" set. This method is generally called before
 * any other activation to ensure that all protocols and accounts are available for other plugins.
 */
void PluginsManager::activateProtocolPlugins()
{
	auto saveList = false;

	for (auto plugin : Core::instance()->pluginRepository())
	{
		if (plugin->info().type() != "protocol")
			continue;

		if (plugin->shouldBeActivated())
		{
			auto activationReason = (plugin->state() == Plugin::PluginStateNew)
					? PluginActivationReason::NewDefault
					: PluginActivationReason::KnownDefault;

			if (!activatePlugin(plugin, activationReason))
				saveList = true;
		}
	}

	// if not all plugins were loaded properly
	// save the list of plugins
	if (saveList)
		ConfigurationManager::instance()->flush();
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Activate all plugins that are enabled.
 *
 * This method activates all plugins that are either enabled (Plugin::PluginStateEnabled) or new (Plugin::PluginStateNew)
 * with attribute "load by default" set. If given enabled plugin is no longer available replacement plugin is searched
 * (by checking Plugin::replaces()). Any found replacement plugin is activated.
 */
void PluginsManager::activatePlugins()
{
	auto saveList = false;

	for (auto plugin : Core::instance()->pluginRepository())
		if (plugin->shouldBeActivated())
		{
			auto activationReason = (plugin->state() == Plugin::PluginStateNew)
					? PluginActivationReason::NewDefault
					: PluginActivationReason::KnownDefault;

			if (!activatePlugin(plugin, activationReason))
				saveList = true;
		}

	for (auto pluginToReplace : Core::instance()->pluginRepository())
	{
		if (pluginToReplace->isActive() || pluginToReplace->state() != Plugin::PluginStateEnabled)
			continue;

		for (auto replacementPlugin : Core::instance()->pluginRepository())
			if (replacementPlugin->state() == Plugin::PluginStateNew
					&& replacementPlugin->info().replaces().contains(pluginToReplace->name()))
				if (activatePlugin(replacementPlugin, PluginActivationReason::NewDefault))
					saveList = true; // list has changed
	}

	// if not all plugins were loaded properly or new plugin was added
	// save the list of plugins
	if (saveList)
		ConfigurationManager::instance()->flush();
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Activate all plugins that are enabled.
 *
 * This method deactivated all active plugins. First iteration of deactivation check Plugin::usageCounter() value
 * to check if given plugin can be safely removed (no other active plugins depends on it). This procedure is
 * performed for all active plugins until no more plugins can be deactivated. Then second iteration is performed.
 * This time no checks are performed.
 */
void PluginsManager::deactivatePlugins()
{
	for (auto plugin : Core::instance()->pluginRepository())
		if (plugin->isActive())
		{
			kdebugm(KDEBUG_INFO, "plugin: %s, usage: %d\n", qPrintable(plugin->name()), plugin->usageCounter());
		}

	// unloading all not used plugins
	// as long as any plugin were unloaded

	bool deactivated;
	do
	{
		auto active = activePlugins();
		deactivated = false;
		for (auto plugin : active)
			if (plugin->usageCounter() == 0)
				if (deactivatePlugin(plugin, PluginDeactivationReason::Exiting))
					deactivated = true;
	}
	while (deactivated);

	// we cannot unload more plugins in normal way
	// so we are making it brutal ;)
	auto active = activePlugins();
	for (auto plugin : active)
	{
		kdebugm(KDEBUG_PANIC, "WARNING! Could not deactivate plugin %s, killing\n", qPrintable(plugin->name()));
		deactivatePlugin(plugin, PluginDeactivationReason::ExitingForce);
	}

}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Lists all active plugins.
 * @return list of all active plugins
 *
 * This method returns list of all active plugins. Active plugin has its shred library loaded and objects
 * created.
 */
QList<Plugin *> PluginsManager::activePlugins() const
{
	auto result = QList<Plugin *>{};
	for (auto plugin : Core::instance()->pluginRepository())
		if (plugin->isActive())
			result.append(plugin);
	return result;
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Marks all dependencies of given plugin as used.
 * @param plugin plugin which dependencies will be marked as used
 *
 * All depenciec of plugin given as parameter will be marked as used so no accidental deactivation
 * will be possible for them.
 */
void PluginsManager::incDependenciesUsageCount(Plugin *plugin)
{
	kdebugmf(KDEBUG_FUNCTION_START, "%s\n", qPrintable(plugin->info().description()));
	for (auto const &pluginName : plugin->info().dependencies())
	{
		kdebugm(KDEBUG_INFO, "incUsage: %s\n", qPrintable(pluginName));
		usePlugin(pluginName);
	}
	kdebugf2();
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Lists all installed plugins names.
 * @return list of all installed plugin names
 *
 * Lists all installed plugins names. Installed plugins are searched in dataDir/kadu/plugins as
 * *.desc files.
 */
QStringList PluginsManager::installedPlugins() const
{
	auto dir = QDir{KaduPaths::instance()->dataPath() + QLatin1String("plugins"), "*.desc"};
	dir.setFilter(QDir::Files);

	auto installed = QStringList{};
	for (auto const &entry : dir.entryList())
		installed.append(entry.left(entry.length() - static_cast<int>(qstrlen(".desc"))));
	return installed;
}

Plugin * PluginsManager::loadPlugin(const QString &pluginName)
{
	auto descFilePath = QString("%1plugins/%2.desc").arg(KaduPaths::instance()->dataPath()).arg(pluginName);
	auto pluginInfoReader = Core::instance()->pluginInfoReader();
	if (!pluginInfoReader)
		return nullptr;

	try
	{
		auto pluginInfo = pluginInfoReader->readPluginInfo(pluginName, descFilePath);
		return new Plugin{pluginInfo, this};
	}
	catch (PluginInfoReaderException &)
	{
		return nullptr;
	}
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Returns name of active plugin that conflicts with given one.
 * @param plugin plugin for which conflict is searched
 * @return name of active plugins that conflicts with given one
 *
 * Return empty string if no active conflict plugin is found or if given plugin is empty or invalid.
 * In other cases name of active conflict plugin is returned. This means:
 * * any active plugin that is in current plugin's PluginInfo::conflicts() list
 * * any active plugin that provides (see PluginInfo::provides()) something that is in current plugin's PluginInfo::conflicts() list
 * * any active plugin that has current plugin it its PluginInfo::conflicts() list
 */
QString PluginsManager::findActiveConflict(Plugin *plugin) const
{
	if (!plugin)
		return {};

	for (auto const &conflict : plugin->info().conflicts())
	{
		// note that conflict may be something provided, not necessarily a plugin
		auto conflictingPlugin = Core::instance()->pluginRepository()->plugin(conflict);
		if (conflictingPlugin && conflictingPlugin->isActive())
			return conflict;

		for (auto possibleConflict : Core::instance()->pluginRepository())
			if (possibleConflict->isActive())
				for (auto const &provided : possibleConflict->info().provides())
					if (conflict == provided)
						return possibleConflict->name();
	}

	for (auto possibleConflict : Core::instance()->pluginRepository())
		if (possibleConflict->isActive())
			for (auto const &sit : possibleConflict->info().conflicts())
				if (sit == plugin->name())
					return plugin->name();

	return {};
}

QVector<Plugin *> PluginsManager::allDependencies(Plugin *plugin)
{
	if (!plugin || !m_pluginDependencyGraph)
		return {};

	auto result = QVector<Plugin *>{};
	auto dependencies = m_pluginDependencyGraph.get()->findDependencies(plugin->name());
	for (auto dependency : dependencies)
	{
		auto dependencyPlugin = Core::instance()->pluginRepository()->plugin(dependency);
		if (!dependencyPlugin)
		{
			// TODO: throw exception
			plugin->activationError(tr("Required plugin %1 was not found").arg(dependency), PluginActivationReason::Dependency);
			return {};
		}

		result += dependencyPlugin;
	}

	return result;
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Returns string with list of all plugins that depends on given one.
 * @param pluginName name of plugin to check dependencies
 * @return string with list of all active plugins that depend on given one
 * @todo ugly, should return QStringList or QList&lt;Plugin *&t;
 *
 * Returns string with list of all active plugins that depend on given one. This string can
 * be displayed to the user.
 */
QString PluginsManager::activeDependentPluginNames(const QString &pluginName) const
{
	auto plugins = QString{};

	for (auto possibleDependentPlugin : Core::instance()->pluginRepository())
		if (possibleDependentPlugin->isActive())
			if (possibleDependentPlugin->info().dependencies().contains(pluginName))
				plugins += "\n- " + possibleDependentPlugin->name();

	return plugins;
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Activates given plugin and all its dependencies.
 * @param plugin plugin to activate
 * @param reason plugin activation reason
 * @return true, if plugin was successfully activated
 *
 * This method activates given plugin and all its dependencies. Plugin can be activated only when no conflict
 * is found and all dependencies can be activated. In other case false is returned and plugin will not be activated.
 * Please note that no dependency plugin activated in this method will be automatically deactivated if
 * this method fails, so list of active plugins can be changed even if plugin could not be activated.
 *
 * \p reason will be passed to Plugin::activate() method.
 *
 * After successfull activation all dependencies are locked using incDependenciesUsageCount() and cannot be
 * deactivated without deactivating plugin. Plugin::usageCounter() of dependencies is increased.
 */
bool PluginsManager::activatePlugin(Plugin *plugin, PluginActivationReason reason)
{
	if (plugin->isActive())
		return true;

	auto conflict = findActiveConflict(plugin);
	if (!conflict.isEmpty())
	{
		plugin->activationError(tr("Plugin %1 conflicts with: %2").arg(plugin->name(), conflict), reason);
		return false;
	}

	try
	{
		auto dependencies = allDependencies(plugin);

		auto actions = QVector<PluginActivationAction>{};
		for (auto dependency : dependencies)
		{
			auto activationReason = PluginActivationReason{};
			if (Plugin::PluginStateEnabled == dependency->state())
				activationReason = PluginActivationReason::KnownDefault;
			else if (Plugin::PluginStateNew == dependency->state() && dependency->info().loadByDefault())
				activationReason = PluginActivationReason::NewDefault;
			else
				activationReason = PluginActivationReason::Dependency;

			actions.append({dependency, activationReason});
		}
		actions.append({plugin, reason});

		for (auto const &action : actions)
			if (!m_pluginActivationService.data()->performActivationAction(action))
				return false;

		incDependenciesUsageCount(plugin);
	}
	catch (PluginDependencyCycleException &e)
	{
		Q_UNUSED(e); // TODO: log? rethrow and use in GUI?

		return false;
	}

	return true;
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Deactivates given plugin.
 * @param plugin plugin to deactivate
 * @param reason plugin deactivation reason
 * @return true, if plugin was successfully deactivated
 * @todo remove message box
 *
 * This method deactivates given plugin. Deactivation can be performed only when plugin is no longed in use (its
 * Plugin::usageCounter() returns 0) or \p reason is ExitingForce.
 *
 * After successfull deactivation all dependenecies are released - their Plugin::usageCounter() is decreaced.
 */
bool PluginsManager::deactivatePlugin(Plugin* plugin, PluginDeactivationReason reason)
{
	kdebugmf(KDEBUG_FUNCTION_START, "name:'%s' reason:%d usage: %d\n", qPrintable(plugin->name()), (int)reason, plugin->usageCounter());

	if (plugin->usageCounter() > 0 && PluginDeactivationReason::ExitingForce != reason)
	{
		MessageDialog::show(KaduIcon("dialog-error"), tr("Kadu"), tr("Plugin %1 cannot be deactivated because it is being used by the following plugins:%2").arg(plugin->name()).arg(activeDependentPluginNames(plugin->name())),
				QMessageBox::Ok, MainConfigurationWindow::instance());
		kdebugf2();
		return false;
	}

	for (auto const &i : plugin->info().dependencies())
		releasePlugin(i);

	if (m_pluginActivationService)
		return m_pluginActivationService.data()->performActivationAction({plugin, reason});
	return false;
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Increases usage counter for given plugin
 * @param pluginName name of plugin to increase usage pointer
 *
 * This method increases usage counter for given plugin. Use it from plugin code when opening
 * dialogs or performing long operatrions to prevent unloading plugins. It is used also to prevent
 * unloading plugins when other loaded plugins are dependent on them.
 *
 * After plugin is no longer needes (operation has finished, dialog is closed) releasePlugin() must
 * be called with the same parameter.
 */
void PluginsManager::usePlugin(const QString &pluginName)
{
	auto plugin = Core::instance()->pluginRepository()->plugin(pluginName);
	if (plugin)
		plugin->incUsage();
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Decreases usage counter for given plugin
 * @param pluginName name of plugin to decrease usage pointer
 *
 * This method decreases usage counter for given plugin. Use it from plugin after calling usePlugin()
 * when locking plugin is no longer needed (for example when long operation has finished or a dialog
 * was closed).
 *
 * It can be only called after usePlugin() with the same parameter.
 */
void PluginsManager::releasePlugin(const QString &pluginName)
{
	auto plugin = Core::instance()->pluginRepository()->plugin(pluginName);
	if (plugin)
		plugin->decUsage();
}

#include "moc_plugins-manager.cpp"
