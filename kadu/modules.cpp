/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "modules.h"
#include "debug.h"
#include "message_box.h"
#include "config_file.h"
#include "config_dialog.h"
#include "kadu.h"
#include "kadu-config.h"

#include <qdir.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qpushbutton.h>
#include <qtextcodec.h>
#include <qregexp.h>
#include <qlistview.h>
#include <qvgroupbox.h>
#include <qsizepolicy.h>

#include <dlfcn.h>

#include "modules_static.cpp"

Library::Library(const QString& file_name)
{
	FileName = file_name;
	Handle = NULL;
}

Library::~Library()
{
	kdebugf();
	if (Handle != NULL)
		dlclose(Handle);
	kdebugf2();
}

bool Library::load()
{
	Handle = dlopen(FileName.local8Bit().data(), RTLD_NOW | RTLD_GLOBAL);
	return (Handle != NULL);
}

void* Library::resolve(const QString& symbol_name)
{
	if (Handle == NULL)
		return NULL;
	return dlsym(Handle, symbol_name.local8Bit().data());
}

QString Library::error()
{
	return QString(dlerror());
}

ModulesDialog::ModulesDialog() 
{
	kdebugf();
	setWFlags(Qt::WDestructiveClose);
	setCaption(tr("Manage Modules"));
	
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
	l_icon->setPixmap(icons_manager.loadIcon("ManageModulesWindowIcon"));
	l_info->setText(tr("This dialog box allows you to manage installed modules. Modules are responsible "
			"for numerous vital features like playing sounds or message encryption. "
			"You can load (or unload) them by double-clicking on their names."));
	l_info->setAlignment(Qt::WordBreak);
	// end create main QLabel widgets (icon and app info)
	
	// our QListView
	lv_modules = new QListView(center);
	lv_modules->addColumn(tr("Module name"), 160);
	lv_modules->addColumn(tr("Module type"), 150);
	lv_modules->addColumn(tr("State"), 120);
	lv_modules->setAllColumnsShowFocus(true);
	// end our QListView
	
	//our QVGroupBox
	QVGroupBox *vgb_info = new QVGroupBox(center);
	vgb_info->setTitle(tr("Info"));
	//end our QGroupBox
	
	l_moduleinfo = new QLabel(vgb_info);
	l_moduleinfo->setText(tr("<b>Module:</b><br/><b>Depends on:</b><br/><b>Conflicts with:</b><br/>"
				"<b>Provides:</b><br/><b>Author:</b><br/><b>Description:</b>"));

	// buttons
	QHBox *bottom=new QHBox(center);
	QWidget *blank2=new QWidget(bottom);
	bottom->setSpacing(5);
	blank2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
	QPushButton *pb_close = new QPushButton(icons_manager.loadIcon("CloseWindow"), tr("&Close"), bottom, "close");
	// end buttons
	
	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));
	connect(lv_modules, SIGNAL(selectionChanged()), this, SLOT(itemsChanging()));
	connect(lv_modules, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(moduleAction(QListViewItem *)));
	
 	loadGeometry(this, "General", "ModulesDialogGeometry", 0, 0, 600, 620);
	refreshList();
	kdebugf2();
}

ModulesDialog::~ModulesDialog()
{
	kdebugf();
	saveGeometry(this, "General", "ModulesDialogGeometry");
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
		if ((lv_modules->selectedItem()->text(1) == tr("Dynamic")) && 
			(lv_modules->selectedItem()->text(2) == tr("Loaded")))
			unloadItem();
		else
		if ((lv_modules->selectedItem()->text(1) == tr("Dynamic")) && 
			(lv_modules->selectedItem()->text(2) == tr("Not loaded")))
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

	QStringList sl_list = modules_manager->staticModules();
	for(unsigned int i = 0; i < sl_list.size(); ++i)
		(void) new QListViewItem(lv_modules, sl_list[i], tr("Static"), tr("Loaded"));
		
	sl_list = modules_manager->loadedModules();
	for(unsigned int i = 0; i < sl_list.size(); ++i)
		(void) new QListViewItem(lv_modules, sl_list[i], tr("Dynamic"), tr("Loaded"));
		
	sl_list = modules_manager->unloadedModules();
	for(unsigned int i = 0; i < sl_list.size(); ++i)
		(void) new QListViewItem(lv_modules, sl_list[i], tr("Dynamic"), tr("Not loaded"));
	
	lv_modules->setSelected(lv_modules->findItem(s_selected, 0), true);

	lv_modules->verticalScrollBar()->setValue(vScrollValue);
	kdebugf2();
}

void ModulesDialog::getInfo()
{
	kdebugf();
	ModuleInfo info;
	
	if(!modules_manager->moduleInfo(lv_modules->selectedItem()->text(0), info))
	{
		kdebugf2();
		return;
	}

	l_moduleinfo->setText(tr("<b>Module: </b>") + lv_modules->selectedItem()->text(0) + 
				tr("<br/><b>Depends on: </b>") + info.depends.join(", ") + 
				tr("<br/><b>Conflicts with: </b>") + info.conflicts.join(", ") + 
				tr("<br/><b>Provides: </b>") + info.provides.join(", ") + 
				tr("<br/><b>Author: </b>") + info.author +
				tr("<br/><b>Description: </b>") + info.description);
	kdebugf2();
}

void ModulesDialog::keyPressEvent(QKeyEvent *ke_event)
{
	if (ke_event->key() == Qt::Key_Escape)
		close();
}

void ModulesManager::initModule()
{
	new ModulesManager();
}

void ModulesManager::closeModule()
{
	delete modules_manager;
}

ModulesManager::ModulesManager() : QObject(NULL, "modules_manager")
{
	kdebugf();
	// wazne, aby inicjalizacja tej zmiennej wystapila przed wczytaniem
	// modulow, bo niektore juz moga z niej korzystac, wiec dlatego
	// przenioslem to tutaj z funkcji ModulesManagert::initModule
	// to samo z menu - moduly powinny sie ladowac na samym koncu
	modules_manager=this;
	kadu->mainMenu()->insertItem(icons_manager.loadIcon("ManageModules"), tr("&Manage Modules"), this, SLOT(showDialog()), HotKey::shortCutFromFile("ShortCuts", "kadu_modulesmanager"), -1, 2);
	//
	Dialog=NULL;
	//
	registerStaticModules();
	QStringList static_list=staticModules();
	for (QStringList::ConstIterator i=static_list.begin(); i!=static_list.end(); ++i)
		if(!moduleIsActive(*i))
			activateModule(*i);

	// za³aduj modu³y wed³ug pliku konfiguracyjnego
	// i ew. zmiennej
	QStringList installed_list = installedModules();
	QString loaded_str = config_file.readEntry("General", "LoadedModules");
	QStringList loaded_list = QStringList::split(',',loaded_str);
	QString unloaded_str = config_file.readEntry("General", "UnloadedModules");
	QStringList unloaded_list = QStringList::split(',',unloaded_str);
	bool load_error = false;
	for(QStringList::ConstIterator i = installed_list.begin(); i != installed_list.end(); i++)
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
				if(!activateModule(*i))
					load_error = true;
		}

	// jesli nie wszystkie moduly zostaly przy starcie prawidlowo
	// zaladowane to zapisz nowa liste zaladowanych modulow
	if(load_error)
		saveLoadedModules();

	ConfigDialog::addTab("ShortCuts", "ShortCutsTab");
	ConfigDialog::addVGroupBox("ShortCuts", "ShortCuts", "Define keys");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QString("&Manage Modules").replace(QRegExp("&"), ""), "kadu_modulesmanager", "F4");
	kdebugf2();
}

ModulesManager::~ModulesManager()
{
	kdebugf();
	// Wyladowujemy wszystkie nieu¿ywane modu³y
	// w pêtli iteruj±c dopóki jakikolwiek udaje
	// siê wy³adowaæ
	bool deactivated;
	do
	{
		QStringList active=activeModules();
		deactivated=false;
		for(QStringList::ConstIterator i=active.begin(); i!=active.end(); ++i)
			if(Modules[*i].usage_counter == 0)
				if(deactivateModule(*i))
					deactivated=true;
	}
	while(deactivated);
	// Wiêcej modu³ów nie mo¿na wy³adowaæ normalnie,
	// je¶li jakie¶ zosta³y trzeba to zrobiæ brutalnie
	QStringList active=activeModules();
	for(QStringList::ConstIterator i=active.begin(); i!=active.end(); ++i)
	{
		kdebugm(KDEBUG_PANIC, "WARNING! Could not deactivate module %s, killing\n",(*i).local8Bit().data());
		deactivateModule(*i,true);
	}
	kdebugf2();
}

QTranslator* ModulesManager::loadModuleTranslation(const QString& module_name)
{
	QTranslator* translator=new QTranslator(0);
	if(translator->load(dataPath("kadu/modules/translations/"+module_name+QString("_") + config_file.readEntry("General", "Language", QTextCodec::locale())), "."))
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
	for (QStringList::ConstIterator it = module_info.depends.begin(); it != module_info.depends.end(); ++it)
	{
		if(!moduleIsActive(*it))
		{
			if(moduleIsInstalled(*it) || moduleIsStatic(*it))
			{
				if(!activateModule(*it))
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
	for (QStringList::ConstIterator it = module_info.depends.begin(); it != module_info.depends.end(); ++it)
		moduleIncUsageCount(*it);
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
	for (QMap<QString,StaticModule>::const_iterator i=StaticModules.begin(); i!=StaticModules.end(); ++i)
		static_modules.append(i.key());
	return static_modules;
}

QStringList ModulesManager::installedModules() const
{
	QDir dir(dataPath("kadu/modules"),"*.so");
	dir.setFilter(QDir::Files);
	QStringList installed;
	for(unsigned int i=0; i<dir.count(); ++i)
	{
		QString name=dir[i];
		installed.append(name.left(name.length()-3));
	}
	return installed;
}

QStringList ModulesManager::loadedModules() const
{
	QStringList loaded;
	for (QMap<QString,Module>::const_iterator i=Modules.begin(); i!=Modules.end(); ++i)
		if(i.data().lib!=NULL)
			loaded.append(i.key());
	return loaded;
}

QStringList ModulesManager::unloadedModules() const
{
	QStringList installed=installedModules();
	QStringList loaded=loadedModules();
	QStringList unloaded;
	for(unsigned int i=0; i<installed.size(); ++i)
	{
		QString name=installed[i];
		if(!loaded.contains(name))
			unloaded.append(name);
	}
	return unloaded;
}

QStringList ModulesManager::activeModules() const
{
	QStringList active;
	for (QMap<QString,Module>::const_iterator i=Modules.begin(); i!=Modules.end(); ++i)
		active.append(i.key());
	return active;
}

bool ModulesManager::moduleInfo(const QString& module_name, ModuleInfo& info) const
{
	if(Modules.contains(module_name))
	{
		info=Modules[module_name].info;
		return true;
	}

	ConfigFile desc_file(dataPath("kadu/modules/"+module_name+".desc"));

	QString lang=config_file.readEntry("General", "Language", "en");

	info.description = desc_file.readEntry("Module", "Description["+lang+"]");
	if(info.description.isEmpty())
		info.description = desc_file.readEntry("Module", "Description");

	info.author = desc_file.readEntry("Module", "Author");

	info.depends = QStringList::split(" ",
		desc_file.readEntry("Module", "Dependencies"));

	info.conflicts=QStringList::split(" ",
		desc_file.readEntry("Module", "Conflicts"));

	info.provides=QStringList::split(" ",
		desc_file.readEntry("Module", "Provides"));

	info.load_by_def = desc_file.readBoolEntry("Module", "LoadByDefault");

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
	for (QStringList::ConstIterator it = module_info.conflicts.begin(); it != module_info.conflicts.end(); ++it)
	{
		if(moduleIsActive(*it))
		{
			MessageBox::msg(tr("Module %1 conflicts with: %2").arg(module_name).arg(*it));
			kdebugf2();
			return true;
		}
		for (QMap<QString, Module>::const_iterator mit=Modules.begin(); mit!=Modules.end(); ++mit)
			for (QStringList::const_iterator sit=(*mit).info.provides.begin(); sit!=(*mit).info.provides.end(); ++sit)
				if ((*it)==(*sit))
				{
					MessageBox::msg(tr("Module %1 conflicts with: %2").arg(module_name).arg(mit.key()));
					kdebugf2();
					return true;
				}
	}
	for (QMap<QString, Module>::const_iterator it=Modules.begin(); it!=Modules.end(); ++it)
		for (QStringList::const_iterator sit=(*it).info.conflicts.begin(); sit!=(*it).info.conflicts.end(); ++sit)
			if ((*sit)==module_name)
			{
				MessageBox::msg(tr("Module %1 conflicts with: %2").arg(module_name).arg(it.key()));
				kdebugf2();
				return true;
			}
	kdebugf2();
	return false;
}

bool ModulesManager::activateModule(const QString& module_name)
{
	Module m;
	kdebugm(KDEBUG_FUNCTION_START, "ModulesManager::activateModule(%s)\n", module_name.local8Bit().data());
	
	if(moduleIsActive(module_name))
	{
		MessageBox::msg(tr("Module %1 is already active").arg(module_name));
		kdebugf2();
		return false;
	}

	if(moduleInfo(module_name,m.info))
	{
		if (conflictsWithLoaded(module_name, m.info))
		{
			kdebugf2();
			return false;
		}
		if(!satisfyModuleDependencies(m.info))
		{
			kdebugf2();
			return false;
		}
	}

	typedef int InitModuleFunc();
	InitModuleFunc* init;
	
	if(moduleIsStatic(module_name))
	{
		m.lib=NULL;
		StaticModule sm=StaticModules[module_name];
		init=sm.init;
		m.close=sm.close;
	}
	else
	{
		m.lib=new Library(dataPath("kadu/modules/"+module_name+".so"));
		if(!m.lib->load())
		{
			MessageBox::msg(tr("Cannot load %1 module library.:\n%2").arg(module_name).arg(m.lib->error()));
			kdebugf2();
			return false;
		}	
		init=(InitModuleFunc*)m.lib->resolve(module_name+"_init");
		m.close=(CloseModuleFunc*)m.lib->resolve(module_name+"_close");
		if(init==NULL||m.close==NULL)
		{
			MessageBox::msg(tr("Cannot find required functions in module %1.\nMaybe it's not Kadu-compatible Module.").arg(module_name));
			delete m.lib;
			kdebugf2();
			return false;
		}
	}
	
	m.translator = loadModuleTranslation(module_name);

	int res=init();
	if(res!=0)
	{
		MessageBox::msg(tr("Module initialization routine for %1 failed.").arg(module_name));
		if(m.lib!=NULL)
			delete m.lib;
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
	kdebugm(KDEBUG_FUNCTION_START, "ModulesManager::deactivateModule(%s,%d) usage:%d\n", (const char *)module_name.local8Bit(), force, m.usage_counter);

	if(m.usage_counter>0 && !force)
	{
		MessageBox::msg(tr("Module %1 cannot be deactivated because it is used now").arg(module_name));
		kdebugf2();
		return false;
	}
	
	for (QStringList::Iterator i = m.info.depends.begin(); i != m.info.depends.end(); ++i)
		moduleDecUsageCount(*i);
	
	m.close();
	if(m.translator!=NULL)
	{
		qApp->removeTranslator(m.translator);
		delete m.translator;
	}
	
	if(m.lib!=NULL)
		m.lib->deleteLater();

	Modules.remove(module_name);
	
	kdebugf2();
	return true;
}

void ModulesManager::showDialog()
{
	kdebugf();
	if(Dialog==NULL)
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

