/*
 * %kadu copyright begin%
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003, 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef EMOTICONS_H
#define EMOTICONS_H

#include <QtCore/QList>
#include <QtCore/QPair>
#include <QtGui/QLabel>
#include <QtGui/QScrollArea>

#include "configuration/configuration-aware-object.h"
#include "html_document.h"
#include "themes.h"
#include "exports.h"

class ChatEditBox;
class EmotsWalker;

enum EmoticonsStyle
{
	EmoticonsStyleNone,
	EmoticonsStyleStatic,
	EmoticonsStyleAnimated
};

enum EmoticonsScaling
{
	EmoticonsScalingNone,
	EmoticonsScalingStatic,
	EmoticonsScalingAnimated,
	EmoticonsScalingAll
};

/**
	Menad�er emotikon�w
**/
class KADUAPI EmoticonsManager : public Themes, ConfigurationAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(EmoticonsManager)

	/**
		Konstruktor tworzy obiekt oraz wyszukuje list� zestaw�w w katalogu
		$DATADIR/kadu/themes/emoticons
	**/
	EmoticonsManager();

	static EmoticonsManager *Instance;

	struct EmoticonsListItem
	{
		QString alias;
		QString escapedAlias;
		QString anim;
		QString stat;
		EmoticonsListItem();
	};
	QList<EmoticonsListItem> Aliases;
	QList<EmoticonsListItem> Selector;
	EmotsWalker *walker;

	static QString getQuoted(const QString &s, unsigned int &pos);

	bool loadGGEmoticonThemePart(const QString &subdir);
	bool loadGGEmoticonTheme();

public:
	static EmoticonsManager * instance();

	~EmoticonsManager();

	/**
		\brief Funkcja ustawia zestaw emotikonek na theme
		\param theme nazwa zestawu
		Funkcja ustawi zestaw je�li taki istnieje w przeciwnym wypadku
		ustawionym zestawem b�dzie "gadu-gadu"
	**/
	void setEmoticonsTheme(const QString &theme);

	/**
		Funkcja wstawia w text zamiast tag�w emotikonek odpowiednie emotikonki
		\param text dokument w kt�rym maj� by� zamienione tagi
		\param style styl emotikonki jaki ma by� wstawiony - domy�lnie jest do
		 aktualny zestaw
	**/
	void expandEmoticons(HtmlDocument &text, EmoticonsStyle style);

	/**
		Funkcja zwraca ilo�� emotikonek w zestawie
	**/
	int selectorCount() const;

	/**
		Funkcja zwraca alias odpowiedniej emotikonki
		\param emot_num nr emotikonki z listy
		\return Zwracany jest pusty ci�g je�li nie znaleziono emotikonki w przeciwnym razie zwracany jest alias.
	**/
	QString selectorString(int emot_num) const;

	/**
		Funkcja zwraca �cie�k� do odpowiedniej animowanej emotikonki
		\param emot_num nr emotikonki z listy
		\return Zwracany jest pusty ci�g je�li nie znaleziono emotikonki w przeciwnym razie zwracana jest �cie�ka do emotikonki
	**/
	QString selectorAnimPath(int emot_num) const;

	/**
		Funkcja zwraca �cie�k� do odpowiedniej statycznej emotikonki
		\param emot_num nr emotikonki z listy
		\return Zwracany jest pusty ci�g je�li nie znaleziono emotikonki w przeciwnym razie zwracana jest �cie�ka do emotikonki
	**/
	QString selectorStaticPath(int emot_num) const;

	virtual void configurationUpdated();

};

/**
	Klasa s�u��ca do wyboru emotikonki z zestawu
**/
class EmoticonSelectorButton : public QLabel
{
	Q_OBJECT

	QString EmoticonString;
	QString AnimPath;
	QString StaticPath;

private slots:
	void buttonClicked();

protected:
	class MovieViewer;
	friend class MovieViewer;

	/**
		Funkcja obs�uguj�ca ruch kursora na obszarze emotikonki.
	**/
	void mouseMoveEvent(QMouseEvent *e);

public:
	/**
		Konstruktor tworz�cy przycisk z emotikonk�
		\param parent widget na kt�rym osadzona ma by� przycisk z emotikonk�
		\param emoticon_string nazwa kt�ra ma by� wy�wietlana po najechaniu na
		przycisk
		\param static_path �cie�ka do statycznej emotikonki
		\param anim_path �cie�ka do animowanej emotikonki
	**/
	EmoticonSelectorButton(const QString &emoticon_string, const QString &static_path, const QString &anim_path, QWidget *parent);

signals:
	/**
		Sygna� emitowany po naci�ni�ciu przycisku z emotikonk�
		przekazywana jest warto�� emoticon_string (przewa�nie jest to tag)
	**/
	void clicked(const QString &emoticon_string);

};

// TODO: make it ignoring wheel events so the widget can be scrolled with mouse wheel
class EmoticonSelectorButton::MovieViewer : public QLabel
{
	Q_OBJECT

protected:
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);

public:
	explicit MovieViewer(EmoticonSelectorButton *parent);

signals:
	void clicked();

};

/**
	Klasa wy�wietlaj�ca list� emotikonek z aktualnego zestawu.
**/
class EmoticonSelector : public QScrollArea
{
	Q_OBJECT

	void addEmoticonButtons(int num_emoticons, QWidget *mainwidget);
	void calculatePositionAndSize(const QWidget *activatingWidget, const QWidget *mainwidget);

private slots:
	void iconClicked(const QString &emoticon_string);

protected:
	bool event(QEvent *e);

public:
	/**
		Konstruktor tworz�cy list� emotikonek.
		\param activatingWidget okno wywo�uj�ce
		\param parent rodzic na kt�rym ma by� wy�wietlona lista
	**/
	EmoticonSelector(const QWidget *activatingWidget, QWidget *parent);

signals:
	// TODO: rename
	void emoticonSelect(const QString &);

};

struct PrefixNode;
typedef QPair<QChar, PrefixNode *> Prefix;

struct PrefixNode
{
	int emotIndex;
	QList<Prefix> children;
	PrefixNode();
};

/**
	this class serves as dictionary of emots, allowing easy
	finding of their occurrences in text;
	new search is initialized by calling 'initWalking()'
	then characters are put into analysis by 'checkEmotOccurrence(c)'
*/
class EmotsWalker
{
	/** dictionary is based on prefix tree */
	PrefixNode *root;
	QPair<QChar, PrefixNode *> myPair;
	/** positions in prefix tree, representing current analysis of text */
	QList<const PrefixNode *> positions;
	QList<int> lengths;
	int amountPositions;

	PrefixNode * findChild(const PrefixNode *node, const QChar &c);
	PrefixNode * insertChild(PrefixNode *node, const QChar &c);
	void removeChilds(PrefixNode *node);

public:
	EmotsWalker();
	~EmotsWalker();

	/**
		adds given string (emot) to dictionary of emots, giving it
		number, which will be used later to notify occurrences of
		emot in analyzed text
	*/
	void insertString(const QString &str, int num);

	/**
		return number of emot, which occurre in analyzed text just
		after adding given character (thus ending on this character)
		beginning of text analysis is turned on by 'initWalking()'
		if no emot occures, -1 is returned
	*/
	int checkEmotOccurrence(const QChar &c);

	/**
		clear internal structures responsible for analyzing text, it allows
		begin of new text analysis
	*/
	void initWalking();

};

#endif // EMOTICONS_H
