/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005, 2007 Marcin Ślusarz (joi@kadu.net)
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

#ifndef KADU_THEMES_H
#define KADU_THEMES_H

#include <QtCore/QMap>
#include <QtCore/QStringList>

#include "exports.h"

/**
	Klasa obs�uguj�ca r��ne zestawy (jak ikony i emotikony)
	\class Themes
	\brief Klasa zarz�dzaj�ca zestawami
**/

class KADUAPI Themes : public QObject
{
	Q_OBJECT

	QStringList ThemesList; //!< lista z nazwami dost�pnych zestaw�w
	QStringList ThemesPaths; //!< lista �cie�ek w kt�rych szukamy zestaw�w
	QStringList additional;
	QString ConfigName; //!< nazwa pliku z konfiguracj� zestawu
	QString Name; //!< typ zestawu (np. "icons", "emoticons")
	QString ActualTheme; //!< nazwa aktualnie wybranego zestawu
	QMap<QString, QString> entries;

	/**
		\fn QStringList getSubDirs(const QString &path, bool validate = true) const
		Wyszukuje podkatalogi w podanej �cie�ce i zwraca list� tych zawieraj�cych
		pliki konfiguracyjne zestawu. Je�li parametr validate ma warto�� false,
		zwr�cona zostanie lista wszystkich znalezionych podkatalog�w.
		\param path �cie�ka do katalogu
		\param validate sprawdzenie zawarto�ci
		\return lista podkatalog�w
	**/
	QStringList getSubDirs(const QString &path, bool validate = true) const;

	/**
		\fn bool validateDir(const QString &path) const
		Sprawdza czy w podanym katalogu, b�d� jego podkatalogach znajduj�
		si� pliki z konfiguracj� zestawu.
		\param path �cie�ka do katalogu
	**/
	bool validateDir(const QString &path) const;

public:
	/**
		\fn Themes(const QString &name, const QString &configname)
		Konstruktor tworz�cy nowy zestaw
		\param name typ zestawu
		\param configname plik z konfiguracj� zestawu
	**/
	Themes(const QString &name, const QString &configname = QString());

	/**
		\fn QStringList defaultKaduPathsWithThemes() const
		Tworzy list� �cie�ek do zestaw�w znajduj�cych si� w lokalizacjach
		domy�lnych dla typu zestawu.
		\return lista �cie�ek do zestaw�w
	**/
	QStringList defaultKaduPathsWithThemes() const;

	/**
		\fn const QStringList &themes() const
		Funkcja zwraca list� z nazwami dost�pnych zestaw�w
	**/
	const QStringList & themes() const;

	/**
		\fn const QString &theme() const
		Funkcja zwraca nazw� aktualnie wybranego zestawu
	**/
	const QString & theme() const;

	/**
		\fn const QStringList &paths() const
		Funkcja zwraca list� �cie�ek, w kt�rych wyszukiwane s� zestawy
	**/
	const QStringList & paths() const;

	const QStringList & additionalPaths() const;

	/**
		\fn const QStringList &paths() const
		Funkcja zwraca �cie�k� do wskazanego zestawu
	**/
	QString themePath(const QString &theme = QString()) const;

	QString getThemeEntry(const QString &name) const;
	const QMap<QString, QString>  &getEntries() { return entries; }

public slots:
		/**
		\fn void setTheme(const QString &theme)
		Zmienia aktualny zestaw na wskazany o ile ten istnieje
		\param theme nazwa zestawu
	**/
	void setTheme(const QString &theme);

	/**
		\fn void setPaths(const QStringList &paths)
		Ustawia list� �cie�ek, w kt�rych wyszukiwane s� zestawy
		\param paths lista �cie�ek
	**/
	void setPaths(const QStringList &paths);

signals:
	/**
		\fn void themeChanged(const QString &theme)
	 	Sygna� jest emitowany, gdy zmieniono aktualny zestaw
		\param theme nazwa nowego zestawu
	**/
	void themeChanged(const QString &theme);

	/**
		\fn void pathsChanged(const QStringList &list)
		Sygna� jest emitowany, gdy zmieniono �cie�k� wyszukiwania
		\param list lista nowych �cie�ek
	**/
	void pathsChanged(const QStringList &list);

};

#endif
