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

#include <qdir.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>

void ModulesManager::initModule()
{
	modules_manager=new ModulesManager();
	kadu->mainMenu()->insertItem(loadIcon("configure.png"), tr("&Manage Modules"), modules_manager, SLOT(showDialog()), QKeySequence(), -1, 2);
}

ModulesManager::ModulesManager() : QObject()
{
	QString loaded_str=config_file.readEntry("General", "LoadedModules");
	QStringList loaded_list=QStringList::split(',',loaded_str);
	for(int i=0; i<loaded_list.count(); i++)
		loadModule(loaded_list[i]);
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

bool ModulesManager::loadModule(const QString& module_name)
{
	Module m;
	m.lib=new QLibrary(QString(DATADIR)+"/kadu/modules/"+module_name+".so");
	if(m.lib->load())
	{
		typedef void InitModuleFunc();
		InitModuleFunc* init=(InitModuleFunc*)m.lib->resolve("init_module");
		m.close=(CloseModuleFunc*)m.lib->resolve("close_module");
		if(init!=NULL&&m.close!=NULL)
			init();
		else
		{
			MessageBox::msg(tr("Cannot find init_module() or close_module().\nMaybe it's not Kadu-compatible Module."));
			return false;
		}
		Modules.insert(module_name,m);
	}
	else
	{
		MessageBox::msg(tr("Cannot load module library.\nMaybe it's incorrecty compiled."));
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

void ModulesManager::saveLoadedModules()
{
	config_file.writeEntry("General", "LoadedModules",loadedModules().join(","));
	config_file.sync();
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
	setCaption(tr("Manage Modules"));
	
	QHBoxLayout* layout= new QHBoxLayout(this);
	QVBoxLayout* installed_layout=new QVBoxLayout(layout);
	QVBoxLayout* loaded_layout=new QVBoxLayout(layout);	
		
	QLabel* InstalledLabel = new QLabel(this);
	InstalledLabel->setText(QString("<b>")+tr("Installed modules")+"</b>");
	
	InstalledListBox = new QListBox(this);
	InstalledListBox->insertStringList(modules_manager->unloadedModules());
	connect(InstalledListBox,SIGNAL(doubleClicked(QListBoxItem*)),
		this,SLOT(loadItem(QListBoxItem*)));
	
	QButton* LoadButton=new QPushButton(this);
	LoadButton->setText(tr("Load"));
	connect(LoadButton,SIGNAL(clicked()),
		this,SLOT(loadSelectedItem()));
	
	installed_layout->addWidget(InstalledLabel);
	installed_layout->addWidget(InstalledListBox);
	installed_layout->addWidget(LoadButton);

	QLabel* LoadedLabel = new QLabel(this);
	LoadedLabel->setText(QString("<b>")+tr("Loaded modules")+"</b>");

	LoadedListBox = new QListBox(this);
	LoadedListBox->insertStringList(modules_manager->loadedModules());	
	connect(LoadedListBox,SIGNAL(doubleClicked(QListBoxItem*)),
		this,SLOT(unloadItem(QListBoxItem*)));

	QButton* UnloadButton=new QPushButton(this);
	UnloadButton->setText(tr("Unload"));
	connect(UnloadButton,SIGNAL(clicked()),
		this,SLOT(unloadSelectedItem()));

	loaded_layout->addWidget(LoadedLabel);
	loaded_layout->addWidget(LoadedListBox);
	loaded_layout->addWidget(UnloadButton);	
}

void ModulesDialog::loadItem(QListBoxItem* item)
{
	QString mod_name=item->text();
	if(modules_manager->loadModule(mod_name))
	{
		LoadedListBox->insertItem(mod_name);
		InstalledListBox->removeItem(InstalledListBox->currentItem());
		modules_manager->saveLoadedModules();
	}		
}

void ModulesDialog::unloadItem(QListBoxItem* item)
{
	QString mod_name=item->text();
	modules_manager->unloadModule(mod_name);
	InstalledListBox->insertItem(mod_name);
	LoadedListBox->removeItem(LoadedListBox->currentItem());
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
