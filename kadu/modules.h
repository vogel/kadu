#ifndef MODULES_H
#define MODULES_H

#include <qstring.h>
#include <qobject.h>
#include <qstringlist.h>
#include <qdialog.h>
#include <qlistbox.h>
#include <qtranslator.h>

/**
	Zastêpuje klasê QLibrary na specyficzne potrzeby Kadu
**/
class Library
{
	private:
		QString FileName;
		void* Handle;
		
	public:
		Library(const QString& file_name);
		~Library();
		bool load();
		void* resolve(const QString& symbol_name);
		QString error();
};

/**
 informacje o module
**/
struct ModuleInfo
{
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
			Library* lib;
			CloseModuleFunc* close;
			QTranslator* translator;
			ModuleInfo info;
			int usage_counter;
		};
		QMap<QString,Module> Modules;	
		ModulesDialog* Dialog;
		void initStaticModules();
		void closeStaticModules();
		/**
			£aduje plik z t³umaczeniem. Zwraca NULL je¶li wyst±pi³
			b³±d.
		**/
		QTranslator* loadModuleTranslation(const QString& module_name);
		/**
			£aduje pliki t³umaczeñ dla wszystkich wkompilowanych
			modu³ów.
		**/
		void loadStaticModulesTranslations();

	private slots:
		void dialogDestroyed();
	
	public:	
		static void initModule();
		static void closeModule();
		ModulesManager();
		~ModulesManager();
		/**
			Zwraca listê modu³ów wkompilowanych
			statycznie w plik wykonywalny kadu
		**/
		QStringList staticModules();
		/**
			Zwraca listê modu³ów zainstalowanych jako
			dzielone biblioteki (shared libraries)
		**/
		QStringList installedModules();
		/**
			Zwraca listê modu³ów za³adowanych do pamiêci
			jako dzielone biblioteki (shared libraries)
		**/
		QStringList loadedModules();
		/**
			Zwraca listê modu³ów zainstalowanych jako
			dzielone biblioteki (shared libraries)
			i nie za³adowanych aktualnie do pamiêci
		**/		
		QStringList unloadedModules();
		/**
			Pobiera do info informacje o module module_name jesli
			sie to udalo zwraca true w przeciwnym wypadku false		 
		**/		
		bool moduleInfo(const QString& module_name, ModuleInfo& info);
		/**
			£aduje modu³ do pamiêci i inicjalizuje go
		**/
		bool loadModule(const QString& module_name);
		/**
			Zamyka modu³ i usuwa go z pamiêci
		**/		
		bool unloadModule(const QString& module_name, bool force=false);
		/**
			Sprawdza czy podany modu³ jest wkompilowany statycznie.
			@param module_name nazwa modulu
		**/
		bool moduleIsStatic(const QString& module_name);
		/**
			Sprawdza czy podany modu³ jest zainstalowany
			w katalogu z modu³ami zewnêtrznymi.
			@param module_name nazwa modulu
		**/
		bool moduleIsInstalled(const QString& module_name);
		/**
			Sprawdza czy podany modu³ zewnêtrzny jest za³adowany
			@param module_name nazwa modulu
		**/		
		bool moduleIsLoaded(const QString& module_name);
		/**
			Sprawdza czy podany modu³ jest aktywny.
			Dzia³a dla modu³ów statycznych i za³adowanych
			zewnêtrznych.
			@param module_name nazwa modulu
		**/
		bool moduleIsActive(const QString& module_name);
		/**
			Zapisuje do pliku konfiguracyjnego listê za³adowanych
			modu³ów.
		**/
		void saveLoadedModules();
		
	public slots:
		void showDialog();
};

extern ModulesManager* modules_manager;

#endif
