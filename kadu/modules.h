#ifndef MODULES_H
#define MODULES_H

#include <qstring.h>
#include <qobject.h>
#include <qlibrary.h>
#include <qstringlist.h>
#include <qdialog.h>
#include <qlistbox.h>

class ModulesManager : public QObject
{
	Q_OBJECT

	private:
		typedef void CloseModuleFunc();
		struct Module
		{
			QLibrary* lib;
			CloseModuleFunc* close;
		};
		QMap<QString,Module> Modules;	

	public:	
		static void initModule();
		ModulesManager();
		QStringList installedModules();
		QStringList loadedModules();
		QStringList unloadedModules();
		bool loadModule(const QString& module_name);
		void unloadModule(const QString& module_name);
		
	public slots:
		void showDialog();
};

extern ModulesManager* modules_manager;

class ModulesDialog : public QDialog
{
	Q_OBJECT
	
	private:
		QListBox* InstalledListBox;
		QListBox* LoadedListBox;
		
	private slots:
		void loadItem(QListBoxItem* item);
		void unloadItem(QListBoxItem* item);
		void loadSelectedItem();
		void unloadSelectedItem();
	
	public:
		ModulesDialog();
};

#endif
