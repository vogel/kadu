/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2002, 2003, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2003, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2002, 2003 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003 Dariusz Jagodzik (mast3r@kadu.net)
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

#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

#include <QtCore/QMap>
#include <QtCore/QRect>
#include <QtCore/QSize>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtGui/QColor>
#include <QtGui/QFont>
#include <QtXml/QDomDocument>

#include "exports.h"

/**
	Klasa reprezentuj�ca tekstowy plik konfiguracyjny (format INI)
**/
class PlainConfigFile
{
	void write() const;
	bool changeEntry(const QString &group, const QString &name, const QString &value);
	QString getEntry(const QString &group, const QString &name, bool *ok = 0);

	QString filename;
	QString CodecName;
	QMap<QString, QMap<QString, QString> > groups;
	mutable QString activeGroupName;
	mutable QMap<QString, QString> activeGroup;
	PlainConfigFile(const PlainConfigFile &);
	PlainConfigFile &operator = (const PlainConfigFile &);

	void changeActiveGroup(const QString &newGroup);

public:
	/**
		Otwiera plik filename i go wczytuje
	**/
	PlainConfigFile(const QString &filename, const QString &codec = "ISO8859-2");

	/**
		Wczytuje plik konfiguracyjny z dysku
	**/
	void read();

	/**
		Zapisuje na dysk zawarto�� konfiguracji
	**/

	void sync();
	/**
		Zwraca ca�� sekcj� "name"
	**/
	QMap<QString, QString> & getGroupSection(const QString &name);

	/**
		Zwraca list� sekcji
	**/
	QStringList getGroupList() const;

	/**
		Zapisuje warto�� "value" do grupy "group" w pole "name"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QVariant do wpisania
	**/
	void writeEntry(const QString &group, const QString &name, const QVariant &value);

	/**
		Zapisuje warto�� "value" do grupy "group" w pole "name"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QString do wpisania
	**/
	void writeEntry(const QString &group,const QString &name, const QString &value);

	/**
		Zapisuje warto�� "value" do grupy "group" w pole "name"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� char* do wpisania
	**/
	void writeEntry(const QString &group,const QString &name, const char *value);

	/**
		Zapisuje warto�� "value" do grupy "group" w pole "name"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� int do wpisania
	**/
	void writeEntry(const QString &group, const QString &name, const int value);

	/**
		Zapisuje warto�� "value" do grupy "group" w pole "name"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� double do wpisania
	**/
	void writeEntry(const QString &group, const QString &name, const double value);

	/**
		Zapisuje warto�� "value" do grupy "group" w pole "name"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� bool do wpisania
	**/
	void writeEntry(const QString &group,const QString &name, const bool value);

	/**
		Zapisuje warto�� "value" do grupy "group" w pole "name"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QRect do wpisania
	**/
	void writeEntry(const QString &group,const QString &name, const QRect &value);

	/**
		Zapisuje warto�� "value" do grupy "group" w pole "name"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QSize do wpisania
	**/
	void writeEntry(const QString &group, const QString &name, const QSize &value);

	/**
		Zapisuje warto�� "value" do grupy "group" w pole "name"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QColor do wpisania
	**/
	void writeEntry(const QString &group, const QString &name, const QColor &value);

	/**
		Zapisuje warto�� "value" do grupy "group" w pole "name"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QFont do wpisania
	**/
	void writeEntry(const QString &group, const QString &name, const QFont &value);

	/**
		Zapisuje warto�� "value" do grupy "group" w pole "name"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QPoint do wpisania
	**/
	void writeEntry(const QString &group, const QString &name, const QPoint &value);

	/**
		Zwraca warto�� pola "name" z grupy "group"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param def domy�lna warto�� pola
		@return warto�� danego pola, je�li nie istnieje zwracana
		jest warto�� "def"
	**/
template <class T>
	T readEntry(const QString &group, const QString &name, const T &def = QVariant::Invalid );

	/**
		Zwraca warto�� pola "name" z grupy "group"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param def domy�lna warto�� pola
		@return warto�� danego pola, je�li nie istnieje zwracana
		jest warto�� "def"
	**/
	QString readEntry(const QString &group, const QString &name, const QString &def = QString());

	/**
		Zwraca warto�� pola "name" z grupy "group"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param def domy�lna warto�� pola
		@return warto�� danego pola, je�li nie istnieje zwracana
		jest warto�� "def"
	**/
	int readNumEntry(const QString &group, const QString &name, int def = 0);

	/**
		Zwraca warto�� pola "name" z grupy "group"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param def domy�lna warto�� pola
		@return warto�� danego pola, je�li nie istnieje zwracana
		jest warto�� "def"
	**/
	unsigned int readUnsignedNumEntry(const QString &group, const QString &name, unsigned int def = 0);

	/**
		Zwraca warto�� pola "name" z grupy "group"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param def domy�lna warto�� pola
		@return warto�� danego pola, je�li nie istnieje zwracana
		jest warto�� "def"
	**/
	double readDoubleNumEntry(const QString &group, const QString &name, double def = 0.0);

	/**
		Zwraca warto�� pola "name" z grupy "group"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param def domy�lna warto�� pola
		@return warto�� danego pola, je�li nie istnieje zwracana
		jest warto�� "def"
	**/
	bool readBoolEntry(const QString &group, const QString &name, bool def = false);

	/**
		Zwraca warto�� pola "name" z grupy "group"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param def domy�lna warto�� pola
		@return warto�� danego pola, je�li nie istnieje zwracana
		jest warto�� "def"
	**/
	QRect readRectEntry(const QString &group, const QString &name, const QRect *def = 0L);

	/**
		Zwraca warto�� pola "name" z grupy "group"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param def domy�lna warto�� pola
		@return warto�� danego pola, je�li nie istnieje zwracana
		jest warto�� "def"
	**/
	QSize readSizeEntry(const QString &group, const QString &name, const QSize *def = 0L);

	/**
		Zwraca warto�� pola "name" z grupy "group"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param def domy�lna warto�� pola
		@return warto�� danego pola, je�li nie istnieje zwracana
		jest warto�� "def"
	**/
	QColor readColorEntry(const QString &group, const QString &name, const QColor *def = 0L);

	/**
		Zwraca warto�� pola "name" z grupy "group"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param def domy�lna warto�� pola
		@return warto�� danego pola, je�li nie istnieje zwracana
		jest warto�� "def"
	**/
	QFont readFontEntry(const QString &group, const QString &name, const QFont *def = 0L);

	/**
		Zwraca warto�� pola "name" z grupy "group"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param def domy�lna warto�� pola
		@return warto�� danego pola, je�li nie istnieje zwracana
		jest warto�� "def"
	**/
	QPoint readPointEntry(const QString &group, const QString &name, const QPoint *def = 0L);

	/**
		Usuwa pole "name" z grupy "group"
		@param group nazwa grupy
		@param name nazwa pola w grupie
	**/
	void removeVariable(const QString &group, const QString &name);

	/**
		Tworzy pole "name" w grupie "group" je�li ono nie istnieje
		i ustawia warto�� na "value"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QPoint do wpisania
	**/
	void addVariable(const QString &group, const QString &name, const QString &value);

	/**
		Tworzy pole "name" w grupie "group" je�li ono nie istnieje
		i ustawia warto�� na "value"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QPoint do wpisania
	**/
	void addVariable(const QString &group, const QString &name, const char *value);

	/**
		Tworzy pole "name" w grupie "group" je�li ono nie istnieje
		i ustawia warto�� na "value"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QPoint do wpisania
	**/
	void addVariable(const QString &group, const QString &name, const int value);

	/**
		Tworzy pole "name" w grupie "group" je�li ono nie istnieje
		i ustawia warto�� na "value"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QPoint do wpisania
	**/
	void addVariable(const QString &group, const QString &name, const double value);

	/**
		Tworzy pole "name" w grupie "group" je�li ono nie istnieje
		i ustawia warto�� na "value"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QPoint do wpisania
	**/
	void addVariable(const QString &group, const QString &name, const bool value);

	/**
		Tworzy pole "name" w grupie "group" je�li ono nie istnieje
		i ustawia warto�� na "value"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QPoint do wpisania
	**/
	void addVariable(const QString &group, const QString &name, const QRect &value);

	/**
		Tworzy pole "name" w grupie "group" je�li ono nie istnieje
		i ustawia warto�� na "value"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QPoint do wpisania
	**/
	void addVariable(const QString &group, const QString &name, const QSize &value);

	/**
		Tworzy pole "name" w grupie "group" je�li ono nie istnieje
		i ustawia warto�� na "value"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QPoint do wpisania
	**/
	void addVariable(const QString &group, const QString &name, const QColor &value);

	/**
		Tworzy pole "name" w grupie "group" je�li ono nie istnieje
		i ustawia warto�� na "value"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QPoint do wpisania
	**/
	void addVariable(const QString &group, const QString &name, const QFont &value);

	/**
		Tworzy pole "name" w grupie "group" je�li ono nie istnieje
		i ustawia warto�� na "value"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QPoint do wpisania
	**/
	void addVariable(const QString &group, const QString &name, const QPoint &value);

};

/**
	Klasa reprezentuj�ca plik z zapisem konfiguracji programu
**/
class KADUAPI ConfigFile
{
	bool changeEntry(const QString &group, const QString &name, const QString &value);
	QString getEntry(const QString &group, const QString &name) const;

	QString filename;

public:
	/**
		Otwiera plik filename i go wczytuje
	**/
	ConfigFile(const QString &filename);

	/**
		Zapisuje na dysk zawarto�� konfiguracji
	**/
	void sync();

	/**
		Zapisuje warto�� "value" do grupy "group" w pole "name"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QString do wpisania
	**/
	void writeEntry(const QString &group, const QString &name, const QString &value);

	/**
		Zapisuje warto�� "value" do grupy "group" w pole "name"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� char* do wpisania
	**/
	void writeEntry(const QString &group, const QString &name, const char *value);

	/**
		Zapisuje warto�� "value" do grupy "group" w pole "name"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� int do wpisania
	**/
	void writeEntry(const QString &group, const QString &name, const int value);

	/**
		Zapisuje warto�� "value" do grupy "group" w pole "name"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� double do wpisania
	**/
	void writeEntry(const QString &group, const QString &name, const double value);

	/**
		Zapisuje warto�� "value" do grupy "group" w pole "name"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� bool do wpisania
	**/
	void writeEntry(const QString &group, const QString &name, const bool value);

	/**
		Zapisuje warto�� "value" do grupy "group" w pole "name"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QRect do wpisania
	**/
	void writeEntry(const QString &group, const QString &name, const QRect &value);

	/**
		Zapisuje warto�� "value" do grupy "group" w pole "name"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QSize do wpisania
	**/
	void writeEntry(const QString &group, const QString &name, const QSize &value);

	/**
		Zapisuje warto�� "value" do grupy "group" w pole "name"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QColor do wpisania
	**/
	void writeEntry(const QString &group, const QString &name, const QColor &value);

	/**
		Zapisuje warto�� "value" do grupy "group" w pole "name"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QFont do wpisania
	**/
	void writeEntry(const QString &group, const QString &name, const QFont &value);

	/**
		Zapisuje warto�� "value" do grupy "group" w pole "name"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QPoint do wpisania
	**/
	void writeEntry(const QString &group, const QString &name, const QPoint &value);

	/**
		Zwraca warto�� pola "name" z grupy "group"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param def domy�lna warto�� pola
		@return warto�� danego pola, je�li nie istnieje zwracana
		jest warto�� "def"
	**/
	QString readEntry(const QString &group, const QString &name, const QString &def = QString()) const;

	/**
		Zwraca warto�� pola "name" z grupy "group"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param def domy�lna warto�� pola
		@return warto�� danego pola, je�li nie istnieje zwracana
		jest warto�� "def"
	**/
	int readNumEntry(const QString &group, const QString &name, int def = 0) const;

	/**
		Zwraca warto�� pola "name" z grupy "group"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param def domy�lna warto�� pola
		@return warto�� danego pola, je�li nie istnieje zwracana
		jest warto�� "def"
	**/
	unsigned int readUnsignedNumEntry(const QString &group, const QString &name, unsigned int def = 0) const;

	/**
		Zwraca warto�� pola "name" z grupy "group"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param def domy�lna warto�� pola
		@return warto�� danego pola, je�li nie istnieje zwracana
		jest warto�� "def"
	**/
	double readDoubleNumEntry(const QString &group, const QString &name, double def = 0.0) const;

	/**
		Zwraca warto�� pola "name" z grupy "group"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param def domy�lna warto�� pola
		@return warto�� danego pola, je�li nie istnieje zwracana
		jest warto�� "def"
	**/
	bool readBoolEntry(const QString &group, const QString &name, bool def = false) const;

	/**
		Zwraca warto�� pola "name" z grupy "group"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param def domy�lna warto�� pola
		@return warto�� danego pola, je�li nie istnieje zwracana
		jest warto�� "def"
	**/
	QRect readRectEntry(const QString &group, const QString &name, const QRect *def = 0L) const;

	/**
		Zwraca warto�� pola "name" z grupy "group"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param def domy�lna warto�� pola
		@return warto�� danego pola, je�li nie istnieje zwracana
		jest warto�� "def"
	**/
	QSize readSizeEntry(const QString &group, const QString &name, const QSize *def = 0L) const;

	/**
		Zwraca warto�� pola "name" z grupy "group"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param def domy�lna warto�� pola
		@return warto�� danego pola, je�li nie istnieje zwracana jest warto�� "def"
	**/
	QColor readColorEntry(const QString &group, const QString &name, const QColor *def = 0L) const;

	/**
		Zwraca warto�� pola "name" z grupy "group"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param def domy�lna warto�� pola
		@return warto�� danego pola, je�li nie istnieje zwracana
		jest warto�� "def"
	**/
	QFont readFontEntry(const QString &group, const QString &name, const QFont *def = 0L) const;

	/**
		Zwraca warto�� pola "name" z grupy "group"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param def domy�lna warto�� pola
		@return warto�� danego pola, je�li nie istnieje zwracana
		jest warto�� "def"
		**/
	QPoint readPointEntry(const QString &group, const QString &name, const QPoint *def = 0L) const;

	/**
		Usuwa pole "name" z grupy "group"
		@param group nazwa grupy
		@param name nazwa pola w grupie
	**/
	void removeVariable(const QString &group, const QString &name);

	/**
		Tworzy pole "name" w grupie "group" je�li ono nie istnieje
		i ustawia warto�� na "value"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QPoint do wpisania
	**/
	void addVariable(const QString &group, const QString &name, const QString &value);

	/**
		Tworzy pole "name" w grupie "group" je�li ono nie istnieje
		i ustawia warto�� na "value"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QPoint do wpisania
	**/
	void addVariable(const QString &group, const QString &name, const char *value);

	/**
		Tworzy pole "name" w grupie "group" je�li ono nie istnieje
		i ustawia warto�� na "value"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QPoint do wpisania
	**/
	void addVariable(const QString &group, const QString &name, const int value);

	/**
		Tworzy pole "name" w grupie "group" je�li ono nie istnieje
		i ustawia warto�� na "value"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QPoint do wpisania
	**/
	void addVariable(const QString &group, const QString &name, const double value);

	/**
		Tworzy pole "name" w grupie "group" je�li ono nie istnieje
		i ustawia warto�� na "value"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QPoint do wpisania
	**/
	void addVariable(const QString &group, const QString &name, const bool value);

	/**
		Tworzy pole "name" w grupie "group" je�li ono nie istnieje
		i ustawia warto�� na "value"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QPoint do wpisania
	**/
	void addVariable(const QString &group, const QString &name, const QRect &value);

	/**
		Tworzy pole "name" w grupie "group" je�li ono nie istnieje
		i ustawia warto�� na "value"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QPoint do wpisania
	**/
	void addVariable(const QString &group, const QString &name, const QSize &value);

	/**
		Tworzy pole "name" w grupie "group" je�li ono nie istnieje
		i ustawia warto�� na "value"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QPoint do wpisania
	**/
	void addVariable(const QString &group, const QString &name, const QColor &value);

	/**
		Tworzy pole "name" w grupie "group" je�li ono nie istnieje
		i ustawia warto�� na "value"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QPoint do wpisania
	**/
	void addVariable(const QString &group, const QString &name, const QFont &value);

	/**
		Tworzy pole "name" w grupie "group" je�li ono nie istnieje
		i ustawia warto�� na "value"
		@param group nazwa grupy
		@param name nazwa pola w grupie
		@param value warto�� QPoint do wpisania
	**/
	void addVariable(const QString &group, const QString &name, const QPoint &value);

};

//TODO: po wydaniu 0.4 trzeba zmieni� nazw� na config_file i wywali� define'a
//TODO: mamy juz 0.6 ;)

extern KADUAPI ConfigFile *config_file_ptr;
#define config_file (*config_file_ptr)

#endif
