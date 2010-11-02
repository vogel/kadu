/*
 * %kadu copyright begin%
 * Copyright 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2003, 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2004, 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KADU_MODULES_H
#define KADU_MODULES_H

#include <QtCore/QMap>
#include <QtGui/QWidget>

#include "exports.h"

class QCheckBox;
class QLabel;
class QTranslator;
class QTreeWidget;
class QTreeWidgetItem;

class ModulesWindow;

#ifndef Q_OS_WIN
/**
	Zast�puje klas� QLibrary na specyficzne potrzeby Kadu.
	\class Library
	\brief Biblioteki dzielone.
**/
class Library : public QObject
{
	Q_OBJECT

	QString FileName;
	void *Handle;

public:
	/**
		\fn Library(const QString &file_name)
		Konstruktor przydzielaj�cy dany plik dla tego obiektu biblioteki dzielonej.
		\param file_name nazwa pliku biblioteki dzielonej.
	**/
	Library(const QString &file_name);
	~Library();

	/**
		\fn bool load()
		�aduje przydzielon� bibliotek� dzielon� do pami�ci.
	**/
	bool load();

	/**
		\fn void * resolve(const QString &symbol_name)
		T�umaczy nazw� symbolu na funkcj� z za�adowanej biblioteki dzielonej.
		\param symbol_name nazwa symbolu do przet�umaczenia.
		\return wska�nik do przt�umaczonego symbolu.
	**/
	void * resolve(const QString &symbol_name);

	/**
		\fn QString error()
		\return tre�� b��du, jaki wyst�pi� podczas �adowanie biblioteki dzielonej.
	**/
	QString errorString();

};
#else
#include <QtCore/QLibrary>
#define Library QLibrary
#endif

/**
	\struct ModuleInfo
	\brief Informacje o module.
**/
struct KADUAPI ModuleInfo
{
	QStringList depends; /*!< Jakie inne modu�y s� wymagane przez ten modu�. */
	QStringList conflicts; /*!< Z jakimi modu�ami ten modu� konfliktuje. */
	QStringList provides; /*!< Jak� cech� dostarcza ten modu�. */
	QStringList replaces;
	QString description; /*!< Opis modu�u. */
	QString author; /*!< Autor modu�u. */
	QString version; /*!< Wersja modu�u. */
	bool load_by_def; /*!< Czy modu� jest domy�lnie �adowany, czy nie? */
	bool base; /*!< Czy modu� nale�y do modu��w podstawowych? */
	ModuleInfo();
};

/**
	Ta klasa odpowiada za obs�ug� modu��w Kadu.
	\class ModulesManager
	\brief Zarz�dzanie modu�ami
**/
class KADUAPI ModulesManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ModulesManager)

	ModulesManager();

	static ModulesManager *Instance;

	typedef int InitModuleFunc(bool);
	typedef void CloseModuleFunc(void);

	/**
		\struct StaticModule
		\brief Informacje o statycznym module.
	**/
	struct StaticModule
	{
		InitModuleFunc *init; /*!< Wska�nik do funkcji inicjalizuj�cej modu�. */
		CloseModuleFunc *close; /*!< Wska�nik do funkcji deinicjalizuj�cej modu�. */
	};

	/**
		Lista statycznych modu��w wype�niania
		przez kod generowany przez configure.
	**/
	QMap<QString, StaticModule> StaticModules;

	/**
		Informacje o aktywnym module
		statycznym b�d� zewn�trznym.
		Dla modu�u statycznego lib==NULL.
		\struct Module
		\brief Informacje o aktywnym module.
	**/
	struct Module
	{
		Library *lib; /*!< Wska�nik do obiektu biblioteki dzielonej. */
		CloseModuleFunc *close; /*!< Wska�nik do funkcji deinicjalizuj�cej modu�. */
		QTranslator *translator; /*!< Wska�nik do obiektu t�umacz�cego dla tego modu�u. */
		ModuleInfo info; /*!< Informacje o module. */
		int usage_counter; /*!< Licznik u�ycia modu�u. */
		Module();
	};

	/**
		Lista aktywnych modu��w
		statycznych b�d� zewn�trznych.
	**/
	QMap<QString, Module> Modules;

	/**
		List of modules that were loaded in the past.
	 **/
	QStringList everLoaded;

	QStringList protocolModulesList;
	QStringList installed_list;
	QStringList loaded_list;
	QStringList unloaded_list;

	ModulesWindow *Window;

	/**
		�aduje plik z t�umaczeniem. Zwraca NULL je�li wyst�pi�
		b��d.
	**/
	QTranslator *loadModuleTranslation(const QString &module_name);

	/**
		Sprawdza czy dost�pne s� modu�y z listy
		zale�no�ci danego modu�u. W razie czego
		stara si� je za�adowa� je�li s� dost�pne.
		@param module_info informacje o module
	**/
	bool satisfyModuleDependencies(const ModuleInfo &module_info);

	/**
		Zwi�ksza liczniki u�ycia modu��w u�ywanych
		przez dany modu�.
		@param module_info informacje o module
	**/

	void incDependenciesUsageCount(const ModuleInfo &module_info);
	/**
		Rejestruje modu� statyczny. Funcja wywo�ywana
		dla wszystkich modu��w statycznych przez kod
		wygenerowany przez configure.
	**/

	void registerStaticModule(const QString &module_name, InitModuleFunc *init, CloseModuleFunc *close);

	/**
		Rejestruje modu�y statyczne. Kod funkcji jest
		generowany przez configure.
	**/
	void registerStaticModules();

	QStringList protocolModules() const;

	/**
		Skupia wszystkie t�umaczenia w jednej hierarchii
	**/
	QObject *translators;

private slots:
	void dialogDestroyed();

public:

	void loadProtocolModules();

	void loadAllModules();

	void unloadAllModules();

	static ModulesManager * instance();

	~ModulesManager();

	/**
		\fn QStringList staticModules() const
		\return list� modu��w wkompilowanych
		statycznie w plik wykonywalny Kadu.
	**/
	QStringList staticModules() const;

	/**
		\fn QStringList installedModules() const
		\return list� modu��w zainstalowanych jako
		dzielone biblioteki (shared libraries).
	**/
	QStringList installedModules() const;

	/**
		\fn QStringList loadedModules() const
		\return list� modu��w za�adowanych do pami�ci
		jako dzielone biblioteki (shared libraries).
	**/
	QStringList loadedModules() const;

	/**
		\fn QStringList unloadedModules() const
		\return list� modu��w zainstalowanych jako
		dzielone biblioteki (shared libraries)
		i nie za�adowanych aktualnie do pami�ci.
	**/
	QStringList unloadedModules() const;

	/**
		\fn QStringList activeModules() const
		\return list� aktywnych modu��w.
		Uwzgl�dniane s� zar�wno aktywne modu�y
		statyczne jak i zewn�trzne.
	**/
	QStringList activeModules() const;

	QString moduleProvides(const QString &provides);

	/**
		\fn bool moduleInfo(const QString &module_name, ModuleInfo &info) const
		Pobiera do info informacje o danym module.
		\param[in] module_name nazwa modu�u, dla kt�rego nale�y pobra� informacj�.
		\param[out] info struktura, w kt�rej te informacje nale�y umie�ci�.
		\return true, je�li si� uda�o, w przeciwnym wypadku false.
	**/
	bool moduleInfo(const QString &module_name, ModuleInfo &info) const;

	/**
		\fn bool moduleIsStatic(const QString &module_name) const
		Sprawdza czy podany modu� jest wkompilowany statycznie.
		\param module_name nazwa modu�u.
		\return true, je�li modu� jest wkompilowany, false je�li nie jest.
	**/
	bool moduleIsStatic(const QString &module_name) const;

	/**
		\fn bool moduleIsInstalled(const QString &module_name) const
		Sprawdza czy podany modu� jest zainstalowany
		w katalogu z modu�ami zewn�trznymi.
		\param module_name nazwa modu�u.
		\return true, je�li modu� jest zainstalowany, false je�li nie jest.
	**/
	bool moduleIsInstalled(const QString &module_name) const;

	/**
		\fn bool moduleIsLoaded(const QString &module_name) const
		Sprawdza czy podany modu� zewn�trzny jest za�adowany.
		\param module_name nazwa modu�u.
		\return true, je�li modu� jest za�adowany, false je�li nie jest.
	**/
	bool moduleIsLoaded(const QString &module_name) const;

	/**
		\fn bool moduleIsActive(const QString &module_name) const
		Sprawdza czy podany modu� jest aktywny.
		Dzia�a dla modu��w statycznych i za�adowanych
		zewn�trznych.
		\param module_name nazwa modu�u.
		\return true, je�li modu� jest aktywny, false je�li nie jest.
	**/
	bool moduleIsActive(const QString &module_name) const;

	/**
		\fn bool conflictsWithLoaded(const QString &module_name, const ModuleInfo &module_info) const
		Sprawdza czy podany modu� konfliktuje
		z jakim� innym za�adowanym modu�em.
		\param module_name nazwa modu�u.
		\param module_info informacje o module.
		\return true, je�li modu� konfliktuje, false je�li nie.
	**/
	bool conflictsWithLoaded(const QString &module_name, const ModuleInfo &module_info) const;

public slots:
	/**
		\fn bool activateModule(const QString &module_name)
		Aktywuje modu� statyczny je�li jest dost�pny
		lub �aduje do pami�ci i aktywuje modu� zewn�trzny.
		Przez aktywacje rozumie si� wywo�anie funkcji *_init z modu�u.
		\param module_name nazwa modu�u.
		\return true je�li aktywacja przebieg�a bezproblemowo, false w przeciwnym wypadku.
	**/
	bool activateModule(const QString &module_name);

	/**
		\fn bool deactivateModule(const QString &module_name, bool force=false)
		Deaktywuje modu� statyczny lub deaktywuje i usuwa z pami�ci modu� zewn�trzny.
		\param module_name nazwa modu�u.
		\return true je�li dezaktywacja przebieg�a bezproblemowo, false w przeciwnym wypadku.
	**/
	bool deactivateModule(const QString &module_name, bool force = false);

	/**
		\fn void showWindow(QAction *sender, bool toggled)
		Wy�wietla okno dialogowe "Zarz�dcy modu��w", czyli tworzy i pokazuje klas� ModulesDialog.
	**/
	void showWindow(QAction *sender, bool toggled);

	/**
		\fn void moduleIncUsageCount(const QString &module_name)
		Zwi�ksza licznik u�ycia modu�u o 1.
		\param module_name nazwa modu�u.
	**/
	void moduleIncUsageCount(const QString &module_name);

	/**
		\fn void moduleDecUsageCount(const QString &module_name)
	 	Zmniejsza licznik u�ycia modu�u o 1.
		\param module_name nazwa modu�u.
	**/
	void moduleDecUsageCount(const QString &module_name);

	/**
		\fn void saveLoadedModules()
		Zapisuje do pliku konfiguracyjnego list� za�adowanych
		modu��w.
	**/
	void saveLoadedModules();

};

#endif
