/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef XML_CONFIG_FILE_H
#define XML_CONFIG_FILE_H

#include <QtXml/QDomDocument>

#include "exports.h"

/**
	Klasa reprezentuj�ca plik XML-owy z zapisem konfiguracji programu
**/
class KADUAPI XmlConfigFile
{
public:
	enum GetNodeMode
	{
		ModeGet,
		ModeFind,
		ModeCreate
	};

private:
	QDomDocument DomDocument;
	void write(const QString& f = QString::null);

	bool isElementNamed(const QDomElement &element, const QString &name);

	void removeNodes(QDomElement parentNode, QDomNodeList nodes);
	void removeNamedNodes(QDomElement parentNode, QDomNodeList nodes, const QString &name);

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

	bool hasNode(const QString &nodeTagName);
	bool hasNode(QDomElement parentNode, const QString &nodeTagName);

	QDomElement getNode(const QString &nodeTagName, GetNodeMode getMode = ModeGet);
	QDomElement getNamedNode(const QString &nodeTagName, const QString &nodeName, GetNodeMode getMode = ModeGet);
	QDomElement getNode(QDomElement parentNode, const QString &nodeTagName, GetNodeMode getMode = ModeGet);
	QDomElement getNamedNode(QDomElement parentNode, const QString &nodeTagName, const QString &nodeName, GetNodeMode getMode = ModeGet);

	QDomNodeList getNodes(QDomElement parent, const QString &nodeTagName);

	void createTextNode(QDomElement parentNode, const QString &nodeTagName, const QString &nodeContent);
	QString getTextNode(QDomElement parentNode, const QString &nodeTagName);

};

extern KADUAPI XmlConfigFile *xml_config_file;

#endif // XML_CONFIG_FILE
