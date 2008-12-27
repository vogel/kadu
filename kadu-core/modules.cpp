/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QTextCodec>
#include <QtCore/QTranslator>
#include <QtCore/QLibrary>
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

#include "config_file.h"
#include "debug.h"
#include "hot_key.h"
#include "icons_manager.h"
#include "kadu.h"
#include "message_box.h"
#include "misc.h"

#include "modules.h"

#include "modules/static_modules.cpp"

#ifdef Q_OS_MACX
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

ModulesDialog::ModulesDialog(QWidget *parent)
	: QWidget(parent, Qt::Window),
	lv_modules(0), l_moduleinfo(0)
{
	kdebugf();
	setWindowTitle(tr("Manage Modules"));
	setAttribute(Qt::WA_DeleteOnClose);

//	layout()->setResizeMode(QLayout::Minimum);

	// create main QLabel widgets (icon and app info)
	QWidget *left = new QWidget(this);
	QVBoxLayout *leftLayout = new QVBoxLayout(left);
	leftLayout->setMargin(10);
	leftLayout->setSpacing(10);

	QLabel *l_icon = new QLabel(left);

	leftLayout->addWidget(l_icon);
	leftLayout->addStretch();

	QWidget *center = new QWidget(this);
	QVBoxLayout *centerLayout = new QVBoxLayout(center);
	centerLayout->setMargin(10);
	centerLayout->setSpacing(10);

	QLabel *l_info = new QLabel(center);
	l_icon->setPixmap(icons_manager->loadPixmap("ManageModulesWindowIcon"));
	l_info->setText(tr("This dialog box allows you to manage installed modules. Modules are responsible "
			"for numerous vital features like playing sounds or message encryption.\n"
			"You can load (or unload) them by double-clicking on their names."));
	l_info->setWordWrap(true);
#ifndef	Q_OS_MAC
	l_info->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
#endif
	// end create main QLabel widgets (icon and app info)

	// our QListView
	lv_modules = new QTreeWidget(center);
	QStringList headers;
	headers << tr("Module name") << tr("Version") << tr("Module type") << tr("State");
	lv_modules->setHeaderLabels(headers);
	lv_modules->setSortingEnabled(true);
	lv_modules->setAllColumnsShowFocus(true);
	lv_modules->setIndentation(false);
	lv_modules->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));

	
	// end our QListView

	//our QVGroupBox
	QGroupBox *vgb_info = new QGroupBox(center);
	QVBoxLayout *infoLayout = new QVBoxLayout(vgb_info);
	vgb_info->setTitle(tr("Info"));
	//end our QGroupBox

	l_moduleinfo = new QLabel(vgb_info);
	l_moduleinfo->setText(tr("<b>Module:</b><br/><b>Depends on:</b><br/><b>Conflicts with:</b><br/><b>Provides:</b><br/><b>Author:</b><br/><b>Version:</b><br/><b>Description:</b>"));
#ifndef	Q_OS_MAC
	l_moduleinfo->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
#endif
	l_moduleinfo->setWordWrap(true);

	infoLayout->addWidget(l_moduleinfo);

	// buttons
	QWidget *bottom = new QWidget(center);
	QHBoxLayout *bottomLayout = new QHBoxLayout(bottom);
	bottomLayout->setSpacing(5);

	hideBaseModules = new QCheckBox(tr("Hide base modules"), bottom);
	hideBaseModules->setChecked(config_file.readBoolEntry("General", "HideBaseModules"));
	connect(hideBaseModules, SIGNAL(clicked()), this, SLOT(refreshList()));
	QPushButton *pb_close = new QPushButton(icons_manager->loadIcon("CloseWindow"), tr("&Close"), bottom, "close");

	bottomLayout->addWidget(hideBaseModules);
	bottomLayout->addStretch();
	bottomLayout->addWidget(pb_close);
#ifdef Q_OS_MAC
	bottom->setMaximumHeight(pb_close->height() + 5);
#endif
	// end buttons

	centerLayout->addWidget(l_info);
	centerLayout->addWidget(lv_modules);
	centerLayout->addWidget(vgb_info);
	centerLayout->addWidget(bottom);

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(left);
	layout->addWidget(center);

	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));
	connect(lv_modules, SIGNAL(itemSelectionChanged()), this, SLOT(itemsChanging()));
	connect(lv_modules, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(moduleAction(QTreeWidgetItem *)));

	loadWindowGeometry(this, "General", "ModulesDialogGeometry", 0, 50, 600, 620);
	refreshList();
	lv_modules->sortByColumn(0, Qt::AscendingOrder);
	kdebugf2();
}

ModulesDialog::~ModulesDialog()
{
	kdebugf();
	config_file.writeEntry("General", "HideBaseModules", hideBaseModules->isChecked());
 	saveWindowGeometry(this, "General", "ModulesDialogGeometry");
	kdebugf2();
}

QTreeWidgetItem * ModulesDialog::getSelected()
{
	if (lv_modules->selectedItems().count())
		return lv_modules->selectedItems()[0];
	else
		return 0;
}

void ModulesDialog::itemsChanging()
{
	if (lv_modules->selectedItems().count())
		getInfo();
}

void ModulesDialog::moduleAction(QTreeWidgetItem *)
{
	kdebugf();

	QTreeWidgetItem *selectedItem = getSelected();
	if (!selectedItem)
		return;

	// TODO: OH LOL
	if ((selectedItem->text(2) == tr("Dynamic")) && (selectedItem->text(3) == tr("Loaded")))
		unloadItem(selectedItem->text(0));
	else
		if ((selectedItem->text(2) == tr("Dynamic")) && (selectedItem->text(3) == tr("Not loaded")))
			loadItem(selectedItem->text(0));

	kdebugf2();
}

void ModulesDialog::loadItem(const QString &item)
{
	kdebugf();
	modules_manager->activateModule(item);
	refreshList();
	modules_manager->saveLoadedModules();
	kdebugf2();
}

void ModulesDialog::unloadItem(const QString &item)
{
	kdebugf();
	modules_manager->deactivateModule(item);
	refreshList();
	modules_manager->saveLoadedModules();
	kdebugf2();
}

void ModulesDialog::refreshList()
{
	kdebugf();

	int vScrollValue = lv_modules->verticalScrollBar()->value();

	QString s_selected;

	QTreeWidgetItem *selectedItem = getSelected();
	if (selectedItem)
		s_selected = selectedItem->text(0);

	lv_modules->clear();

	QStringList moduleList = modules_manager->staticModules();
	ModuleInfo info;
	bool hideBase = hideBaseModules->isChecked();
	foreach(const QString &module, moduleList)
	{
		QStringList strings;

		if (modules_manager->moduleInfo(module, info))
		{
			if (info.base && hideBase)
				continue;

			strings << module << info.version << tr("Static") << tr("Loaded");
		}
		else
			strings << module << QString::null << tr("Static") << tr("Loaded");
	
		new QTreeWidgetItem(lv_modules, strings);
	}

	moduleList = modules_manager->loadedModules();
	foreach(const QString &module, moduleList)
	{
		QStringList strings;

		if (modules_manager->moduleInfo(module, info))
		{
			if (info.base && hideBase)
				continue;

			strings << module << info.version << tr("Dynamic") << tr("Loaded");
		}
		else
			strings << module << QString::null << tr("Dynamic") << tr("Loaded");
	
		new QTreeWidgetItem(lv_modules, strings);
	}

	moduleList = modules_manager->unloadedModules();
	foreach(const QString &module, moduleList)
	{
		QStringList strings;

		if (modules_manager->moduleInfo(module, info))
		{
			if (info.base && hideBase)
				continue;

			strings << module << info.version << tr("Dynamic") << tr("Not loaded");
		}
		else
			strings << module << QString::null << tr("Dynamic") << tr("Not loaded");
	
		new QTreeWidgetItem(lv_modules, strings);
	}
	lv_modules->resizeColumnToContents(0);
// 	lv_modules->setSelected(lv_modules->findItem(s_selected, 0), true);

	lv_modules->verticalScrollBar()->setValue(vScrollValue);
	kdebugf2();
}

void ModulesDialog::getInfo()
{
	kdebugf();
	ModuleInfo info;

	QTreeWidgetItem *selected = getSelected();
	if (!selected)
		return;

	if (!modules_manager->moduleInfo(selected->text(0), info))
	{
		kdebugf2();
		return;
	}

	l_moduleinfo->setText(
		tr("<b>Module: </b>%1"
			"<br/><b>Depends on: </b>%2"
			"<br/><b>Conflicts with: </b>%3"
			"<br/><b>Provides: </b>%4"
			"<br/><b>Author: </b>%5"
			"<br/><b>Version: </b>%6"
			"<br/><b>Description: </b>%7")
			.arg(selected->text(0))
			.arg(info.depends.join(", "))
			.arg(info.conflicts.join(", "))
			.arg(info.provides.join(", "))
			.arg(info.author)
			.arg(info.version)
			.arg(info.description));
	kdebugf2();
}

void ModulesDialog::keyPressEvent(QKeyEvent *ke_event)
{
	if (ke_event->key() == Qt::Key_Escape)
		close();
}

ModulesManager::Module::Module() : lib(0), close(0), translator(0), info(), usage_counter(0)
{
}

void ModulesManager::initModule()
{
	new ModulesManager();
}

void ModulesManager::closeModule()
{
	delete modules_manager;
}

ModulesManager::ModulesManager() : QObject(NULL, "modules_manager"),
	StaticModules(), Modules(), Dialog(0), translators(new QObject(this, "translators"))
{
	kdebugf();
	// it's important to initiate that variable before loading modules
	// because some of them is using that value - that's why
	// i moved it here from ModulesManagert::initModule
	// the same is true for menu - modules should load up at end
	modules_manager = this;

	ActionDescription *manageModulesActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "manageModulesAction",
		this, SLOT(showDialog(QAction *, bool)),
		"ManageModules", tr("&Modules")
	);
	manageModulesActionDescription->setShortcut("kadu_modulesmanager", Qt::ApplicationShortcut);
	kadu->insertMenuActionDescription(manageModulesActionDescription, Kadu::MenuKadu, 2);

	everLoaded = QStringList::split(',', config_file.readEntry("General", "EverLoaded"));

	// load modules as config file say
	QStringList installed_list = installedModules();
	QString loaded_str = config_file.readEntry("General", "LoadedModules");
	QStringList loaded_list = QStringList::split(',', loaded_str);
	QString unloaded_str = config_file.readEntry("General", "UnloadedModules");
	QStringList unloaded_list = QStringList::split(',', unloaded_str);
	bool load_error = false;

	registerStaticModules();
	QStringList static_list = staticModules();
	foreach(const QString &i, static_list)
		if (!moduleIsActive(i))
			activateModule(i);

	foreach(const QString &i, installed_list)
	{
		if (!moduleIsActive(i))
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
			if (load_module)
				if (!activateModule(i))
					load_error = true;
		}
	}

	// if not all modules were loaded properly
	// save the list of modules
	if (load_error)
		saveLoadedModules();

	foreach(const QString &it, Modules.keys())
		kdebugm(KDEBUG_INFO, "module: %s, usage: %d\n", qPrintable(it), Modules[it].usage_counter);

	kdebugf2();
}

ModulesManager::~ModulesManager()
{
	kdebugf();

	saveLoadedModules();

	foreach(const QString &it, Modules.keys())
		kdebugm(KDEBUG_INFO, "module: %s, usage: %d\n", qPrintable(it), Modules[it].usage_counter);

	// unloading all not used modules
	// as long as any module were unloaded

	bool deactivated;
	do
	{
		QStringList active = activeModules();
		deactivated = false;
		foreach(const QString &i, active)
			if (Modules[i].usage_counter == 0)
				if (deactivateModule(i))
					deactivated = true;
	}
	while (deactivated);

	// we cannot unload more modules in normal way
	// so we are making it brutal ;)
	QStringList active = activeModules();
	foreach(const QString &i, active)
	{
		kdebugm(KDEBUG_PANIC, "WARNING! Could not deactivate module %s, killing\n",qPrintable(i));
		deactivateModule(i, true);
	}

	delete translators;
	translators = NULL;

	kdebugf2();
}

QTranslator* ModulesManager::loadModuleTranslation(const QString& module_name)
{
	QTranslator* translator = new QTranslator(translators, "module_translator");
	if (translator->load(dataPath("kadu/modules/translations/" + module_name + '_' + config_file.readEntry("General", "Language", QString(QTextCodec::locale()).mid(0,2))), "."))
	{
		qApp->installTranslator(translator);
		return translator;
	}
	else
	{
		delete translator;
		return NULL;
	}
}

bool ModulesManager::satisfyModuleDependencies(const ModuleInfo& module_info)
{
	kdebugf();
	foreach(const QString &it, module_info.depends)
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
				MessageBox::msg(tr("Required module %1 was not found").arg(it));
				kdebugf2();
				return false;
			}
		}
	}
	kdebugf2();
	return true;
}

void ModulesManager::incDependenciesUsageCount(const ModuleInfo& module_info)
{
	kdebugmf(KDEBUG_FUNCTION_START, "%s\n", qPrintable(module_info.description));
	foreach(const QString &it, module_info.depends)
	{
		kdebugm(KDEBUG_INFO, "incUsage: %s\n", qPrintable(it));
		moduleIncUsageCount(it);
	}
	kdebugf2();
}

void ModulesManager::registerStaticModule(const QString& module_name,
	InitModuleFunc* init,CloseModuleFunc* close)
{
	StaticModule m;
	m.init=init;
	m.close=close;
	StaticModules.insert(module_name,m);
}

QStringList ModulesManager::staticModules() const
{
	QStringList static_modules;
	foreach(const QString &i, StaticModules.keys())
		static_modules.append(i);
	return static_modules;
}

QStringList ModulesManager::installedModules() const
{
	QDir dir(libPath("kadu/modules"),SO_PREFIX"*." SO_EXT);
	dir.setFilter(QDir::Files);
	QStringList installed;
	QStringList entries = dir.entryList();
	foreach(const QString &entry, entries)
		installed.append(entry.mid(SO_PREFIX_LEN, entry.length() - SO_EXT_LEN - SO_PREFIX_LEN - 1));
	return installed;
}

QStringList ModulesManager::loadedModules() const
{
	QStringList loaded;
	foreach(const QString &i, Modules.keys())
		if (Modules[i].lib!=NULL)
			loaded.append(i);
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

	return "";
}

bool ModulesManager::moduleInfo(const QString& module_name, ModuleInfo& info) const
{
	if (Modules.contains(module_name))
	{
		info=Modules[module_name].info;
		return true;
	}

	PlainConfigFile desc_file(dataPath("kadu/modules/" + module_name + ".desc"));

	QString lang = config_file.readEntry("General", "Language", QString(QTextCodec::locale()).mid(0,2));

	info.description = desc_file.readEntry("Module", "Description[" + lang + ']');
	if(info.description.isEmpty())
		info.description = desc_file.readEntry("Module", "Description");

	info.author = desc_file.readEntry("Module", "Author");

	if (desc_file.readEntry("Module", "Version") == "core")
		info.version = VERSION;
	else
		info.version = desc_file.readEntry("Module", "Version");

	info.depends = QStringList::split(" ",
		desc_file.readEntry("Module", "Dependencies"));

	info.conflicts=QStringList::split(" ",
		desc_file.readEntry("Module", "Conflicts"));

	info.provides=QStringList::split(" ",
		desc_file.readEntry("Module", "Provides"));

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

bool ModulesManager::conflictsWithLoaded(const QString &module_name, const ModuleInfo& module_info) const
{
	kdebugf();
	foreach(const QString &it, module_info.conflicts)
	{
		if (moduleIsActive(it))
		{
			MessageBox::msg(narg(tr("Module %1 conflicts with: %2"), module_name, it));
			kdebugf2();
			return true;
		}
		foreach(const QString &key, Modules.keys())
			foreach(const QString &sit, Modules[key].info.provides)
				if (it == sit)
				{
					MessageBox::msg(narg(tr("Module %1 conflicts with: %2"), module_name, key));
					kdebugf2();
					return true;
				}
	}
	foreach(const QString &key, Modules.keys())
		foreach(const QString &sit, Modules[key].info.conflicts)
			if (sit == module_name)
			{
				MessageBox::msg(narg(tr("Module %1 conflicts with: %2"), module_name, key));
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
		MessageBox::msg(tr("Module %1 is already active").arg(module_name));
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
		m.lib=NULL;
		StaticModule sm=StaticModules[module_name];
		init=sm.init;
		m.close=sm.close;
	}
	else
	{
		m.lib = new Library(libPath("kadu/modules/"SO_PREFIX + module_name + "." SO_EXT));
		if (!m.lib->load())
		{
			QString err = m.lib->errorString();
			MessageBox::msg(narg(tr("Cannot load %1 module library.:\n%2"), module_name, err));
			kdebugm(KDEBUG_ERROR, "cannot load %s because of: %s\n", qPrintable(module_name), qPrintable(err));
			delete m.lib;
			kdebugf2();
			return false;
		}
		init = (InitModuleFunc *)m.lib->resolve(module_name+"_init");
		m.close = (CloseModuleFunc *)m.lib->resolve(module_name+"_close");
		if (init==NULL||m.close==NULL)
		{
			MessageBox::msg(tr("Cannot find required functions in module %1.\nMaybe it's not Kadu-compatible Module.").arg(module_name));
			delete m.lib;
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
		MessageBox::msg(tr("Module initialization routine for %1 failed.").arg(module_name));
		if (m.lib != NULL)
			delete m.lib;
		if (m.translator != NULL)
		{
			qApp->removeTranslator(m.translator);
			delete m.translator;
		}
		return false;
	}

	incDependenciesUsageCount(m.info);

	m.usage_counter=0;
	Modules.insert(module_name,m);
	kdebugf2();
	return true;
}

bool ModulesManager::deactivateModule(const QString& module_name, bool force)
{
	Module m=Modules[module_name];
	kdebugmf(KDEBUG_FUNCTION_START, "name:'%s' force:%d usage:%d\n", qPrintable(module_name), force, m.usage_counter);

	if (m.usage_counter>0 && !force)
	{
		MessageBox::msg(tr("Module %1 cannot be deactivated because it is used now").arg(module_name));
		kdebugf2();
		return false;
	}

	foreach(const QString &i, m.info.depends)
		moduleDecUsageCount(i);

	m.close();
	if (m.translator)
	{
		qApp->removeTranslator(m.translator);
		delete m.translator;
	}

	if (m.lib)
		m.lib->deleteLater();

	Modules.remove(module_name);

	kdebugf2();
	return true;
}

void ModulesManager::showDialog(QAction *sender, bool toggled)
{
	kdebugf();

	if (!Dialog)
	{
		Dialog = new ModulesDialog();
		connect(Dialog, SIGNAL(destroyed()), this, SLOT(dialogDestroyed()));
		Dialog->show();
	}

	Dialog->raise();
	Dialog->setActiveWindow();

	kdebugf2();
}

void ModulesManager::dialogDestroyed()
{
	kdebugf();
	Dialog=NULL;
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

ModulesManager* modules_manager;
