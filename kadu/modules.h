#ifndef MODULES_H
#define MODULES_H

#include <qstring.h>
#include <qobject.h>
#include <qlibrary.h>
#include <qstringlist.h>

class ModulesManager : public QObject
{
	Q_OBJECT

	private:
		QLibrary* ModuleLib;

	public:	
		static void initModule();
		ModulesManager();
		QStringList installedModules();
		bool loadModule(const QString& file_name);
};

extern ModulesManager* modules_manager;

#endif
