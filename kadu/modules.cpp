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

void ModulesManager::initModule()
{
	modules_manager=new ModulesManager();
}

ModulesManager::ModulesManager() : QObject()
{
//	QStringList modules=installedModules();
	loadModule("autoresponder");
}

QStringList ModulesManager::installedModules()
{
	QDir dir(QString(DATADIR)+"/kadu/modules");
	dir.setFilter(QDir::Files);
	return dir.entryList();
}

bool ModulesManager::loadModule(const QString& file_name)
{
	ModuleLib=new QLibrary(QString(DATADIR)+"/kadu/modules/"+file_name);
	if(ModuleLib->load())
	{
		typedef void InitModuleFunc();
		InitModuleFunc* init=(InitModuleFunc*)ModuleLib->resolve("init_module");
		if(init!=NULL)
			init();
		else
		{
			fprintf(stderr,"Cannot find init_module() in loaded module\n");
			return false;
		}
	}
	else
	{
		fprintf(stderr,"Cannot load module\n");	
	}
}

ModulesManager* modules_manager;
