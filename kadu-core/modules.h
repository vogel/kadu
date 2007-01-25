#ifndef KADU_MODULES_H
#define KADU_MODULES_H

#include <qhbox.h>
#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>

class QCheckBox;
class QLabel;
class QListView;
class QListViewItem;
class QTranslator;

/**
	Zastêpuje klasê QLibrary na specyficzne potrzeby Kadu.
	\class Library
	\brief Biblioteki dzielone.
**/
class Library : public QObject
{
	private:
		QString FileName;
		void* Handle;

	public:
		/**
			\fn Library(const QString& file_name)
			Konstruktor przydzielaj±cy dany plik dla tego obiektu biblioteki dzielonej.
			\param file_name nazwa pliku biblioteki dzielonej.
		**/
		Library(const QString& file_name);
		~Library();

		/**
			\fn bool load()
			£aduje przydzielon± bibliotekê dzielon± do pamiêci.
		**/
		bool load();

		/**
			\fn void* resolve(const QString& symbol_name)
			T³umaczy nazwê symbolu na funkcjê z za³adowanej biblioteki dzielonej.
			\param symbol_name nazwa symbolu do przet³umaczenia.
			\return wska¼nik do przt³umaczonego symbolu.
		**/
		void* resolve(const QString& symbol_name);

		/**
			\fn QString error()
			\return tre¶æ b³êdu, jaki wyst±pi³ podczas ³adowanie biblioteki dzielonej.
		**/
		QString error();
};

/**
	\struct ModuleInfo
	\brief Informacje o module.
**/
struct ModuleInfo
{
	QStringList depends; /*!< Jakie inne modu³y s± wymagane przez ten modu³. */
	QStringList conflicts; /*!< Z jakimi modu³ami ten modu³ konfliktuje. */
	QStringList provides; /*!< Jak± cechê dostarcza ten modu³. */
	QString description; /*!< Opis modu³u. */
	QString author; /*!< Autor modu³u. */
	QString version; /*!< Wersja modu³u. */
	bool load_by_def; /*!< Czy modu³ jest domy¶lnie ³adowany, czy nie? */
	bool base; /*!< Czy modu³ nale¿y do modu³ów podstawowych? */
	ModuleInfo();
};

class LayoutHelper;
/**
	Klasa reprezentuj±ca okno dialogowe, s³u¿±ce do ³adowanie i wy³adowywania modu³ów.
	£adowanie/wy³adowywanie, oraz inne operacje na modu³ach z poziomu C++ dokonywane
	s± za pomoc± klasy ModulesManager. Ta klasa tworzy jedynie okno dialogowe, bêd±ce
	interfejsem do ww. klasy dla u¿ytkownika Kadu.
	\class ModulesDialog
	\brief "Zarz±dca modu³ów"
**/
class ModulesDialog : public QHBox
{
	Q_OBJECT

	private:
		QListView* lv_modules;
		QLabel* l_moduleinfo;
		LayoutHelper *layoutHelper;
		QCheckBox *hideBaseModules;

	private slots:
		void loadItem();
		void unloadItem();
		void moduleAction(QListViewItem *);
		void itemsChanging();
		void getInfo();
		void refreshList();
		void keyPressEvent(QKeyEvent *);

	public:
		/**
			\fn ModulesDialog()
			Konstruktor standardowy.
		**/
		ModulesDialog();
		~ModulesDialog();
	protected:
		virtual void resizeEvent(QResizeEvent *);
};

/**
	Ta klasa odpowiada za obs³ugê modu³ów Kadu.
	\class ModulesManager
	\brief Zarz±dzanie modu³ami
**/
class ModulesManager : public QObject
{
	Q_OBJECT

	private:
		typedef int InitModuleFunc();
		typedef void CloseModuleFunc();
		/**
			\struct StaticModule
			\brief Informacje o statycznym module.
		**/
		struct StaticModule
		{
			InitModuleFunc* init; /*!< Wska¼nik do funkcji inicjalizuj±cej modu³. */
			CloseModuleFunc* close; /*!< Wska¼nik do funkcji deinicjalizuj±cej modu³. */
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
			\struct Module
			\brief Informacje o aktywnym module.
		**/
		struct Module
		{
			Library* lib; /*!< Wska¼nik do obiektu biblioteki dzielonej. */
			CloseModuleFunc* close; /*!< Wska¼nik do funkcji deinicjalizuj±cej modu³. */
			QTranslator* translator; /*!< Wska¼nik do obiektu t³umacz±cego dla tego modu³u. */
			ModuleInfo info; /*!< Informacje o module. */
			int usage_counter; /*!< Licznik u¿ycia modu³u. */
			Module();
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

		/**
			Skupia wszystkie t³umaczenia w jednej hierarchii
		**/
		QObject *translators;

	private slots:
		void dialogDestroyed();

	public:
		/**
			\fn static void initModule()
			Inicjalizuje obs³ugê modu³ów. Metoda ta wywo³ywana jest przy starcie Kadu, przez jego rdzeñ.
		**/
		static void initModule();

		/**
			\fn static void closeModule()
			Deinicjalizuje obs³ugê modu³ów. Metoda ta jest wywo³ywana przy zamykaniu Kadu, przez jego rdzeñ.
		**/
		static void closeModule();

		/**
			\fn ModulesManager()
			Standardowy konstruktor.
		**/
		ModulesManager();
		~ModulesManager();

		/**
			\fn QStringList staticModules() const
			\return listê modu³ów wkompilowanych
			statycznie w plik wykonywalny Kadu.
		**/
		QStringList staticModules() const;

		/**
			\fn QStringList installedModules() const
			\return listê modu³ów zainstalowanych jako
			dzielone biblioteki (shared libraries).
		**/
		QStringList installedModules() const;

		/**
			\fn QStringList loadedModules() const
			\return listê modu³ów za³adowanych do pamiêci
			jako dzielone biblioteki (shared libraries).
		**/
		QStringList loadedModules() const;

		/**
			\fn QStringList unloadedModules() const
			\return listê modu³ów zainstalowanych jako
			dzielone biblioteki (shared libraries)
			i nie za³adowanych aktualnie do pamiêci.
		**/
		QStringList unloadedModules() const;

		/**
			\fn QStringList activeModules() const
			\return listê aktywnych modu³ów.
			Uwzglêdniane s± zarówno aktywne modu³y
			statyczne jak i zewnêtrzne.
		**/
		QStringList activeModules() const;

		/**
			\fn bool moduleInfo(const QString& module_name, ModuleInfo& info) const
			Pobiera do info informacje o danym module.
			\param[in] module_name nazwa modu³u, dla którego nale¿y pobraæ informacjê.
			\param[out] info struktura, w której te informacje nale¿y umie¶ciæ.
			\return true, je¶li siê uda³o, w przeciwnym wypadku false.
		**/
		bool moduleInfo(const QString& module_name, ModuleInfo& info) const;

		/**
			\fn bool moduleIsStatic(const QString& module_name) const
			Sprawdza czy podany modu³ jest wkompilowany statycznie.
			\param module_name nazwa modu³u.
			\return true, je¶li modu³ jest wkompilowany, false je¶li nie jest.
		**/
		bool moduleIsStatic(const QString& module_name) const;

		/**
			\fn bool moduleIsInstalled(const QString& module_name) const
			Sprawdza czy podany modu³ jest zainstalowany
			w katalogu z modu³ami zewnêtrznymi.
			\param module_name nazwa modu³u.
			\return true, je¶li modu³ jest zainstalowany, false je¶li nie jest.
		**/
		bool moduleIsInstalled(const QString& module_name) const;

		/**
			\fn bool moduleIsLoaded(const QString& module_name) const
			Sprawdza czy podany modu³ zewnêtrzny jest za³adowany.
			\param module_name nazwa modu³u.
			\return true, je¶li modu³ jest za³adowany, false je¶li nie jest.
		**/
		bool moduleIsLoaded(const QString& module_name) const;

		/**
			\fn bool moduleIsActive(const QString& module_name) const
			Sprawdza czy podany modu³ jest aktywny.
			Dzia³a dla modu³ów statycznych i za³adowanych
			zewnêtrznych.
			\param module_name nazwa modu³u.
			\return true, je¶li modu³ jest aktywny, false je¶li nie jest.
		**/
		bool moduleIsActive(const QString& module_name) const;

		/**
			\fn bool conflictsWithLoaded(const QString &module_name, const ModuleInfo& module_info) const
			Sprawdza czy podany modu³ konfliktuje
			z jakim¶ innym za³adowanym modu³em.
			\param module_name nazwa modu³u.
			\param module_info informacje o module.
			\return true, je¶li modu³ konfliktuje, false je¶li nie.
		**/
		bool conflictsWithLoaded(const QString &module_name, const ModuleInfo& module_info) const;

	public slots:
		/**
			\fn bool activateModule(const QString& module_name)
			Aktywuje modu³ statyczny je¶li jest dostêpny
			lub ³aduje do pamiêci i aktywuje modu³ zewnêtrzny.
			Przez aktywacje rozumie siê wywo³anie funkcji *_init z modu³u.
			\param module_name nazwa modu³u.
			\return true je¶li aktywacja przebieg³a bezproblemowo, false w przeciwnym wypadku.
		**/
		bool activateModule(const QString& module_name);

		/**
			\fn bool deactivateModule(const QString& module_name, bool force=false)
			Deaktywuje modu³ statyczny lub deaktywuje i usuwa z pamiêci modu³ zewnêtrzny.
			\param module_name nazwa modu³u.
			\return true je¶li dezaktywacja przebieg³a bezproblemowo, false w przeciwnym wypadku.
		**/
		bool deactivateModule(const QString& module_name, bool force=false);

		/**
			\fn void showDialog()
			Wy¶wietla okno dialogowe "Zarz±dcy modu³ów", czyli tworzy i pokazuje klasê ModulesDialog.
		**/
		void showDialog();

		/**
			\fn void moduleIncUsageCount(const QString& module_name)
			Zwiêksza licznik u¿ycia modu³u o 1.
			\param module_name nazwa modu³u.
		**/
		void moduleIncUsageCount(const QString& module_name);

		/**
			\fn void moduleDecUsageCount(const QString& module_name)
		 	Zmniejsza licznik u¿ycia modu³u o 1.
			\param module_name nazwa modu³u.
		**/
		void moduleDecUsageCount(const QString& module_name);

		/**
			\fn void saveLoadedModules()
			Zapisuje do pliku konfiguracyjnego listê za³adowanych
			modu³ów.
		**/
		void saveLoadedModules();
};

extern ModulesManager* modules_manager;

#endif
