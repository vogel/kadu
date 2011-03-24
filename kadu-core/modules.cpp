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

#include "modules/static_modules.cpp"

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
		StaticModules(), Modules(), Window(0)
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

	QStringList everLoaded = config_file.readEntry("General", "EverLoaded").split(',', QString::SkipEmptyParts);

	foreach (const QString &moduleName, installedModules())
	{
		Plugin *plugin = new Plugin(moduleName, this);
		plugin->ensureLoaded();
		Modules.insert(moduleName, plugin);
	}

	QString loaded_str = config_file.readEntry("General", "LoadedModules");

	QStringList loadedPlugins = loaded_str.split(',', QString::SkipEmptyParts);
	everLoaded += loadedPlugins;
	QString unloaded_str = config_file.readEntry("General", "UnloadedModules");
	QStringList unloadedPlugins = unloaded_str.split(',', QString::SkipEmptyParts);

	QStringList allPlugins = everLoaded + unloadedPlugins; // just in case...
	foreach (const QString &pluginName, allPlugins)
		if (!Modules.contains(pluginName))
		{
			Plugin *plugin = new Plugin(pluginName, this);
			plugin->ensureLoaded();
			Modules.insert(pluginName, plugin);
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

	foreach (Plugin *plugin, Modules)
		if (loadedPlugins.contains(plugin->name()))
			plugin->setState(Plugin::PluginStateLoaded);
		else
			plugin->setState(Plugin::PluginStateNotLoaded);

	registerStaticModules();
}

void ModulesManager::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	StorableObject::store();

	foreach (Plugin *plugin, Modules)
		plugin->store();
}

void ModulesManager::ensureLoadedAtLeastOnce(const QString& moduleName)
{
	if (!Modules.contains(moduleName))
		return;

	if (!Plugin::PluginStateNew == Modules.value(moduleName)->state())
		Modules.value(moduleName)->setState(Plugin::PluginStateLoaded);
}

void ModulesManager::loadProtocolModules()
{
	foreach (const QString &i, staticModules())
		if (!moduleIsActive(i) && moduleIsProtocol(i))
			activateModule(i);

	foreach (Plugin *plugin, Modules)
	{
		if (!moduleIsProtocol(plugin->name()))
			continue;
		if (plugin->isActive())
			continue;

		bool load_module = false;
		if (Plugin::PluginStateLoaded == plugin->state())
			load_module = true;
		else if (Plugin::PluginStateNotLoaded == plugin->state())
			load_module = false;
		else
		{
			PluginInfo *m_info = plugin->info();
			if (m_info)
				load_module = m_info->loadByDefault();
			else
				load_module = false;
		}
		if (load_module)
			activateModule(plugin->name());
	}
}

void ModulesManager::loadAllModules()
{
	bool saveList = false;

	foreach (const QString &i, staticModules())
		if (!moduleIsActive(i))
			activateModule(i);

	foreach (Plugin *plugin, Modules)
	{
		if (moduleIsProtocol(plugin->name()))
			continue;
		if (plugin->isActive())
			continue;

		bool load_module;
		if (Plugin::PluginStateLoaded == plugin->state())
			load_module = true;
		else if (Plugin::PluginStateNotLoaded == plugin->state())
			load_module = false;
		else
		{
			PluginInfo *m_info = plugin->info();
			if (m_info)
				load_module = m_info->loadByDefault();
			else
				load_module = false;
		}

		if (load_module && !activateModule(plugin->name()))
			saveList = true;
	}

	foreach (Plugin *i, Modules)
	{
		if (!i->isActive())
		{
			foreach (Plugin *plugin, Modules)
			{
				PluginInfo *m_info = plugin->info();
				if (m_info && m_info->replaces().contains(i->name()))
					if (activateModule(i->name()))
						saveList = true;
			}
		}
	}

	// if not all modules were loaded properly
	// save the list of modules
	if (saveList)
		saveLoadedModules();
}

bool ModulesManager::satisfyModuleDependencies(PluginInfo *pluginInfo)
{
	if (!pluginInfo)
		return true;

	kdebugf();
	foreach (const QString &it, pluginInfo->dependencies())
	{
		if (!moduleIsActive(it))
		{
			if (moduleIsInstalled(it) || moduleIsStatic(it))
			{
				if (!activateModule(it))
				{
					kdebugf2();
					return false;
				}
			}
			else
			{
				MessageDialog::show("dialog-warning", tr("Kadu"), tr("Required module %1 was not found").arg(it));
				kdebugf2();
				return false;
			}
		}
	}
	kdebugf2();
	return true;
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

void ModulesManager::registerStaticModule(const QString &module_name, InitModuleFunc *init, CloseModuleFunc *close)
{
	StaticModule m;
	m.init = init;
	m.close = close;
	StaticModules.insert(module_name, m);
}

QStringList ModulesManager::staticModules() const
{
	QStringList static_modules;
	foreach (const QString &i, StaticModules.keys())
		static_modules.append(i);
	return static_modules;
}

QStringList ModulesManager::installedModules() const
{
	QDir dir(dataPath("kadu/modules"), "*.desc");
	dir.setFilter(QDir::Files);

	QStringList installed;
	QStringList entries = dir.entryList();
	foreach (const QString &entry, entries)
		installed.append(entry.left(entry.length() - 5));
	return installed;
}

QStringList ModulesManager::loadedModules() const
{
	QStringList loaded;
	for (QMap<QString, Plugin *>::const_iterator i = Modules.constBegin(); i != Modules.constEnd(); ++i)
		if (i.value()->pluginLibrary() || i.value()->pluginObject())
			loaded.append(i.key());
	return loaded;
}

QStringList ModulesManager::unloadedModules() const
{
	QStringList installed = installedModules();
	QStringList loaded = loadedModules();
	QStringList unloaded;
	foreach(const QString &module, installed)
		if (!loaded.contains(module))
			unloaded.append(module);
	return unloaded;
}

QStringList ModulesManager::activeModules() const
{
	QStringList active;
	foreach(const QString &i, Modules.keys())
		active.append(i);
	return active;
}

QString ModulesManager::moduleProvides(const QString &provides)
{
	QStringList moduleList = staticModules();
	foreach(const QString &moduleName, moduleList)
		if (Modules.contains(moduleName))
		{
			PluginInfo *info = Modules.value(moduleName)->info();
			if (info && info->provides().contains(provides))
				return moduleName;
		}

	moduleList = installedModules();
	foreach(const QString &moduleName, moduleList)
		if (Modules.contains(moduleName))
		{
			PluginInfo *info = Modules.value(moduleName)->info();
			if (info && info->provides().contains(provides))
				if (moduleIsLoaded(moduleName))
					return moduleName;
		}

	return QString();
}

bool ModulesManager::moduleIsProtocol(const QString& module_name) const
{
	return module_name.endsWith("_protocol");
}

bool ModulesManager::moduleIsStatic(const QString& module_name) const
{
	return staticModules().contains(module_name);
}

bool ModulesManager::moduleIsInstalled(const QString& module_name) const
{
	return installedModules().contains(module_name);
}

bool ModulesManager::moduleIsLoaded(const QString& module_name) const
{
	return loadedModules().contains(module_name);
}

bool ModulesManager::moduleIsActive(const QString& module_name) const
{
	return Modules.contains(module_name) && (Modules.value(module_name)->isActive());
}

void ModulesManager::saveLoadedModules()
{
	config_file.writeEntry("General", "LoadedModules", loadedModules().join(","));
	config_file.writeEntry("General", "UnloadedModules", unloadedModules().join(","));
	config_file.sync();
}

QString ModulesManager::modulesUsing(const QString &module_name) const
{
	QStringList moduleList = loadedModules();
	QString modules;

	foreach (const QString &moduleName, moduleList)
		if (Modules.contains(moduleName))
		{
			PluginInfo *info = Modules.value(moduleName)->info();
			if (info && info->dependencies().contains(module_name))
				modules += "\n- " + moduleName;
		}

	return modules;
}

bool ModulesManager::conflictsWithLoaded(const QString &module_name, PluginInfo *pluginInfo) const
{
	if (!pluginInfo)
		return false;

	kdebugf();
	foreach (const QString &it, pluginInfo->conflicts())
	{
		if (moduleIsActive(it))
		{
			MessageDialog::show("dialog-warning", tr("Kadu"), tr("Module %1 conflicts with: %2").arg(module_name, it));
			kdebugf2();
			return true;
		}
		foreach (const QString &key, Modules.keys())
			if (Modules.value(key)->info())
				foreach (const QString &sit, Modules.value(key)->info()->provides())
					if (moduleIsActive(key) && (it == sit))
					{
						MessageDialog::show("dialog-warning", tr("Kadu"), tr("Module %1 conflicts with: %2").arg(module_name, key));
						kdebugf2();
						return true;
					}
	}
	foreach (const QString &key, Modules.keys())
		if (Modules.value(key)->info())
			foreach (const QString &sit, Modules.value(key)->info()->conflicts())
				if (moduleIsActive(key) && (sit == module_name))
				{
					MessageDialog::show("dialog-warning", tr("Kadu"), tr("Module %1 conflicts with: %2").arg(module_name, key));
					kdebugf2();
					return true;
				}
	kdebugf2();
	return false;
}

bool ModulesManager::activateModule(const QString& module_name)
{
	kdebugmf(KDEBUG_FUNCTION_START, "'%s'\n", qPrintable(module_name));

	if (moduleIsActive(module_name))
	{
		MessageDialog::show("dialog-warning", tr("Kadu"), tr("Module %1 is already active").arg(module_name));
		kdebugf2();
		return false;
	}

	Plugin *plugin = Modules.value(module_name);
	if (!plugin)
		return false;

	if (plugin->isActive())
		return true;

	if (conflictsWithLoaded(module_name, plugin->info()))
		return false;

	if (!satisfyModuleDependencies(plugin->info()))
		return false;

	plugin->setState(Plugin::PluginStateLoaded);
	return plugin->activate();
}

void ModulesManager::unloadAllModules()
{
	saveLoadedModules();

	foreach (const QString &it, Modules.keys())
	{
		Q_UNUSED(it) // only in debug mode
		kdebugm(KDEBUG_INFO, "module: %s, usage: %d\n", qPrintable(it), Modules.value(it)->usageCounter());
	}

	// unloading all not used modules
	// as long as any module were unloaded

	bool deactivated;
	do
	{
		QStringList active = activeModules();
		deactivated = false;
		foreach (const QString &i, active)
			if (Modules.value(i)->usageCounter() == 0)
				if (deactivateModule(i, false, false))
					deactivated = true;
	}
	while (deactivated);

	// we cannot unload more modules in normal way
	// so we are making it brutal ;)
	QStringList active = activeModules();
	foreach (const QString &i, active)
	{
		kdebugm(KDEBUG_PANIC, "WARNING! Could not deactivate module %s, killing\n",qPrintable(i));
		deactivateModule(i, false, true);
	}

}

bool ModulesManager::deactivateModule(const QString& module_name, bool setAsUnloaded, bool force)
{
	Plugin *plugin = Modules.value(module_name);
	kdebugmf(KDEBUG_FUNCTION_START, "name:'%s' force:%d usage:%d\n", qPrintable(module_name), force, plugin->usageCounter());

	if (plugin->usageCounter() > 0 && !force)
	{
		MessageDialog::show("dialog-warning", tr("Kadu"), tr("Module %1 cannot be deactivated because it is being used by the following modules:%2").arg(module_name).arg(modulesUsing(module_name)));
		kdebugf2();
		return false;
	}

	foreach (const QString &i, plugin->info()->dependencies())
		moduleDecUsageCount(i);

	if (setAsUnloaded)
		plugin->setState(Plugin::PluginStateNotLoaded);
	return plugin->deactivate();
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
	Modules.value(module_name)->incUsage();
}

void ModulesManager::moduleDecUsageCount(const QString& module_name)
{
	Modules.value(module_name)->decUsage();
}
