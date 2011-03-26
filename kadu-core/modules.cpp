/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2004, 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010 Radosław Szymczyszyn (lavrin@gmail.com)
 * Copyright 2004, 2005 Paweł Płuciennik (pawel_p@kadu.net)
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

#ifndef Q_WS_WIN
#include <dlfcn.h>
#endif

#include "configuration/configuration-file.h"
#include "configuration/configuration-manager.h"
#include "core/core.h"
#include "gui/hot-key.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/modules-window.h"
#include "gui/windows/message-dialog.h"
#include "misc/path-conversion.h"
#include "plugins/generic-plugin.h"
#include "plugins/plugin.h"
#include "plugins/plugin-info.h"
#include "activate.h"
#include "debug.h"
#include "icons-manager.h"

#include "modules.h"

#ifdef Q_OS_MAC
	#define SO_EXT "so"
	#define SO_EXT_LEN 2
	#define SO_PREFIX "lib"
	#define SO_PREFIX_LEN 3
#elif defined(Q_OS_WIN)
	#define SO_EXT "dll"
	#define SO_EXT_LEN 3
	#define SO_PREFIX ""
	#define SO_PREFIX_LEN 0
#else
	#define SO_EXT "so"
	#define SO_EXT_LEN 2
	#define SO_PREFIX "lib"
	#define SO_PREFIX_LEN 3
#endif

ModulesManager * ModulesManager::Instance = 0;

ModulesManager * ModulesManager::instance()
{
	if (0 == Instance)
	{
		Instance = new ModulesManager();
		// do not move to contructor
		// Instance variable must be available ModulesManager::load method
		Instance->ensureLoaded();
	}

	return Instance;
}

ModulesManager::ModulesManager() :
		Plugins(), Window(0)
{
	ConfigurationManager::instance()->registerStorableObject(this);

	setState(StateNotLoaded);
}

ModulesManager::~ModulesManager()
{
	ConfigurationManager::instance()->unregisterStorableObject(this);
}

void ModulesManager::load()
{
	if (!isValidStorage())
		return;

	StorableObject::load();

	QDomElement itemsNode = storage()->point();
	if (!itemsNode.isNull())
	{
		QList<QDomElement> pluginElements = storage()->storage()->getNodes(itemsNode, QLatin1String("Plugin"));

		foreach (const QDomElement &pluginElement, pluginElements)
		{
			QSharedPointer<StoragePoint> storagePoint(new StoragePoint(storage()->storage(), pluginElement));
			QString name = storagePoint->point().attribute("name");
			if (!name.isEmpty())
			{
				Plugin *plugin = new Plugin(name, this);
				Plugins.insert(name, plugin);
			}
		}
	}

	foreach (const QString &moduleName, installedPlugins())
	{
		Plugin *plugin = new Plugin(moduleName, this);
		Plugins.insert(moduleName, plugin);
	}

	foreach (Plugin *plugin, Plugins)
		plugin->ensureLoaded();

	if (!loadAttribute<bool>("imported_from_09", false))
	{
		importFrom09();
		storeAttribute("imported_from_09", true);
	}
}

void ModulesManager::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	StorableObject::store();

	foreach (Plugin *plugin, Plugins)
		plugin->store();
}

void ModulesManager::importFrom09()
{
	QStringList everLoaded = config_file.readEntry("General", "EverLoaded").split(',', QString::SkipEmptyParts);
	QString loaded_str = config_file.readEntry("General", "LoadedModules");

	QStringList loadedPlugins = loaded_str.split(',', QString::SkipEmptyParts);
	everLoaded += loadedPlugins;
	QString unloaded_str = config_file.readEntry("General", "UnloadedModules");
	QStringList unloadedPlugins = unloaded_str.split(',', QString::SkipEmptyParts);

	QStringList allPlugins = everLoaded + unloadedPlugins; // just in case...
	foreach (const QString &pluginName, allPlugins)
		if (!Plugins.contains(pluginName))
		{
			Plugin *plugin = new Plugin(pluginName, this);
			plugin->ensureLoaded();
			Plugins.insert(pluginName, plugin);
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

	ensureLoadedAtLeastOnce("gadu_protocol");
	ensureLoadedAtLeastOnce("jabber_protocol");
	ensureLoadedAtLeastOnce("sql_history");
	ensureLoadedAtLeastOnce("history_migration");
	ensureLoadedAtLeastOnce("profiles_import");

	foreach (Plugin *plugin, Plugins)
		if (loadedPlugins.contains(plugin->name()))
			plugin->setState(Plugin::PluginStateLoaded);
		else
			plugin->setState(Plugin::PluginStateNotLoaded);
}

void ModulesManager::ensureLoadedAtLeastOnce(const QString& moduleName)
{
	if (!Plugins.contains(moduleName))
		return;

	if (!Plugin::PluginStateNew == Plugins.value(moduleName)->state())
		Plugins.value(moduleName)->setState(Plugin::PluginStateLoaded);
}

void ModulesManager::activateProtocolPlugins()
{
	bool saveList = false;

	foreach (Plugin *plugin, Plugins)
	{
		if (plugin->type() != "protocol")
			continue;

		if (plugin->shouldBeActivated())
			if (!activatePlugin(plugin))
				saveList = true;
	}

	// if not all plugins were loaded properly
	// save the list of modules
	if (saveList)
		ConfigurationManager::instance()->flush();
}

void ModulesManager::activatePlugins()
{
	bool saveList = false;

	foreach (Plugin *plugin, Plugins)
		if (plugin->shouldBeActivated())
			if (!activatePlugin(plugin))
				saveList = true;

	foreach (Plugin *pluginToReplace, Plugins)
	{
		if (pluginToReplace->isActive())
			continue;

		foreach (Plugin *replacementPlugin, Plugins)
			if (replacementPlugin->isValid() && replacementPlugin->info()->replaces().contains(pluginToReplace->name()))
				if (activatePlugin(replacementPlugin))
					saveList = true; // list has changed
	}

	// if not all plugins were loaded properly or new plugin was added
	// save the list of modules
	if (saveList)
		ConfigurationManager::instance()->flush();
}

void ModulesManager::deactivatePlugins()
{
	ConfigurationManager::instance()->flush();

	foreach (Plugin *plugin, Plugins)
		if (plugin->isActive())
		{
			kdebugm(KDEBUG_INFO, "module: %s, usage: %d\n", qPrintable(plugin->name()), plugin->usageCounter());
		}

	// unloading all not used modules
	// as long as any module were unloaded

	bool deactivated;
	do
	{
		QList<Plugin *> active = activePlugins();
		deactivated = false;
		foreach (Plugin *plugin, active)
			if (plugin->usageCounter() == 0)
				if (deactivatePlugin(plugin, false, false))
					deactivated = true;
	}
	while (deactivated);

	// we cannot unload more modules in normal way
	// so we are making it brutal ;)
	QList<Plugin *> active = activePlugins();
	foreach (Plugin *plugin, active)
	{
		kdebugm(KDEBUG_PANIC, "WARNING! Could not deactivate module %s, killing\n", qPrintable(plugin->name()));
		deactivatePlugin(plugin, false, true);
	}

}

QList<Plugin *> ModulesManager::activePlugins() const
{
	QList<Plugin *> result;
	foreach (Plugin *plugin, Plugins)
		if (plugin->isActive())
			result.append(plugin);
	return result;
}

void ModulesManager::incDependenciesUsageCount(PluginInfo *pluginInfo)
{
	kdebugmf(KDEBUG_FUNCTION_START, "%s\n", qPrintable(pluginInfo->description()));
	foreach (const QString &it, pluginInfo->dependencies())
	{
		kdebugm(KDEBUG_INFO, "incUsage: %s\n", qPrintable(it));
		moduleIncUsageCount(it);
	}
	kdebugf2();
}

QStringList ModulesManager::installedPlugins() const
{
	QDir dir(dataPath("kadu/plugins"), "*.desc");
	dir.setFilter(QDir::Files);

	QStringList installed;
	QStringList entries = dir.entryList();
	foreach (const QString &entry, entries)
		installed.append(entry.left(entry.length() - 5));
	return installed;
}

QString ModulesManager::moduleProvides(const QString &provides)
{
	foreach (Plugin *plugin, Plugins)
	{
		PluginInfo *info = plugin->info();
		if (info && info->provides().contains(provides))
			if (Plugin::PluginStateLoaded == plugin->state())
				return plugin->name();
	}

	return QString();
}

bool ModulesManager::moduleIsActive(const QString& module_name) const
{
	return Plugins.contains(module_name) && (Plugins.value(module_name)->isActive());
}

QString ModulesManager::modulesUsing(const QString &module_name) const
{
	QString modules;

	foreach (Plugin *plugin, Plugins)
	{
		PluginInfo *info = plugin->info();
		if (info && info->dependencies().contains(module_name))
			modules += "\n- " + plugin->name();
	}

	return modules;
}

QString ModulesManager::findActiveConflict(Plugin *plugin) const
{
	if (!plugin || !plugin->isValid())
		return QString();

	foreach (const QString &it, plugin->info()->conflicts())
	{
		if (moduleIsActive(it))
			return it;

		foreach (Plugin *possibleConflict, Plugins)
			if (possibleConflict->isValid() && possibleConflict->isActive())
				foreach (const QString &sit, possibleConflict->info()->provides())
					if (it == sit)
						return possibleConflict->name();
	}

	foreach (Plugin *possibleConflict, Plugins)
		if (possibleConflict->isValid() && possibleConflict->isActive())
			foreach (const QString &sit, possibleConflict->info()->conflicts())
				if (sit == plugin->name())
					return plugin->name();

	return QString();
}

bool ModulesManager::activateDependencies(Plugin *plugin)
{
	if (!plugin || !plugin->isValid())
		return true; // always true

	foreach (const QString &dependencyName, plugin->info()->dependencies())
	{
		if (!Plugins.contains(dependencyName))
		{
			MessageDialog::show("dialog-warning", tr("Kadu"), tr("Required module %1 was not found").arg(dependencyName));
			return false;
		}

		if (!activatePlugin(Plugins.value(dependencyName)))
			return false;
	}

	return true;
}

bool ModulesManager::activatePlugin(Plugin *plugin)
{
	if (plugin->isActive())
		return true;

	QString conflict = findActiveConflict(plugin);
	if (!conflict.isEmpty())
	{
		MessageDialog::show("dialog-warning", tr("Kadu"), tr("Module %1 conflicts with: %2").arg(plugin->name(), conflict));
		return false;
	}

	if (!activateDependencies(plugin))
		return false;

	bool result = plugin->activate();
	if (result)
		plugin->setState(Plugin::PluginStateLoaded);

	return result;
}

bool ModulesManager::activatePlugin(const QString& pluginName)
{
	kdebugmf(KDEBUG_FUNCTION_START, "'%s'\n", qPrintable(pluginName));

	if (!Plugins.contains(pluginName))
		return false;

	return activatePlugin(Plugins.value(pluginName));
}

bool ModulesManager::deactivatePlugin(Plugin *plugin, bool setAsUnloaded, bool force)
{
	kdebugmf(KDEBUG_FUNCTION_START, "name:'%s' force:%d usage: %d\n", qPrintable(plugin->name()), force, plugin->usageCounter());

	if (plugin->usageCounter() > 0 && !force)
	{
		MessageDialog::show("dialog-warning", tr("Kadu"), tr("Module %1 cannot be deactivated because it is being used by the following modules:%2").arg(plugin->name()).arg(modulesUsing(plugin->name())));
		kdebugf2();
		return false;
	}

	foreach (const QString &i, plugin->info()->dependencies())
		moduleDecUsageCount(i);

	bool result = plugin->deactivate();
	if (result && setAsUnloaded)
		plugin->setState(Plugin::PluginStateNotLoaded);

	return result;
}

bool ModulesManager::deactivatePlugin(const QString &pluginName, bool setAsUnloaded, bool force)
{
	kdebugmf(KDEBUG_FUNCTION_START, "name:'%s' force:%d\n", qPrintable(pluginName), force);

	if (!Plugins.contains(pluginName))
		return true; // non-existing module is properly deactivated

	return deactivatePlugin(Plugins.value(pluginName), setAsUnloaded, force);
}

void ModulesManager::showWindow(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	kdebugf();

	if (!Window)
	{
		Window = new ModulesWindow();
		connect(Window, SIGNAL(destroyed()), this, SLOT(dialogDestroyed()));
		Window->show();
	}

	_activateWindow(Window);

	kdebugf2();
}

void ModulesManager::dialogDestroyed()
{
	kdebugf();
	Window = 0;
	kdebugf2();
}

void ModulesManager::moduleIncUsageCount(const QString& module_name)
{
	Plugins.value(module_name)->incUsage();
}

void ModulesManager::moduleDecUsageCount(const QString& module_name)
{
	Plugins.value(module_name)->decUsage();
}
