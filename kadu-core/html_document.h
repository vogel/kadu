/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
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

#ifndef KADU_HTML_DOCUMENT_H
#define KADU_HTML_DOCUMENT_H

#include <QtCore/QList>
#include <QtCore/QString>

#include "exports.h"

/**
	TODO: replace, refactor, remove or something

	Klasa reprezentuj�ca dokument html. Przechowuje
	list� element�w. Element mo�e by� tagiem html
	albo kawa�kiem tekstu.
**/
class KADUAPI HtmlDocument
{
	struct Element
	{
		QString text;
		bool tag;
		Element() : text(), tag(false) {}
	};
	QList<Element> Elements;
	void addElement(Element e);

public:
	static void escapeText(QString &text);
	static void unescapeText(QString &text);

	HtmlDocument();

	/**
		Dodaje podany tekst do listy element�w jako
		tag html.
		@param text tekst do dodania
	**/
	void addTag(const QString &text);

	/**
		Dodaje podany tekst do listy element�w jako
		zwyk�y tekst.
		@param text tekst do dodania
	**/
	void addText(const QString &text);

	/**
		Wstawia podany tekst do listy element�w jako
		tag html przed innym elementem.
		@param pos indeks elementu przed ktorym wstawiamy
		@param text tekst do dodania
	**/
	void insertTag(const int pos, const QString &text);

	/**
		Wstawia podany tekst do listy element�w jako
		zwykly tekst przed innym elementem.
		@param pos indeks elementu przed ktorym wstawiamy
		@param text tekst do dodania
	**/
	void insertText(const int pos, const QString &text);

	/**
		Parsuje podany napis zgodny ze struktur� html
		i otrzymane dane przypisuje do dokumentu.
		@param html napis do skonwertowania
	**/
	void parseHtml(const QString &html);

	/**
		Na podstawie zawarto�ci dokumentu generuje
		napis zgodny ze struktur� html. Znaki specjalne
		wchodz�ce w sk�ad element�w nie b�d�cych tagami
		html s� escapowane.
	**/
	QString generateHtml() const;

	/**
		Zwraca ilo�� element�w wchodz�cych w sk�ad
		dokumentu.
	**/
	int countElements() const;

	/**
		Sprawdza czy element o podanym indeksie jest
		tagiem html czy zwyk�ym tekstem.
	**/
	bool isTagElement(int index) const;

	/**
		Zwraca tekst elementu o podanym indeksie.
	**/
	const QString & elementText(int index) const;

	/**
		Zwraca tekst elementu o podanym indeksie.
	**/
	QString & elementText(int index);

	/**
		Ustawia tekst i typ elementu o podanym indeksie.
	**/
	void setElementValue(int index, const QString &text, bool tag = false);

	/**
		Wydziela z elementu podany fragment tekstu.
		Element mo�e pozosta� jeden albo
		ulec podzieleniu na dwa lub nawet trzy elementy.
		Funkcja zmienia warto�� argumentu index, aby
		indeks wydzielonego elementu by� aktualny.
	**/
	void splitElement(int &index, int start, int length);

};

#endif // KADU_HTML_DOCUMENT_H
