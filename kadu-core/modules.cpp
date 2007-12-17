/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qtextcodec.h>
#include <qtranslator.h>
#include <qvbox.h>
#include <qvgroupbox.h>

#include <dlfcn.h>

// #include "config_dialog.h"
#include "config_file.h"
#include "debug.h"
#include "hot_key.h"
#include "icons_manager.h"
#include "kadu.h"
#include "message_box.h"
#include "misc.h"
#include "modules.h"
#include "modules_static.cpp"

#ifdef Q_OS_MACX
	#define SO_EXT "dylib"
	#define SO_EXT_LEN 5
#else
	#define SO_EXT "so"
	#define SO_EXT_LEN 2
#endif

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
	Handle = dlopen(FileName.local8Bit().data(), RTLD_NOW | RTLD_GLOBAL);
	return (Handle != 0);
}

void* Library::resolve(const QString& symbol_name)
{
	if (Handle == 0)
		return 0;
	return dlsym(Handle, symbol_name.local8Bit().data());
}

QString Library::error()
{
	return QString(dlerror());
}

ModuleInfo::ModuleInfo() : depends(), conflicts(), provides(),
	description(), author(), version(), load_by_def(false)
{
}

void ModulesDialog::resizeEvent(QResizeEvent * /*e*/)
{
	// TODO: WTF
	layoutHelper->resizeLabels();
}

ModulesDialog::ModulesDialog() : QHBox(kadu, "modules_dialog", WType_TopLevel | WDestructiveClose),
	lv_modules(0), l_moduleinfo(0), layoutHelper(new LayoutHelper())
{
	kdebugf();
	setCaption(tr("Manage Modules"));
	layout()->setResizeMode(QLayout::Minimum);

	// create main QLabel widgets (icon and app info)
	QVBox *left=new QVBox(this);
	left->setMargin(10);
	left->setSpacing(10);

	QLabel *l_icon = new QLabel(left);
	QWidget *blank=new QWidget(left);
	blank->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));

	QVBox *center=new QVBox(this);
	center->setMargin(10);
	center->setSpacing(10);

	QLabel *l_info = new QLabel(center);
	l_icon->setPixmap(icons_manager->loadIcon("ManageModulesWindowIcon"));
	l_info->setText(tr("This dialog box allows you to manage installed modules. Modules are responsible "
			"for numerous vital features like playing sounds or message encryption. "
			"You can load (or unload) them by double-clicking on their names."));
	l_info->setAlignment(Qt::WordBreak);
	// end create main QLabel widgets (icon and app info)

	// our QListView
	lv_modules = new QListView(center);
	lv_modules->addColumn(tr("Module name"), 160);
	lv_modules->addColumn(tr("Version"), 100);
	lv_modules->addColumn(tr("Module type"), 150);
	lv_modules->addColumn(tr("State"), 120);
	lv_modules->setAllColumnsShowFocus(true);
	// end our QListView

	//our QVGroupBox
	QVGroupBox *vgb_info = new QVGroupBox(center);
	vgb_info->setTitle(tr("Info"));
	//end our QGroupBox

	l_moduleinfo = new QLabel(vgb_info);
	l_moduleinfo->setText(tr("<b>Module:</b><br/><b>Depends on:</b><br/><b>Conflicts with:</b><br/><b>Provides:</b><br/><b>Author:</b><br/><b>Version:</b><br/><b>Description:</b>"));

	// buttons
	QHBox *bottom=new QHBox(center);
	hideBaseModules = new QCheckBox(tr("Hide base modules"), bottom);
	hideBaseModules->setChecked(config_file.readBoolEntry("General", "HideBaseModules"));
	connect(hideBaseModules, SIGNAL(clicked()), this, SLOT(refreshList()));
	QWidget *blank2 = new QWidget(bottom);
	bottom->setSpacing(5);
	blank2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
	QPushButton *pb_close = new QPushButton(icons_manager->loadIcon("CloseWindow"), tr("&Close"), bottom, "close");
	// end buttons

	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));
	connect(lv_modules, SIGNAL(selectionChanged()), this, SLOT(itemsChanging()));
	connect(lv_modules, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(moduleAction(QListViewItem *)));

	layoutHelper->addLabel(l_info);
	layoutHelper->addLabel(l_moduleinfo);
	loadGeometry(this, "General", "ModulesDialogGeometry", 0, 30, 600, 620);
	refreshList();
	kdebugf2();
}

ModulesDialog::~ModulesDialog()
{
	kdebugf();
	config_file.writeEntry("General", "HideBaseModules", hideBaseModules->isChecked());
	saveGeometry(this, "General", "ModulesDialogGeometry");
	delete layoutHelper;
	kdebugf2();
}

void ModulesDialog::itemsChanging()
{
	if (lv_modules->selectedItem() != NULL)
		getInfo();
}

void ModulesDialog::moduleAction(QListViewItem *)
{
	kdebugf();
	if (lv_modules->selectedItem() != NULL)
		if ((lv_modules->selectedItem()->text(2) == tr("Dynamic")) &&
			(lv_modules->selectedItem()->text(3) == tr("Loaded")))
			unloadItem();
		else
		if ((lv_modules->selectedItem()->text(2) == tr("Dynamic")) &&
			(lv_modules->selectedItem()->text(3) == tr("Not loaded")))
			loadItem();
	kdebugf2();
}

void ModulesDialog::loadItem()
{
	kdebugf();
	modules_manager->activateModule(lv_modules->selectedItem()->text(0));
	refreshList();
	modules_manager->saveLoadedModules();
	kdebugf2();
}

void ModulesDialog::unloadItem()
{
	kdebugf();
	modules_manager->deactivateModule(lv_modules->selectedItem()->text(0));
	refreshList();
	modules_manager->saveLoadedModules();
	kdebugf2();
}

void ModulesDialog::refreshList()
{
	kdebugf();

	int vScrollValue=lv_modules->verticalScrollBar()->value();

	QString s_selected;

	if (lv_modules->selectedItem() != NULL)
		s_selected = lv_modules->selectedItem()->text(0);

	lv_modules->clear();

	QStringList moduleList = modules_manager->staticModules();
	ModuleInfo info;
	bool hideBase = hideBaseModules->isChecked();
	CONST_FOREACH(module, moduleList)
	{
		if (modules_manager->moduleInfo(*module,info))
		{
			if (!info.base || !hideBase)
				new QListViewItem(lv_modules, *module, info.version, tr("Static"), tr("Loaded"));
		}
		else
			new QListViewItem(lv_modules, *module, QString::null, tr("Static"), tr("Loaded"));
	}

	moduleList = modules_manager->loadedModules();
	CONST_FOREACH(module, moduleList)
	{
		if (modules_manager->moduleInfo(*module,info))
		{
			if (!info.base || !hideBase)
				new QListViewItem(lv_modules, *module, info.version, tr("Dynamic"), tr("Loaded"));
		}
		else
			new QListViewItem(lv_modules, *module, QString::null, tr("Dynamic"), tr("Loaded"));
	}

	moduleList = modules_manager->unloadedModules();
	CONST_FOREACH(module, moduleList)
	{
		if (modules_manager->moduleInfo(*module,info))
		{
			if (!info.base || !hideBase)
				new QListViewItem(lv_modules, *module, info.version, tr("Dynamic"), tr("Not loaded"));
		}
		else
			new QListViewItem(lv_modules, *module, QString::null, tr("Dynamic"), tr("Not loaded"));
	}

	lv_modules->setSelected(lv_modules->findItem(s_selected, 0), true);

	lv_modules->verticalScrollBar()->setValue(vScrollValue);
	kdebugf2();
}

void ModulesDialog::getInfo()
{
	kdebugf();
	ModuleInfo info;

	if (!modules_manager->moduleInfo(lv_modules->selectedItem()->text(0), info))
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
			.arg(lv_modules->selectedItem()->text(0))
			.arg(info.depends.join(", "))
			.arg(info.conflicts.join(", "))
			.arg(info.provides.join(", "))
			.arg(info.author)
			.arg(info.version)
			.arg(info.description);
	layoutHelper->textChanged(l_moduleinfo);
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

	QPopupMenu *MainMenu = kadu->mainMenu();
	MainMenu->insertItem(icons_manager->loadIcon("ManageModules"), tr("&Manage Modules"), this, SLOT(showDialog()), HotKey::shortCutFromFile("ShortCuts", "kadu_modulesmanager"), -1, 2);
	icons_manager->registerMenuItem(MainMenu, tr("&Manage Modules"), "ManageModules");

	registerStaticModules();
	QStringList static_list = staticModules();
	CONST_FOREACH(i, static_list)
		if (!moduleIsActive(*i))
			activateModule(*i);

	// load modules as config file say
	QStringList installed_list = installedModules();
	QString loaded_str = config_file.readEntry("General", "LoadedModules");
	QStringList loaded_list = QStringList::split(',', loaded_str);
	QString unloaded_str = config_file.readEntry("General", "UnloadedModules");
	QStringList unloaded_list = QStringList::split(',', unloaded_str);
	bool load_error = false;
	CONST_FOREACH(i, installed_list)
		if (!moduleIsActive(*i))
		{
			bool load_module;
			if (loaded_list.contains(*i))
				load_module = true;
			else if (unloaded_list.contains(*i))
				load_module = false;
			else
			{
				ModuleInfo m_info;
				if (moduleInfo(*i, m_info))
					load_module = m_info.load_by_def;
				else
					load_module = false;
			}
			if (load_module)
				if (!activateModule(*i))
					load_error = true;
		}

	// if not all modules were loaded properly
	// save the list of modules
	if (load_error)
		saveLoadedModules();

	CONST_FOREACH(it, Modules)
		kdebugm(KDEBUG_INFO, "module: %s, usage: %d\n", it.key().local8Bit().data(), it.data().usage_counter);

	kdebugf2();
}

ModulesManager::~ModulesManager()
{
	kdebugf();

	CONST_FOREACH(it, Modules)
		kdebugm(KDEBUG_INFO, "module: %s, usage: %d\n", it.key().local8Bit().data(), it.data().usage_counter);

	// unloading all not used modules
	// as long as any module were unloaded

	bool deactivated;
	do
	{
		QStringList active = activeModules();
		deactivated = false;
		CONST_FOREACH(i, active)
			if (Modules[*i].usage_counter == 0)
				if (deactivateModule(*i))
					deactivated = true;
	}
	while (deactivated);

	// we cannot unload more modules in normal way
	// so we are making it brutal ;)
	QStringList active = activeModules();
	CONST_FOREACH(i, active)
	{
		kdebugm(KDEBUG_PANIC, "WARNING! Could not deactivate module %s, killing\n",(*i).local8Bit().data());
		deactivateModule((*i), true);
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
	CONST_FOREACH(it, module_info.depends)
	{
		if (!moduleIsActive(*it))
		{
			if (moduleIsInstalled(*it) || moduleIsStatic(*it))
			{
				if (!activateModule(*it))
				{
					kdebugf2();
					return false;
				}
			}
			else
			{
				MessageBox::msg(tr("Required module %1 was not found").arg(*it));
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
	kdebugmf(KDEBUG_FUNCTION_START, "%s\n", module_info.description.local8Bit().data());
	CONST_FOREACH(it, module_info.depends)
	{
		kdebugm(KDEBUG_INFO, "incUsage: %s\n", (*it).local8Bit().data());
		moduleIncUsageCount(*it);
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
	CONST_FOREACH(i, StaticModules)
		static_modules.append(i.key());
	return static_modules;
}

QStringList ModulesManager::installedModules() const
{
	QDir dir(libPath("kadu/modules"),"*." SO_EXT);
	dir.setFilter(QDir::Files);
	QStringList installed;
	QStringList entries = dir.entryList();
	CONST_FOREACH(entry, entries)
		installed.append((*entry).left((*entry).length() - SO_EXT_LEN - 1));
	return installed;
}

QStringList ModulesManager::loadedModules() const
{
	QStringList loaded;
	CONST_FOREACH(i, Modules)
		if (i.data().lib!=NULL)
			loaded.append(i.key());
	return loaded;
}


QStringList ModulesManager::unloadedModules() const
{
	QStringList installed = installedModules();
	QStringList loaded = loadedModules();
	QStringList unloaded;
	CONST_FOREACH(module, installed)
		if (!loaded.contains(*module))
			unloaded.append(*module);
	return unloaded;
}

QStringList ModulesManager::activeModules() const
{
	QStringList active;
	CONST_FOREACH(i, Modules)
		active.append(i.key());
	return active;
}

QString ModulesManager::moduleProvides(const QString &provides)
{
	ModuleInfo info;

	QStringList moduleList = staticModules();
	CONST_FOREACH(moduleName, moduleList)
		if (moduleInfo(*moduleName, info))
			if (info.provides.contains(provides))
				return *moduleName;

	moduleList = installedModules();
	CONST_FOREACH(moduleName, moduleList)
		if (moduleInfo(*moduleName, info) && info.provides.contains(provides))
			if (moduleIsLoaded(*moduleName))
				return *moduleName;

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
	config_file.writeEntry("General", "LoadedModules",loadedModules().join(","));
	config_file.writeEntry("General", "UnloadedModules",unloadedModules().join(","));
	config_file.sync();
}

bool ModulesManager::conflictsWithLoaded(const QString &module_name, const ModuleInfo& module_info) const
{
	kdebugf();
	CONST_FOREACH(it, module_info.conflicts)
	{
		if (moduleIsActive(*it))
		{
			MessageBox::msg(narg(tr("Module %1 conflicts with: %2"), module_name, *it));
			kdebugf2();
			return true;
		}
		CONST_FOREACH(mit, Modules)
			CONST_FOREACH(sit, (*mit).info.provides)
				if ((*it)==(*sit))
				{
					MessageBox::msg(narg(tr("Module %1 conflicts with: %2"), module_name, mit.key()));
					kdebugf2();
					return true;
				}
	}
	CONST_FOREACH(it, Modules)
		CONST_FOREACH(sit, (*it).info.conflicts)
			if ((*sit)==module_name)
			{
				MessageBox::msg(narg(tr("Module %1 conflicts with: %2"), module_name, it.key()));
				kdebugf2();
				return true;
			}
	kdebugf2();
	return false;
}

bool ModulesManager::activateModule(const QString& module_name)
{
	Module m;
	kdebugmf(KDEBUG_FUNCTION_START, "'%s'\n", module_name.local8Bit().data());

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

	typedef int InitModuleFunc();
	InitModuleFunc* init;

	if (moduleIsStatic(module_name))
	{
		m.lib=NULL;
		StaticModule sm=StaticModules[module_name];
		init=sm.init;
		m.close=sm.close;
	}
	else
	{
		m.lib = new Library(libPath("kadu/modules/" + module_name + "." SO_EXT));
		if (!m.lib->load())
		{
			QString err = m.lib->error();
			MessageBox::msg(narg(tr("Cannot load %1 module library.:\n%2"), module_name, err));
			kdebugm(KDEBUG_ERROR, "cannot load %s because of: %s\n", module_name.local8Bit().data(), err.local8Bit().data());
			delete m.lib;
			kdebugf2();
			return false;
		}
		init=(InitModuleFunc*)m.lib->resolve(module_name+"_init");
		m.close=(CloseModuleFunc*)m.lib->resolve(module_name+"_close");
		if (init==NULL||m.close==NULL)
		{
			MessageBox::msg(tr("Cannot find required functions in module %1.\nMaybe it's not Kadu-compatible Module.").arg(module_name));
			delete m.lib;
			kdebugf2();
			return false;
		}
	}

	m.translator = loadModuleTranslation(module_name);

	int res = init();
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
	kdebugmf(KDEBUG_FUNCTION_START, "name:'%s' force:%d usage:%d\n", module_name.local8Bit().data(), force, m.usage_counter);

	if (m.usage_counter>0 && !force)
	{
		MessageBox::msg(tr("Module %1 cannot be deactivated because it is used now").arg(module_name));
		kdebugf2();
		return false;
	}

	CONST_FOREACH(i, m.info.depends)
		moduleDecUsageCount(*i);

	m.close();
	if (m.translator!=NULL)
	{
		qApp->removeTranslator(m.translator);
		delete m.translator;
	}

	if (m.lib!=NULL)
		m.lib->deleteLater();

	Modules.remove(module_name);

	kdebugf2();
	return true;
}

void ModulesManager::showDialog()
{
	kdebugf();
	if (Dialog==NULL)
	{
		Dialog=new ModulesDialog();
		connect(Dialog,SIGNAL(destroyed()),this,SLOT(dialogDestroyed()));
		Dialog->show();
	}
	else
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
