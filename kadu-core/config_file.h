/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

#include <QtCore/QMap>
#include <QtCore/QRect>
#include <QtCore/QSize>
#include <QtCore/QStringList>
#include <QtGui/QColor>
#include <QtGui/QFont>
#include <QtXml/QDomDocument>

#include "exports.h"

/**
	Klasa reprezentuj�ca plik XML-owy z zapisem konfiguracji programu
**/
class KADUAPI XmlConfigFile
{
	QDomDocument DomDocument;
	void write(const QString& f = QString::null);

public:
	/**
		Otwiera plik i go wczytuje
	**/
	XmlConfigFile();

	/**
		Wczytuje plik konfiguracyjny z dysku
	**/
	void read();

	/**
		Zapisuje na dysk zawarto�� konfiguracji
	**/
	void sync();

	/**
		Zapisuje do wybranego pliku kopi� konfiguracji
	**/
	void saveTo(const QString &filename);

	void makeBackup();

	/**
		Zwraca glowny element konfiguracji
	**/
	QDomElement rootElement();

	/**
		Dodaje nowy element i przypisuje do rodzica.
	**/

	QDomElement createElement(QDomElement parent, const QString &tag_name);
	/**
		Zwraca pierwszy element lub element typu null jesli nie znajdzie.
	**/

	QDomElement findElement(QDomElement parent, const QString &tag_name) const;

	/**
		Zwraca pierwszy element ktorego dany atrybut ma dana wartosc
		lub element typu null jesli nie znajdzie.
	**/
	QDomElement findElementByProperty(QDomElement parent, const QString &tag_name,
		const QString &property_name, const QString &property_value) const;

	/**
		Zwraca pierwszy element lub dodaje nowy i przypisuje do rodzica.
	**/
	QDomElement accessElement(QDomElement parent, const QString &tag_name);

	/**
		Zwraca pierwszy element ktorego dany atrybut ma dana wartosc
		lub dodaje nowy i przypisuje do rodzica (ustawia rowniez zadany
		atrybut na zadana wartosc).
	**/
	QDomElement accessElementByProperty(QDomElement parent, const QString &tag_name,
		const QString &property_name, const QString &property_value);

	/**
		Usuwa wszystkie dzieci elementu.
	**/
	void removeChildren(QDomElement parent);

};

extern KADUAPI XmlConfigFile *xml_config_file;

/**
	Klasa reprezentuj�ca tekstowy plik konfiguracyjny (format INI)
**/
class PlainConfigFile
{
	void write() const;
	bool changeEntry(const QString &group, const QString &name, const QString &value);
	QString getEntry(const QString &group, const QString &name, bool *ok = 0) const;

	QString filename;
	QMap<QString, QMap<QString, QString> > groups;
	mutable QString activeGroupName;
	mutable QMap<QString, QString> *activeGroup;
	PlainConfigFile(const PlainConfigFile &);
	PlainConfigFile &operator = (const PlainConfigFile &);

public:
	/**
		Otwiera plik filename i go wczytuje
	**/
	PlainConfigFile(const QString &filename);

	/**
		Wczytuje plik konfiguracyjny z dysku
	**/
	void read();

	/**
		Zapisuje na dysk zawarto�� konfiguracji
	**/

	void sync() const;
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
	QString readEntry(const QString &group, const QString &name, const QString &def = QString::null) const;

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
		@return warto�� danego pola, je�li nie istnieje zwracana
		jest warto�� "def"
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

/**
	Klasa reprezentuj�ca plik z zapisem konfiguracji programu
**/
class KADUAPI ConfigFile
{
	bool changeEntry(const QString &group, const QString &name, const QString &value);
	QString getEntry(const QString &group, const QString &name, bool *ok = 0) const;

	QString filename;

public:
	/**
		Otwiera plik filename i go wczytuje
	**/
	ConfigFile(const QString &filename);

	/**
		Zapisuje na dysk zawarto�� konfiguracji
	**/
	void sync() const;

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
	QString readEntry(const QString &group, const QString &name, const QString &def = QString::null) const;

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
