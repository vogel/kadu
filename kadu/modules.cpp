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
#include <qlabel.h>
#include <qpushbutton.h>
#include <qtextcodec.h>
#include <qregexp.h>

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
	: QDialog(NULL,NULL)
{
	setWFlags(Qt::WDestructiveClose);
	setCaption(tr("Manage Modules"));
	
	QHBoxLayout* layout=new QHBoxLayout(this);
	layout->setAutoAdd(true);
	
	QVBox* static_box=new QVBox(this);
	QVBox* installed_box=new QVBox(this);
	QVBox* loaded_box=new QVBox(this);	

	/*QLabel* StaticLabel =*/ new QLabel(tr("Static"), static_box);
	StaticListBox = new QListBox(static_box);
	StaticListBox->insertStringList(modules_manager->staticModules());

	/*QLabel* InstalledLabel = */new QLabel(tr("Installed"), installed_box);
	InstalledListBox = new QListBox(installed_box);
	connect(InstalledListBox,SIGNAL(doubleClicked(QListBoxItem*)),
		this,SLOT(loadItem(QListBoxItem*)));
	
	/*QLabel* LoadedLabel =*/ new QLabel(tr("Loaded"), loaded_box);
	LoadedListBox = new QListBox(loaded_box);
	connect(LoadedListBox,SIGNAL(doubleClicked(QListBoxItem*)),
		this,SLOT(unloadItem(QListBoxItem*)));

	QButton* InfoButton=new QPushButton(tr("Info"), static_box);
	connect(InfoButton, SIGNAL(clicked()),
		this, SLOT(getInfo()));

	QButton* LoadButton=new QPushButton(tr("Load"), installed_box);
	connect(LoadButton,SIGNAL(clicked()),
		this,SLOT(loadSelectedItem()));


	QButton* UnloadButton=new QPushButton(tr("Unload"), loaded_box);
	connect(UnloadButton,SIGNAL(clicked()),
		this,SLOT(unloadSelectedItem()));

	QRect def_rect(0, 0, 450, 400);
	config_file.addVariable("General", "ModulesDialogGeometry", def_rect);

	QRect geom=config_file.readRectEntry("General", "ModulesDialogGeometry");
	resize(geom.width(),geom.height());
	move(geom.x(),geom.y());

	refreshLists();
}

ModulesDialog::~ModulesDialog()
{
	kdebugf();
	QRect geom;
	geom.setX(pos().x());
	geom.setY(pos().y());
	geom.setWidth(size().width());
	geom.setHeight(size().height());
	
	config_file.writeEntry("General", "ModulesDialogGeometry",geom);
}

void ModulesDialog::loadItem(QListBoxItem* item)
{
	QString mod_name=item->text();
	if(modules_manager->activateModule(mod_name))
	{
		refreshLists();
		modules_manager->saveLoadedModules();
	}		
}

void ModulesDialog::unloadItem(QListBoxItem* item)
{
	QString mod_name=item->text();
	modules_manager->deactivateModule(mod_name);
	refreshLists();
	modules_manager->saveLoadedModules();
}

void ModulesDialog::loadSelectedItem()
{
	int current=InstalledListBox->currentItem();
	if(current>=0)
		loadItem(InstalledListBox->item(current));
}

void ModulesDialog::unloadSelectedItem()
{
	int current=LoadedListBox->currentItem();
	if(current>=0)
		unloadItem(LoadedListBox->item(current));
}

void ModulesDialog::refreshLists()
{
	kdebugf();
	LoadedListBox->clear();
	LoadedListBox->insertStringList(modules_manager->loadedModules());	
	InstalledListBox->clear();
	InstalledListBox->insertStringList(modules_manager->unloadedModules());
}

void ModulesDialog::getInfo()
{
	kdebugf();
	QListBoxItem *item;
	int current;
	QListBox *currentListBox;

	if(StaticListBox->hasFocus())
		currentListBox=StaticListBox;
	else if(InstalledListBox->hasFocus())
		currentListBox=InstalledListBox;
	else
		currentListBox=LoadedListBox;

	current=currentListBox->currentItem();
	item=currentListBox->item(current);
	
	if(current>=0){
		ModuleInfo info;
		QString message;
		if(!modules_manager->moduleInfo(item->text(), info))
			return;
		message+=tr(
				"<b>Module:</b>"
				"<br/>%1<br/>"
				"<b>Depends on:</b><br/>").arg(item->text());
		message+=info.depends.join("\n");
		message+=tr("<br/><b>Conflicts with:</b><br/>");
		message+=info.conflicts.join("\n");
		message+=tr("<br/><b>Provides:</b><br/>");
		message+=info.provides.join("\n");
		message+=tr(
				"<br/><b>Author:</b><br/>"
				"%1<br/>"
				"<b>Description</b>:<br/>"
				"%2").arg(info.author).arg(info.description);
		MessageBox::msg(message);
	}
}

void ModulesManager::initModule()
{
	new ModulesManager();
}

void ModulesManager::closeModule()
{
	delete modules_manager;
}

ModulesManager::ModulesManager() : QObject()
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
	for (QStringList::ConstIterator i=static_list.begin(); i!=static_list.end(); i++)
		if(!moduleIsActive(*i))
			activateModule(*i);
	//
	QString loaded_str=config_file.readEntry("General", "LoadedModules");
	QStringList loaded_list=QStringList::split(',',loaded_str);
	bool all_loaded=true;
	for(QStringList::ConstIterator i=loaded_list.begin(); i!=loaded_list.end(); i++)
		if(!moduleIsActive(*i))
			if(!activateModule(*i))
				all_loaded=false;
	// jesli nie wszystkie moduly zostaly przy starcie prawidlowo
	// zaladowane to zapisz nowa liste zaladowanych modulow
	if(!all_loaded)
		saveLoadedModules();

	ConfigDialog::addTab("ShortCuts");
	ConfigDialog::addVGroupBox("ShortCuts", "ShortCuts", "Define keys");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QString("&Manage Modules").replace(QRegExp("&"), ""), "kadu_modulesmanager", "F4");
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
		for(QStringList::ConstIterator i=active.begin(); i!=active.end(); i++)
			if(Modules[*i].usage_counter == 0)
				if(deactivateModule(*i))
					deactivated=true;
	}
	while(deactivated);
	// Wiêcej modu³ów nie mo¿na wy³adowaæ normalnie,
	// je¶li jakie¶ zosta³y trzeba to zrobiæ brutalnie
	QStringList active=activeModules();
	for(QStringList::ConstIterator i=active.begin(); i!=active.end(); i++)
	{
		kdebug("WARNING! Could not deactivate module %s, killing\n",(*i).local8Bit().data());
		deactivateModule(*i,true);
	}
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
					return false;
			}			
			else
			{
				MessageBox::msg(tr("Required module %1 was not found").arg(*it));
				return false;
			}
		}
	}
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
	for (QMap<QString,StaticModule>::const_iterator i=StaticModules.begin(); i!=StaticModules.end(); i++)
		static_modules.append(i.key());
	return static_modules;
}

QStringList ModulesManager::installedModules() const
{
	QDir dir(dataPath("kadu/modules"),"*.so");
	dir.setFilter(QDir::Files);
	QStringList installed;
	for(unsigned int i=0; i<dir.count(); i++)
	{
		QString name=dir[i];
		installed.append(name.left(name.length()-3));
	}
	return installed;
}

QStringList ModulesManager::loadedModules() const
{
	QStringList loaded;
	for (QMap<QString,Module>::const_iterator i=Modules.begin(); i!=Modules.end(); i++)
		if(i.data().lib!=NULL)
			loaded.append(i.key());
	return loaded;
}

QStringList ModulesManager::unloadedModules() const
{
	QStringList installed=installedModules();
	QStringList loaded=loadedModules();
	QStringList unloaded;
	for(unsigned int i=0; i<installed.size(); i++)
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
	for (QMap<QString,Module>::const_iterator i=Modules.begin(); i!=Modules.end(); i++)
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
	config_file.sync();
}

bool ModulesManager::conflictsWithLoaded(const QString &module_name, const ModuleInfo& module_info) const
{
	for (QStringList::ConstIterator it = module_info.conflicts.begin(); it != module_info.conflicts.end(); ++it)
	{
		if(moduleIsActive(*it))
		{
			MessageBox::msg(tr("Module %1 conflicts with: %2").arg(module_name).arg(*it));
			return true;
		}
		for (QMap<QString, Module>::const_iterator mit=Modules.begin(); mit!=Modules.end(); mit++)
			for (QStringList::const_iterator sit=(*mit).info.provides.begin(); sit!=(*mit).info.provides.end(); sit++)
				if ((*it)==(*sit))
				{
					MessageBox::msg(tr("Module %1 conflicts with: %2").arg(module_name).arg(mit.key()));
					return true;
				}
	}
	for (QMap<QString, Module>::const_iterator it=Modules.begin(); it!=Modules.end(); it++)
		for (QStringList::const_iterator sit=(*it).info.conflicts.begin(); sit!=(*it).info.conflicts.end(); sit++)
			if ((*sit)==module_name)
			{
				MessageBox::msg(tr("Module %1 conflicts with: %2").arg(module_name).arg(it.key()));
				return true;
			}
	return false;
}

bool ModulesManager::activateModule(const QString& module_name)
{
	Module m;
	kdebug(QString("activateModule %1\n").arg(module_name));
	
	if(moduleIsActive(module_name))
	{
		MessageBox::msg(tr("Module %1 is already active").arg(module_name));
		return false;
	}

	if(moduleInfo(module_name,m.info))
	{
		if (conflictsWithLoaded(module_name, m.info))
			return false;
		if(!satisfyModuleDependencies(m.info))
			return false;
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
			return false;
		}	
		init=(InitModuleFunc*)m.lib->resolve(module_name+"_init");
		m.close=(CloseModuleFunc*)m.lib->resolve(module_name+"_close");
		if(init==NULL||m.close==NULL)
		{
			MessageBox::msg(tr("Cannot find required functions.\nMaybe it's not Kadu-compatible Module."));
			delete m.lib;
			return false;
		}
	}
	
	m.translator = loadModuleTranslation(module_name);

	int res=init();
	if(res!=0)
	{
		MessageBox::msg(tr("Module initialization routine failed."));
		if(m.lib!=NULL)
			delete m.lib;
		return false;		
	}
	
	incDependenciesUsageCount(m.info);
	
	m.usage_counter=0;
	Modules.insert(module_name,m);
	return true;
}

bool ModulesManager::deactivateModule(const QString& module_name, bool force)
{
	Module m=Modules[module_name];
	kdebug("ModulesManager::deactivateModule(%s,%d) usage:%d\n", (const char *)module_name.local8Bit(), force, m.usage_counter);

	if(m.usage_counter>0 && !force)
	{
		MessageBox::msg(tr("Module %1 cannot be deactivated because it is used now").arg(module_name));
		return false;
	}
	
	for (QStringList::Iterator i = m.info.depends.begin(); i != m.info.depends.end(); i++)
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
}

void ModulesManager::dialogDestroyed()
{
	kdebugf();
	Dialog=NULL;
}

void ModulesManager::moduleIncUsageCount(const QString& module_name)
{
	Modules[module_name].usage_counter++;	
}

void ModulesManager::moduleDecUsageCount(const QString& module_name)
{
	Modules[module_name].usage_counter--;
}

ModulesManager* modules_manager;

