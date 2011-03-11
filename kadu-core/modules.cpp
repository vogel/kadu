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
#include <QtCore/QTextCodec>
#include <QtCore/QTranslator>
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
#include "core/core.h"
#include "gui/hot-key.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/modules-window.h"
#include "gui/windows/message-dialog.h"

#include "activate.h"
#include "debug.h"
#include "icons-manager.h"
#include "misc/path-conversion.h"

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

#ifndef Q_WS_WIN
Library::Library(const QString& file_name) : FileName(file_name), Handle(0)
{
}

Library::~Library()
{
	kdebugf();
	if (Handle != 0)
		dlclose(Handle);
	kdebugf2();
}

bool Library::load()
{
	Handle = dlopen(qPrintable(FileName), RTLD_NOW | RTLD_GLOBAL);
	return (Handle != 0);
}

void* Library::resolve(const QString& symbol_name)
{
	if (Handle == 0)
		return 0;
	return dlsym(Handle, qPrintable(symbol_name));
}

QString Library::errorString()
{
	return QString(dlerror());
}
#endif

ModuleInfo::ModuleInfo() : depends(), conflicts(), provides(),
	description(), author(), version(), load_by_def(false)
{
}

ModulesManager::Module::Module() : lib(0), close(0), translator(0), info(), usage_counter(0)
{
}

ModulesManager * ModulesManager::Instance = 0;

ModulesManager * ModulesManager::instance()
{
	if (0 == Instance)
		Instance = new ModulesManager();

	return Instance;
}

ModulesManager::ModulesManager() : QObject(),
	StaticModules(), Modules(), Window(0), translators(new QObject(this))
{
	kdebugf();

	everLoaded = config_file.readEntry("General", "EverLoaded").split(',', QString::SkipEmptyParts);

	// load modules as config file say
	installed_list = installedModules();
	QString loaded_str = config_file.readEntry("General", "LoadedModules");
	loaded_list = loaded_str.split(',', QString::SkipEmptyParts);
	QString unloaded_str = config_file.readEntry("General", "UnloadedModules");
	unloaded_list = unloaded_str.split(',', QString::SkipEmptyParts);

	if (loaded_list.contains("encryption"))
	{
		loaded_list.removeAll("encryption");
		loaded_list.append("encryption_ng");
		loaded_list.append("encryption_ng_simlite");
	}
	if (loaded_list.contains("osd_hints"))
	{
		loaded_list.removeAll("osd_hints");
		if (!loaded_list.contains("hints"))
			loaded_list.append("hints");
	}

	ensureLoadedAtLeastOnce("gadu_protocol");
	ensureLoadedAtLeastOnce("jabber_protocol");
	ensureLoadedAtLeastOnce("sql_history");
	ensureLoadedAtLeastOnce("history_migration");
	ensureLoadedAtLeastOnce("profiles_import");

	registerStaticModules();

	foreach (const QString &i, staticModules())
		if (i.right(9) == "_protocol")
			    protocolModulesList.append(i);
	foreach (const QString &i, installed_list)
		if (i.right(9) == "_protocol")
			    protocolModulesList.append(i);

	kdebugf2();
}

ModulesManager::~ModulesManager()
{
	kdebugf();
	kdebugf2();
}

void ModulesManager::ensureLoadedAtLeastOnce(const QString& moduleName)
{
	if (!everLoaded.contains(moduleName) && !loaded_list.contains(moduleName) && unloaded_list.contains(moduleName))
	{
		loaded_list.append(moduleName);
		unloaded_list.removeAll(moduleName);
	}
}

void ModulesManager::loadProtocolModules()
{
	foreach (const QString &i, protocolModulesList)
	{
		if (!moduleIsActive(i))
		{
			bool load_module;
			if (loaded_list.contains(i))
				load_module = true;
			else if (unloaded_list.contains(i))
				load_module = false;
			else if (staticModules().contains(i))
				load_module = true;
			else
			{
				ModuleInfo m_info;
				if (moduleInfo(i, m_info))
					load_module = m_info.load_by_def;
				else
					load_module = false;
			}
			if (load_module)
				activateModule(i);
		}
	}
}

void ModulesManager::loadAllModules()
{
	bool saveList = false;

	foreach (const QString &i, staticModules())
		if (!moduleIsActive(i))
			activateModule(i);

	foreach (const QString &i, installed_list)
	{
		if (!moduleIsActive(i) && !protocolModulesList.contains(i))
		{
			bool load_module;
			if (loaded_list.contains(i))
				load_module = true;
			else if (unloaded_list.contains(i))
				load_module = false;
			else
			{
				ModuleInfo m_info;
				if (moduleInfo(i, m_info))
					load_module = m_info.load_by_def;
				else
					load_module = false;
			}

			if (load_module && !activateModule(i))
				saveList = true;
		}
	}

	foreach (const QString &i, loaded_list)
	{
		if (!moduleIsActive(i))
		{
			foreach (const QString &module, installed_list)
			{
				ModuleInfo m_info;
				if (moduleInfo(module, m_info) && m_info.replaces.contains(i))
					if (activateModule(i))
						saveList = true;
			}
		}
	}

	// if not all modules were loaded properly
	// save the list of modules
	if (saveList)
		saveLoadedModules();

}

QTranslator* ModulesManager::loadModuleTranslation(const QString &module_name)
{
	QTranslator* translator = new QTranslator(translators);
	const QString lang = config_file.readEntry("General", "Language");
	if (translator->load(module_name + '_' + lang, dataPath("kadu/modules/translations/")))
	{
		qApp->installTranslator(translator);
		return translator;
	}
	else
	{
		delete translator;
		return 0;
	}
}

bool ModulesManager::satisfyModuleDependencies(const ModuleInfo &module_info)
{
	kdebugf();
	foreach (const QString &it, module_info.depends)
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

void ModulesManager::incDependenciesUsageCount(const ModuleInfo &module_info)
{
	kdebugmf(KDEBUG_FUNCTION_START, "%s\n", qPrintable(module_info.description));
	foreach (const QString &it, module_info.depends)
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
	QDir dir(libPath("kadu/modules"), SO_PREFIX "*." SO_EXT);
	dir.setFilter(QDir::Files);
	QStringList installed;
	QStringList entries = dir.entryList();
	foreach (const QString &entry, entries)
		installed.append(entry.mid(SO_PREFIX_LEN, entry.length() - SO_EXT_LEN - SO_PREFIX_LEN - 1));
	return installed;
}

QStringList ModulesManager::loadedModules() const
{
	QStringList loaded;
	for (QMap<QString, Module>::const_iterator i = Modules.constBegin(); i != Modules.constEnd(); ++i)
		if (i.value().lib)
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
	ModuleInfo info;

	QStringList moduleList = staticModules();
	foreach(const QString &moduleName, moduleList)
		if (moduleInfo(moduleName, info))
			if (info.provides.contains(provides))
				return moduleName;

	moduleList = installedModules();
	foreach(const QString &moduleName, moduleList)
		if (moduleInfo(moduleName, info) && info.provides.contains(provides))
			if (moduleIsLoaded(moduleName))
				return moduleName;

	return QString();
}

bool ModulesManager::moduleInfo(const QString& module_name, ModuleInfo& info) const
{
	if (Modules.contains(module_name))
	{
		info = Modules[module_name].info;
		return true;
	}

	PlainConfigFile desc_file(dataPath("kadu/modules/" + module_name + ".desc"));

	const QString lang = config_file.readEntry("General", "Language");

	info.description = desc_file.readEntry("Module", "Description[" + lang + ']');
	if(info.description.isEmpty())
		info.description = desc_file.readEntry("Module", "Description");

	info.author = desc_file.readEntry("Module", "Author");

	if (desc_file.readEntry("Module", "Version") == "core")
		info.version = Core::version();
	else
		info.version = desc_file.readEntry("Module", "Version");

	info.depends = desc_file.readEntry("Module", "Dependencies").split(' ', QString::SkipEmptyParts);
	info.conflicts = desc_file.readEntry("Module", "Conflicts").split(' ', QString::SkipEmptyParts);
	info.provides = desc_file.readEntry("Module", "Provides").split(' ', QString::SkipEmptyParts);
	info.replaces = desc_file.readEntry("Module", "Replaces").split(' ', QString::SkipEmptyParts);

	info.load_by_def = desc_file.readBoolEntry("Module", "LoadByDefault");
	info.base = desc_file.readBoolEntry("Module", "Base");

	return true;
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
	return Modules.contains(module_name);
}

void ModulesManager::saveLoadedModules()
{
	config_file.writeEntry("General", "LoadedModules", loadedModules().join(","));
	config_file.writeEntry("General", "UnloadedModules", unloadedModules().join(","));
	config_file.sync();
}

QString ModulesManager::modulesUsing(const QString &module_name) const
{
	ModuleInfo info;
	QStringList moduleList = loadedModules();
	QString modules;

	foreach (const QString &moduleName, moduleList)
		if (moduleInfo(moduleName, info))
			if (info.depends.contains(module_name))
				modules += "\n- " + moduleName;

	return modules;
}

bool ModulesManager::conflictsWithLoaded(const QString &module_name, const ModuleInfo& module_info) const
{
	kdebugf();
	foreach (const QString &it, module_info.conflicts)
	{
		if (moduleIsActive(it))
		{
			MessageDialog::show("dialog-warning", tr("Kadu"), tr("Module %1 conflicts with: %2").arg(module_name, it));
			kdebugf2();
			return true;
		}
		foreach (const QString &key, Modules.keys())
			foreach (const QString &sit, Modules[key].info.provides)
				if (it == sit)
				{
					MessageDialog::show("dialog-warning", tr("Kadu"), tr("Module %1 conflicts with: %2").arg(module_name, key));
					kdebugf2();
					return true;
				}
	}
	foreach (const QString &key, Modules.keys())
		foreach (const QString &sit, Modules[key].info.conflicts)
			if (sit == module_name)
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
	Module m;
	kdebugmf(KDEBUG_FUNCTION_START, "'%s'\n", qPrintable(module_name));

	if (moduleIsActive(module_name))
	{
		MessageDialog::show("dialog-warning", tr("Kadu"), tr("Module %1 is already active").arg(module_name));
		kdebugf2();
		return false;
	}

	if (moduleInfo(module_name,m.info))
	{
		if (conflictsWithLoaded(module_name, m.info))
		{
			kdebugf2();
			return false;
		}
		if (!satisfyModuleDependencies(m.info))
		{
			kdebugf2();
			return false;
		}
	}
	else
	{
		kdebugf2();
		return false;
	}

	InitModuleFunc *init;

	if (moduleIsStatic(module_name))
	{
		m.lib = 0;
		StaticModule sm = StaticModules[module_name];
		init = sm.init;
		m.close = sm.close;
	}
	else
	{
		m.lib = new Library(libPath("kadu/modules/"SO_PREFIX + module_name + "." SO_EXT));
		if (!m.lib->load())
		{
			QString err = m.lib->errorString();
			MessageDialog::show("dialog-warning", tr("Kadu"), tr("Cannot load %1 module library.:\n%2").arg(module_name, err));
			kdebugm(KDEBUG_ERROR, "cannot load %s because of: %s\n", qPrintable(module_name), qPrintable(err));
			delete m.lib;
			m.lib = 0;
			m.close = 0;
			kdebugf2();
			return false;
		}
		init = (InitModuleFunc *)m.lib->resolve(qPrintable(module_name + "_init"));
		m.close = (CloseModuleFunc *)m.lib->resolve(qPrintable(module_name + "_close"));
		if (!init || !m.close)
		{
			MessageDialog::show("dialog-warning", tr("Kadu"), tr("Cannot find required functions in module %1.\n"
					"Maybe it's not Kadu-compatible Module.").arg(module_name));
			delete m.lib;
			m.lib = 0;
			m.close = 0;
			kdebugf2();
			return false;
		}
	}

	m.translator = loadModuleTranslation(module_name);

	int res = init(!everLoaded.contains(module_name));
	if (!everLoaded.contains(module_name))
	{
		everLoaded.append(module_name);
		config_file.writeEntry("General", "EverLoaded", everLoaded.join(","));
	}

	if (res != 0)
	{
		MessageDialog::show("dialog-warning", tr("Kadu"), tr("Module initialization routine for %1 failed.").arg(module_name));
		if (m.lib)
		{
			delete m.lib;
			m.lib = 0;
		}
		if (m.translator)
		{
			qApp->removeTranslator(m.translator);
			delete m.translator;
			m.translator = 0;
		}
		return false;
	}

	incDependenciesUsageCount(m.info);

	m.usage_counter = 0;
	Modules.insert(module_name,m);
	kdebugf2();
	return true;
}

void ModulesManager::unloadAllModules()
{
	saveLoadedModules();

	foreach (const QString &it, Modules.keys())
	{
		Q_UNUSED(it) // only in debug mode
		kdebugm(KDEBUG_INFO, "module: %s, usage: %d\n", qPrintable(it), Modules[it].usage_counter);
	}

	// unloading all not used modules
	// as long as any module were unloaded

	bool deactivated;
	do
	{
		QStringList active = activeModules();
		deactivated = false;
		foreach (const QString &i, active)
			if (Modules[i].usage_counter == 0)
				if (deactivateModule(i))
					deactivated = true;
	}
	while (deactivated);

	// we cannot unload more modules in normal way
	// so we are making it brutal ;)
	QStringList active = activeModules();
	foreach (const QString &i, active)
	{
		kdebugm(KDEBUG_PANIC, "WARNING! Could not deactivate module %s, killing\n",qPrintable(i));
		deactivateModule(i, true);
	}

}

bool ModulesManager::deactivateModule(const QString& module_name, bool force)
{
	Module m = Modules[module_name];
	kdebugmf(KDEBUG_FUNCTION_START, "name:'%s' force:%d usage:%d\n", qPrintable(module_name), force, m.usage_counter);

	if (m.usage_counter > 0 && !force)
	{
		MessageDialog::show("dialog-warning", tr("Kadu"), tr("Module %1 cannot be deactivated because it is being used by the following modules:%2").arg(module_name).arg(modulesUsing(module_name)));
		kdebugf2();
		return false;
	}

	foreach (const QString &i, m.info.depends)
		moduleDecUsageCount(i);

	m.close();
	if (m.translator)
	{
		qApp->removeTranslator(m.translator);
		delete m.translator;
		m.translator = 0;
	}

	if (m.lib)
	{
		m.lib->deleteLater();
		m.lib = 0;
	}

	Modules.remove(module_name);

	kdebugf2();
	return true;
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
	++Modules[module_name].usage_counter;
}

void ModulesManager::moduleDecUsageCount(const QString& module_name)
{
	--Modules[module_name].usage_counter;
}
