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
#include "gui/windows/plugin-error-dialog.h"
#include "icons/icons-manager.h"
#include "misc/kadu-paths.h"
#include "plugins/dependency-graph/plugin-dependency-cycle-exception.h"
#include "plugins/dependency-graph/plugin-dependency-graph.h"
#include "plugins/dependency-graph/plugin-dependency-graph-builder.h"
#include "plugins/plugin-root-component.h"
#include "plugins/plugin-activation-action.h"
#include "plugins/plugin-activation-error-exception.h"
#include "plugins/plugin-activation-service.h"
#include "plugins/plugin-info-reader-exception.h"
#include "plugins/plugin-info-reader.h"
#include "plugins/plugin-info.h"
#include "plugins/plugin-repository.h"
#include "plugins/plugin.h"
#include "plugins/plugins-common.h"
#include "activate.h"
#include "debug.h"

#include <QtCore/QDir>
#include <QtCore/QLibrary>
#include <QtCore/QPluginLoader>
#include <QtCore/QTextCodec>
#include <QtCore/QTimer>
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

	m_pluginDependencyDAG = Core::instance()->pluginDependencyGraphBuilder()->buildGraph(*Core::instance()->pluginRepository());

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
	auto everLoaded = config_file.readEntry("General", "EverLoaded").split(',', QString::SkipEmptyParts).toSet();
	auto loaded = config_file.readEntry("General", "LoadedModules");

	auto loadedPlugins = loaded.split(',', QString::SkipEmptyParts).toSet();
	everLoaded += loadedPlugins;
	auto unloaded_str = config_file.readEntry("General", "UnloadedModules");
	auto unloadedPlugins = unloaded_str.split(',', QString::SkipEmptyParts).toSet();

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
		loadedPlugins.remove("encryption");
		loadedPlugins.insert("encryption_ng");
		loadedPlugins.insert("encryption_ng_simlite");
	}
	if (loadedPlugins.contains("osd_hints"))
	{
		loadedPlugins.remove("osd_hints");
		loadedPlugins.insert("hints");
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

		if (shouldActivate(plugin))
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
	if (!m_pluginActivationService)
		return;

	auto saveList = false;

	for (auto plugin : Core::instance()->pluginRepository())
		if (shouldActivate(plugin))
		{
			auto activationReason = (plugin->state() == Plugin::PluginStateNew)
					? PluginActivationReason::NewDefault
					: PluginActivationReason::KnownDefault;

			if (!activatePlugin(plugin, activationReason))
				saveList = true;
		}

	for (auto pluginToReplace : Core::instance()->pluginRepository())
	{
		if (m_pluginActivationService.data()->isActive(pluginToReplace) || pluginToReplace->state() != Plugin::PluginStateEnabled)
			continue;

		auto replacementPlugin = findReplacementPlugin(pluginToReplace->name());
		if (replacementPlugin->state() == Plugin::PluginStateNew)
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
 * @short Returns true if this plugin should be activated.
 * @return true if this plugin should be activated
 *
 * Module should be activated only if:
 * <ul>
 *   <li>it is valid (has .desc file associated with it)
 *   <li>is either PluginStateEnabled or PluginStateNew with PluginInfo::loadByDefault() set to true
 * </ul>
 */
bool PluginsManager::shouldActivate(Plugin *plugin) const noexcept
{
	auto state = plugin->state();

	if (Plugin::PluginStateEnabled == state)
		return true;
	if (Plugin::PluginStateDisabled == state)
		return false;
	return plugin->info().loadByDefault();
}

Plugin * PluginsManager::findReplacementPlugin(const QString &pluginToReplace) const noexcept
{
	for (auto plugin : Core::instance()->pluginRepository())
		if (plugin->info().replaces().contains(pluginToReplace))
			return plugin;

	return {};
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
	auto active = activePlugins();
	for (auto plugin : active)
	{
		kdebugm(KDEBUG_INFO, "plugin: %s\n", qPrintable(plugin->name()));
		deactivatePlugin(plugin, PluginDeactivationReason::Exiting);
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
	if (!m_pluginActivationService)
		return result;

	for (auto plugin : Core::instance()->pluginRepository())
		if (m_pluginActivationService.data()->isActive(plugin))
			result.append(plugin);
	return result;
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
 * @short Returns name of active plugin that provides given feature.
 * @param feature feature to search
 * @return name of active plugins that conflicts provides given feature.
 */
QString PluginsManager::findActiveProviding(const QString &feature) const
{
	if (feature.isEmpty())
		return {};

	for (auto activePlugin : activePlugins())
		if (activePlugin->info().provides() == feature)
			return activePlugin->info().name();

	return {};
}

QVector<Plugin *> PluginsManager::allDependencies(Plugin *plugin) noexcept
{
	if (!plugin || !m_pluginDependencyDAG)
		return {};

	auto result = QVector<Plugin *>{};

	auto dependencies = m_pluginDependencyDAG.get()->findDependencies(plugin->name());
	for (auto dependency : dependencies)
	{
		auto dependencyPlugin = Core::instance()->pluginRepository()->plugin(dependency);
		if (!dependencyPlugin)
		{
			throw PluginActivationErrorException(nullptr, tr("Required plugin %1 was not found").arg(dependency));
		}

		result += dependencyPlugin;
	}

	return result;
}

QVector<Plugin *> PluginsManager::allDependents(Plugin *plugin) noexcept
{
	if (!plugin || !m_pluginDependencyDAG)
		return {};

	auto result = QVector<Plugin *>{};

	auto dependents = m_pluginDependencyDAG.get()->findDependents(plugin->name());
	for (auto dependent : dependents)
	{
		auto dependentPlugin = Core::instance()->pluginRepository()->plugin(dependent);
		if (dependentPlugin)
			result += dependentPlugin;
	}

	return result;
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
	if (!m_pluginActivationService)
		return false;

	if (m_pluginActivationService.data()->isActive(plugin))
		return true;

	auto conflict = findActiveProviding(plugin->info().provides());
	if (!conflict.isEmpty())
	{
		activationError(plugin, tr("Plugin %1 conflicts with: %2").arg(plugin->name(), conflict), reason);
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

		try
		{
			for (auto const &action : actions)
			{
				m_pluginActivationService.data()->performActivationAction(action);
				/* This is perfectly intentional. We have to set state to either enabled or disabled, as new
				 * means that it was never loaded. If the only reason to load the plugin was because some other
				 * plugin depended upon it, set state to disabled as we don't want that plugin to be loaded
				 * next time when its reverse dependency will not be loaded. Otherwise set state to enabled.
				 */
				action.plugin()->setState(Plugin::PluginStateDisabled);
			}
		}
		catch (PluginActivationErrorException &e)
		{
			activationError(e.plugin(), e.errorMessage(), PluginActivationReason::Dependency);
			return false;
		}

		try
		{
			m_pluginActivationService.data()->performActivationAction({plugin, reason});
			plugin->setState(Plugin::PluginStateEnabled);
		}
		catch (PluginActivationErrorException &e)
		{
			activationError(e.plugin(), e.errorMessage(), reason);
			return false;
		}
	}
	catch (PluginDependencyCycleException &e)
	{
		Q_UNUSED(e); // TODO: log? rethrow and use in GUI?

		return false;
	}

	return true;
}

/**
 * @author Bartosz 'beevvy' Brachaczek
 * @short Shows activation error to the user.
 * @param errorMessage error message that will be displayer to the user
 * @param activationReason plugin activation reason
 * @todo it really shouldn't call gui classes directly
 *
 * This method creates new PluginErrorDialog with message \p errorMessage and opens it. Depending on
 * \p activationReason, it also intructs the dialog wheter to offer the user choice wheter to try
 * to load this plugin automatically in future.
 */
void PluginsManager::activationError(Plugin *plugin, const QString &errorMessage, PluginActivationReason activationReason)
{
	if (!plugin)
		return;

	auto offerLoadInFutureChoice = (PluginActivationReason::KnownDefault == activationReason);

	// TODO: set parent to MainConfigurationWindow is it exists
	auto errorDialog = new PluginErrorDialog(plugin, errorMessage, offerLoadInFutureChoice, 0);
	if (offerLoadInFutureChoice)
		connect(errorDialog, SIGNAL(accepted(Plugin*,bool)), this, SLOT(setStateEnabledIfInactive(Plugin*,bool)));

	QTimer::singleShot(0, errorDialog, SLOT(open()));
}

void PluginsManager::deactivatePlugin(Plugin *plugin, PluginDeactivationReason reason)
{
	if (!m_pluginActivationService)
		return;

	if (!m_pluginActivationService.data()->isActive(plugin))
		return;

	auto dependents = allDependents(plugin);
	auto actions = QVector<PluginActivationAction>{};
	for (auto dependent : dependents)
		actions.append({dependent, reason});
	actions.append({plugin, reason});

	for (auto const &action : actions)
	{
		m_pluginActivationService.data()->performActivationAction(action);
		if (PluginDeactivationReason::UserRequest == reason)
			action.plugin()->setState(Plugin::PluginStateDisabled);
	}
}

/**
 * @author Bartosz 'beevvy' Brachaczek
 * @short Sets state enablement of plugin if it is inactive.
 *
 * If this plugin is active or its state is PluginStateNew, this method does nothing.
 *
 * Otherwise, this method sets its state to PluginStateEnabled if \p enable is true.
 * If \p enable is false, this method sets the plugin's state to PluginStateDisabled.
 */
void PluginsManager::setStateEnabledIfInactive(Plugin *plugin, bool enable)
{
	if (!m_pluginActivationService)
		return;

	if (m_pluginActivationService.data()->isActive(plugin))
		return;

	// It is necessary to not break firstLoad.
	if (Plugin::PluginStateNew == plugin->state())
		return;

	plugin->setState(enable ? Plugin::PluginStateEnabled : Plugin::PluginStateDisabled);
}

#include "moc_plugins-manager.cpp"
