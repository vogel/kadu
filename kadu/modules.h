#ifndef MODULES_H
#define MODULES_H

#include <qstring.h>
#include <qobject.h>
#include <qstringlist.h>
#include <qdialog.h>
#include <qlistview.h>
#include <qtranslator.h>
#include <qlabel.h>
#include <qhbox.h>

/**
	Zastêpuje klasê QLibrary na specyficzne potrzeby Kadu
**/
class Library : public QObject
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
	 jakie inne modu³y s± wymagane przez ten modu³
	**/
	QStringList depends;
	/**
	 z jakimi modu³ami ten modu³ konfliktuje
	**/
	QStringList conflicts;
	/**
	 jak± cechê dostarcza ten modu³
	**/
	QStringList provides;
	/**
	 opis
	**/
	QString description;
	/**
	 autor modu³u
	**/
	QString author;
};

class ModulesDialog : public QHBox
{
	Q_OBJECT
	
	private:
		QListView* lv_modules;
		QLabel* l_moduleinfo;
		
	private slots:
		void loadItem();
		void unloadItem();
		void moduleAction(QListViewItem *);
		void itemsChanging();
		void getInfo();
		void refreshList();
		void keyPressEvent(QKeyEvent *);
	
	public:
		ModulesDialog();
		~ModulesDialog();
};

class ModulesManager : public QObject
{
	Q_OBJECT

	private:
		typedef int InitModuleFunc();
		typedef void CloseModuleFunc();
		/**
			Informacje o statycznym module.
		**/
		struct StaticModule
		{
			InitModuleFunc* init;
			CloseModuleFunc* close;
		};
		/**
			Lista statycznych modu³ów wype³niania
			przez kod generowany przez configure.
		**/
		QMap<QString,StaticModule> StaticModules;
		/**
			Informacje o aktywnym module
			statycznym b±d¼ zewnêtrznym.
			Dla modu³u statycznego lib==NULL.
		**/
		struct Module
		{
			Library* lib;
			CloseModuleFunc* close;
			QTranslator* translator;
			ModuleInfo info;
			int usage_counter;
		};
		/**
			Lista aktywnych modu³ów
			statycznych b±d¼ zewnêtrznych.
		**/
		QMap<QString,Module> Modules;	
		/**
		**/
		ModulesDialog* Dialog;
		/**
			£aduje plik z t³umaczeniem. Zwraca NULL je¶li wyst±pi³
			b³±d.
		**/
		QTranslator* loadModuleTranslation(const QString& module_name);
		/**
			Sprawdza czy dostêpne s± modu³y z listy
			zale¿no¶ci danego modu³u. W razie czego
			stara siê je za³adowaæ je¶li s± dostêpne.
			@param module_info informacje o module
		**/
		bool satisfyModuleDependencies(const ModuleInfo& module_info);
		/**
			Zwiêksza liczniki u¿ycia modu³ów u¿ywanych
			przez dany modu³.
			@param module_info informacje o module
		**/
		void incDependenciesUsageCount(const ModuleInfo& module_info);
		/**
			Rejestruje modu³ statyczny. Funcja wywo³ywana
			dla wszystkich modu³ów statycznych przez kod
			wygenerowany przez configure.
		**/
		void registerStaticModule(const QString& module_name,
			InitModuleFunc* init,CloseModuleFunc* close);
		/**
			Rejestruje modu³y statyczne. Kod funkcji jest
			generowany przez configure.
		**/
		void registerStaticModules();

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
		QStringList staticModules() const;
		/**
			Zwraca listê modu³ów zainstalowanych jako
			dzielone biblioteki (shared libraries)
		**/
		QStringList installedModules() const;
		/**
			Zwraca listê modu³ów za³adowanych do pamiêci
			jako dzielone biblioteki (shared libraries)
		**/
		QStringList loadedModules() const;
		/**
			Zwraca listê modu³ów zainstalowanych jako
			dzielone biblioteki (shared libraries)
			i nie za³adowanych aktualnie do pamiêci
		**/		
		QStringList unloadedModules() const;
		/**
			Zwraca listê aktywnych modu³ów.
			Uwzglêdniane s± zarówno aktywne modu³y
			statyczne jak i zewnêtrzne.
		**/
		QStringList activeModules() const;
		/**
			Pobiera do info informacje o module module_name jesli
			sie to udalo zwraca true w przeciwnym wypadku false		 
		**/		
		bool moduleInfo(const QString& module_name, ModuleInfo& info) const;
		/**
			Sprawdza czy podany modu³ jest wkompilowany statycznie.
			@param module_name nazwa modu³u
		**/
		bool moduleIsStatic(const QString& module_name) const;
		/**
			Sprawdza czy podany modu³ jest zainstalowany
			w katalogu z modu³ami zewnêtrznymi.
			@param module_name nazwa modu³u
		**/
		bool moduleIsInstalled(const QString& module_name) const;
		/**
			Sprawdza czy podany modu³ zewnêtrzny jest za³adowany
			@param module_name nazwa modu³u
		**/		
		bool moduleIsLoaded(const QString& module_name) const;
		/**
			Sprawdza czy podany modu³ jest aktywny.
			Dzia³a dla modu³ów statycznych i za³adowanych
			zewnêtrznych.
			@param module_name nazwa modu³u
		**/
		bool moduleIsActive(const QString& module_name) const;
		/**
			Sprawdza czy podany modu³ konfliktuje
			z jakim¶ innym za³adowanym modu³em
			@param module_name nazwa modu³u
			@param module_info informacje o module
		**/
		bool conflictsWithLoaded(const QString &module_name, const ModuleInfo& module_info) const;
		
	public slots:
		/**
			Aktywuje modu³ statyczny je¶li jest dostêpny
			lub ³aduje do pamiêci i aktywuje modu³ zewnêtrzny.
			@param module_name nazwa modu³u
		**/
		bool activateModule(const QString& module_name);
		/**
			Deaktywuje modu³ statyczny lub
			deaktywuje i usuwa z pamiêci modu³ zewnêtrzny.
			@param module_name nazwa modu³u
		**/		
		bool deactivateModule(const QString& module_name, bool force=false);
		void showDialog();
		/**
		 	Zwiêksza licznik u¿ycia modu³u o 1
			@param module_name nazwa modu³u
		**/
		void moduleIncUsageCount(const QString& module_name);
		/**
		 	Zmniejsza licznik u¿ycia modu³u o 1
			@param module_name nazwa modu³u
		**/
		void moduleDecUsageCount(const QString& module_name);
		/**
			Zapisuje do pliku konfiguracyjnego listê za³adowanych
			modu³ów.
		**/
		void saveLoadedModules();
};

extern ModulesManager* modules_manager;

#endif
