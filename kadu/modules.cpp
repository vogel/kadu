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

#include <qdir.h>
#include <qlayout.h>

void ModulesManager::initModule()
{
	modules_manager=new ModulesManager();
}

ModulesManager::ModulesManager() : QObject()
{
}

QStringList ModulesManager::installedModules()
{
	QDir dir(QString(DATADIR)+"/kadu/modules");
	dir.setFilter(QDir::Files);
	return dir.entryList();
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

bool ModulesManager::loadModule(const QString& module_name)
{
	Module m;
	m.lib=new QLibrary(QString(DATADIR)+"/kadu/modules/"+module_name);
	if(m.lib->load())
	{
		typedef void InitModuleFunc();
		InitModuleFunc* init=(InitModuleFunc*)m.lib->resolve("init_module");
		m.close=(CloseModuleFunc*)m.lib->resolve("close_module");
		if(init!=NULL&&m.close!=NULL)
			init();
		else
		{
			fprintf(stderr,"Cannot find init_module() or close_module() in loaded module\n");
			return false;
		}
		Modules.insert(module_name,m);
	}
	else
	{
		fprintf(stderr,"Cannot load module\n");	
		return false;
	}
	return true;
}

void ModulesManager::unloadModule(const QString& module_name)
{
	Module m=Modules[module_name];
	m.close();
	delete m.lib;
	Modules.remove(module_name);
}

void ModulesManager::showDialog()
{
	ModulesDialog* dialog=new ModulesDialog();
	dialog->show();
}

ModulesManager* modules_manager;

ModulesDialog::ModulesDialog()
	: QDialog(NULL,NULL)
{
	setWFlags(Qt::WDestructiveClose);
	resize(300,200);
	
	QHBoxLayout* layout= new QHBoxLayout(this);
	layout->setAutoAdd(true);

	InstalledListBox = new QListBox(this);
	InstalledListBox->insertStringList(modules_manager->unloadedModules());
	connect(InstalledListBox,SIGNAL(doubleClicked(QListBoxItem*)),
		this,SLOT(loadItem(QListBoxItem*)));
	
	LoadedListBox = new QListBox(this);
	LoadedListBox->insertStringList(modules_manager->loadedModules());	
	connect(LoadedListBox,SIGNAL(doubleClicked(QListBoxItem*)),
		this,SLOT(unloadItem(QListBoxItem*)));
}

void ModulesDialog::loadItem(QListBoxItem* item)
{
	QString mod_name=item->text();
	if(modules_manager->loadModule(mod_name))
	{
		LoadedListBox->insertItem(mod_name);
		InstalledListBox->removeItem(InstalledListBox->currentItem());
	}		
}

void ModulesDialog::unloadItem(QListBoxItem* item)
{
	QString mod_name=item->text();
	modules_manager->unloadModule(mod_name);
	InstalledListBox->insertItem(mod_name);
	LoadedListBox->removeItem(LoadedListBox->currentItem());
}
