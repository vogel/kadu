#ifndef MODULES_H
#define MODULES_H

#include <qstring.h>
#include <qobject.h>
#include <qlibrary.h>
#include <qstringlist.h>
#include <qdialog.h>
#include <qlistbox.h>
#include <qtranslator.h>
/**
 informacje o module
**/
struct ModuleInfo{
	/**
	 jakie inne moduly sa wymagane przez ten modul
	**/
	QStringList depends;
	/**
	 opis
	**/
	QString description;
	/**
	 autor modulu
	**/
	QString author;
};

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
		void getInfo();
		void refreshLists();
	
	public:
		ModulesDialog();
};

class ModulesManager : public QObject
{
	Q_OBJECT

	private:
		typedef void CloseModuleFunc();
		struct Module
		{
			QLibrary* lib;
			CloseModuleFunc* close;
			QTranslator* translator;
			ModuleInfo info;
			int usage_counter;
		};
		QMap<QString,Module> Modules;	
		ModulesDialog* Dialog;

	private slots:
		void dialogDestroyed();
	
	public:	
		static void initModule();
		static void closeModule();
		ModulesManager();
		~ModulesManager();
		QStringList installedModules();
		QStringList loadedModules();
		QStringList unloadedModules();
		/**
			Pobiera do info informacje o module module_name jesli
			sie to udalo zwraca true w przeciwnym wypadku false		 
		**/		
		bool moduleInfo(const QString& module_name, ModuleInfo& info);
		bool loadModule(const QString& module_name);
		bool unloadModule(const QString& module_name, bool force=false);
		void saveLoadedModules();
		
	public slots:
		void showDialog();
};

extern ModulesManager* modules_manager;

#endif
