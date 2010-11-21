/*
 * %kadu copyright begin%
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003, 2004, 2006 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2003, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2003, 2004 Dariusz Jagodzik (mast3r@kadu.net)
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

#ifndef EMOTICONS_MANAGER_H
#define EMOTICONS_MANAGER_H

#include "emoticons/emoticons.h"
#include "themes.h"

class EmoticonThemeManager;
class EmotsWalker;
class HtmlDocument;

/**
	Menad�er emotikon�w
**/
class KADUAPI EmoticonsManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(EmoticonsManager)

	/**
		Konstruktor tworzy obiekt oraz wyszukuje list� zestaw�w w katalogu
		$KADU_DATADIR/kadu/themes/emoticons
	**/
	EmoticonsManager();

	static EmoticonsManager *Instance;

	EmoticonThemeManager *ThemeManager;

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

	bool loadGGEmoticonThemePart(const QString &themeSubDirPath);
	bool loadGGEmoticonTheme(const QString &themeDirPath);
	void loadTheme();

public:
	static EmoticonsManager * instance();

	EmoticonThemeManager * themeManager() const;

	~EmoticonsManager();

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

	void configurationUpdated();
};

#endif // EMOTICONS_MANAGER_H
