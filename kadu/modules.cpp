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
#include "kadu.h"
#include "kadu-config.h"

#include <qdir.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qtextcodec.h>

#include "modules_static.cpp"

ModulesDialog::ModulesDialog()
	: QDialog(NULL,NULL)
{
	setWFlags(Qt::WDestructiveClose);
	resize(400,200);
	setCaption(tr("Manage Modules"));
	
	QHBoxLayout* layout=new QHBoxLayout(this);
	QVBoxLayout* static_layout=new QVBoxLayout(layout);
	QVBoxLayout* installed_layout=new QVBoxLayout(layout);
	QVBoxLayout* loaded_layout=new QVBoxLayout(layout);	
	QVBoxLayout* buttons_layout=new QVBoxLayout(layout);

	QLabel* StaticLabel = new QLabel(this);
	StaticLabel->setText(QString("<b>")+tr("Static modules")+"</b>");
	
	QListBox* StaticListBox = new QListBox(this);
	StaticListBox->insertStringList(modules_manager->staticModules());

	static_layout->addWidget(StaticLabel);
	static_layout->addWidget(StaticListBox);
		
	QLabel* InstalledLabel = new QLabel(this);
	InstalledLabel->setText(QString("<b>")+tr("Installed modules")+"</b>");
	
	InstalledListBox = new QListBox(this);
	connect(InstalledListBox,SIGNAL(doubleClicked(QListBoxItem*)),
		this,SLOT(loadItem(QListBoxItem*)));
	
	QButton* LoadButton=new QPushButton(this);
	LoadButton->setText(tr("Load"));
	connect(LoadButton,SIGNAL(clicked()),
		this,SLOT(loadSelectedItem()));

	QButton* InfoButton=new QPushButton(this);
	InfoButton->setText(tr("Info"));
	connect(InfoButton, SIGNAL(clicked()),
		this, SLOT(getInfo()));
	
	installed_layout->addWidget(InstalledLabel);
	installed_layout->addWidget(InstalledListBox);

	QLabel* LoadedLabel = new QLabel(this);
	LoadedLabel->setText(QString("<b>")+tr("Loaded modules")+"</b>");

	LoadedListBox = new QListBox(this);
	connect(LoadedListBox,SIGNAL(doubleClicked(QListBoxItem*)),
		this,SLOT(unloadItem(QListBoxItem*)));

	QButton* UnloadButton=new QPushButton(this);
	UnloadButton->setText(tr("Unload"));
	connect(UnloadButton,SIGNAL(clicked()),
		this,SLOT(unloadSelectedItem()));

	loaded_layout->addWidget(LoadedLabel);
	loaded_layout->addWidget(LoadedListBox);

	buttons_layout->addWidget(LoadButton);
	buttons_layout->addWidget(UnloadButton);
	buttons_layout->addWidget(InfoButton);

	refreshLists();
}

void ModulesDialog::loadItem(QListBoxItem* item)
{
	QString mod_name=item->text();
	if(modules_manager->loadModule(mod_name))
	{
		refreshLists();
		modules_manager->saveLoadedModules();
	}		
}

void ModulesDialog::unloadItem(QListBoxItem* item)
{
	QString mod_name=item->text();
	modules_manager->unloadModule(mod_name);
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
	LoadedListBox->clear();
	LoadedListBox->insertStringList(modules_manager->loadedModules());	
	InstalledListBox->clear();
	InstalledListBox->insertStringList(modules_manager->unloadedModules());
}

void ModulesDialog::getInfo()
{
	QListBoxItem *item;
	int current;

	if(InstalledListBox->hasFocus()){
		current=InstalledListBox->currentItem();
		item=InstalledListBox->item(current);
	}
	else{
		current=LoadedListBox->currentItem();
		item=LoadedListBox->item(current);
	}
	
	if(current>=0){
		ModuleInfo info;
		QString message;
		if(!modules_manager->moduleInfo(item->text(), info))
			return;
		message+=tr(
				"<b>Module:</b>"
				"<br>%1<br>"
				"<b>Depends on:</b><br>").arg(item->text());
		for (QStringList::Iterator it = info.depends.begin(); it != info.depends.end(); ++it)
			message+=QString("%1\n").arg((*it));
		message+=tr(
				"<br><b>Author:</b><br>"
				"%1<br>"
				"<b>Description</b>:<br>"
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
	// wazne, aby inicjalizacja tej zmiennej wystapila przed wczytaniem
	// modulow, bo niektore juz moga z niej korzystac, wiec dlatego
	// przenioslem to tutaj z funkcji ModulesManagert::initModule
	// to samo z menu - moduly powinny sie ladowac na samym koncu
	modules_manager=this;
	kadu->mainMenu()->insertItem(icons_manager.loadIcon("ManageModules"), tr("&Manage Modules"), this, SLOT(showDialog()), QKeySequence(), -1, 2);
	//
	Dialog=NULL;
	//
	initStaticModules();
	//
	QString loaded_str=config_file.readEntry("General", "LoadedModules");
	QStringList loaded_list=QStringList::split(',',loaded_str);
	bool all_loaded=true;
	for(int i=0; i<loaded_list.count(); i++)
		if(!Modules.contains(loaded_list[i]))
			if(!loadModule(loaded_list[i]))
				all_loaded=false;
	// jesli nie wszystkie moduly zostaly przy starcie prawidlowo
	// zaladowane to zapisz nowa liste zaladowanych modulow
	if(!all_loaded)
		saveLoadedModules();
}

ModulesManager::~ModulesManager()
{
	QStringList loaded=loadedModules();
	for(int i=0; i<loaded.size(); i++)
		unloadModule(loaded[i], true);
	//
	closeStaticModules();
}

QStringList ModulesManager::staticModules()
{
	return QStringList::split(" ",STATIC_MODULES);
}

QStringList ModulesManager::installedModules()
{
	QDir dir(QString(DATADIR)+"/kadu/modules","*.so");
	dir.setFilter(QDir::Files);
	QStringList installed;
	for(int i=0; i<dir.count(); i++)
	{
		QString name=dir[i];
		installed.append(name.left(name.length()-3));
	}
	return installed;
}

QStringList ModulesManager::loadedModules()
{
	QStringList loaded;
	for (QMap<QString,Module>::const_iterator it=Modules.begin(); it!=Modules.end(); it++)
		loaded.append(it.key());
	return loaded;
}

QStringList ModulesManager::unloadedModules()
{
	QStringList installed=installedModules();
	QStringList unloaded;
	for(int i=0; i<installed.size(); i++)
	{
		QString name=installed[i];
		if(!Modules.contains(name))
			unloaded.append(name);
	}
	return unloaded;
}

bool ModulesManager::moduleInfo(const QString& module_name, ModuleInfo& info)
{
	if(Modules.contains(module_name))
	{
		info=Modules[module_name].info;
		return true;
	}
	QLibrary* lib=new QLibrary(QString(DATADIR)+"/kadu/modules/"+module_name+".so");

	if(!lib->load())
	{
		MessageBox::msg(tr("Cannot load %1 module library.\nMaybe it's incorrecty compiled.").arg(module_name));
		return false;
	}
		
	typedef void InfoModuleFunc(ModuleInfo* info);
	InfoModuleFunc* info_f=(InfoModuleFunc*)lib->resolve(module_name+"_info");

	if(info_f)
		info_f(&info);
	else
	{
		delete lib;
		return true;
	}

	delete lib;
	return true;
}

bool ModulesManager::loadModule(const QString& module_name)
{
	Module m;
	ModuleInfo modinfo;

	kdebug(QString("loadModule %1\n").arg(module_name));
	
	if(Modules.contains(module_name)){
		MessageBox::msg(tr("Module %1 is loaded").arg(module_name));
		return false;
	}

	m.lib=new QLibrary(QString(DATADIR)+"/kadu/modules/"+module_name+".so");
	if(!m.lib->load())
	{
		MessageBox::msg(tr("Cannot load %1 module library.\nMaybe it's incorrecty compiled.").arg(module_name));
		return false;
	}
		
	typedef int InitModuleFunc();
	typedef void InfoModuleFunc(ModuleInfo* info);
	InitModuleFunc* init=(InitModuleFunc*)m.lib->resolve(module_name+"_init");
	InfoModuleFunc* info=(InfoModuleFunc*)m.lib->resolve(module_name+"_info");
	m.close=(CloseModuleFunc*)m.lib->resolve(module_name+"_close");

	if(init==NULL||m.close==NULL)
	{
		MessageBox::msg(tr("Cannot find required functions.\nMaybe it's not Kadu-compatible Module."));
		delete m.lib;
		return false;
	}

	if(info)
	{
		info(&modinfo);
		for (QStringList::Iterator it = modinfo.depends.begin(); it != modinfo.depends.end(); ++it)
		{
			if((*loadedModules().find(*it))==QString::null)
			{
				if((*installedModules().find(*it))!=QString::null)
				{
					if(loadModule(*it))
						Modules[*it].usage_counter++;
					else
					{
						delete m.lib;
						return false;
					}
				}			
				else
				{
					MessageBox::msg(tr("Required module %1 was not found").arg(*it));
					delete m.lib;
					return false;
				}
			}
			else
				Modules[*it].usage_counter++;
		}
		m.info=modinfo;
	}

	m.translator=new QTranslator(0);
	if(m.translator->load(QString(DATADIR) + QString("/kadu/modules/translations/")+module_name+QString("_") + config_file.readEntry("General", "Language", QTextCodec::locale()), "."))
	{
		qApp->installTranslator(m.translator);
	}
	else
	{
		delete m.translator;
		m.translator=NULL;
	}

	int res=init();
	if(res!=0)
	{
		MessageBox::msg(tr("Module initialization routine failed."));
		delete m.lib;
		return false;		
	}
	
	m.usage_counter=0;
	Modules.insert(module_name,m);
	return true;
}

bool ModulesManager::unloadModule(const QString& module_name, bool force)
{
	Module m=Modules[module_name];

	if(m.usage_counter>0 && !force){
		MessageBox::msg(tr("Module %1 cannot be unloaded because it is used by another module").arg(module_name));
		return false;
	}
	
	for (QStringList::Iterator it = m.info.depends.begin(); it != m.info.depends.end(); ++it)
		Modules[*it].usage_counter--;
	
	m.close();
	if(m.translator!=NULL)
	{
		qApp->removeTranslator(m.translator);
		delete m.translator;
	}
	delete m.lib;
	Modules.remove(module_name);
}

void* ModulesManager::moduleSymbol(const QString& module_name, const QString& symbol_name)
{
	if (!Modules.contains(module_name))
		return NULL;
	Module m=Modules[module_name];
	return m.lib->resolve(symbol_name);
}

void ModulesManager::saveLoadedModules()
{
	config_file.writeEntry("General", "LoadedModules",loadedModules().join(","));
	config_file.sync();
}

void ModulesManager::showDialog()
{
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
	Dialog=NULL;
}


ModulesManager* modules_manager;

