/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2009 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2008, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <QtCore/QMap>
#include <QtCore/QString>

#ifdef Q_WS_MAC
#include "macspellchecker.h"
#endif

#include "gui/windows/main-configuration-window.h"

class QListWidget;
class QListWidgetItem;

class ChatWidget;

#ifdef HAVE_ASPELL
struct AspellSpeller;
struct AspellConfig;
#else
namespace enchant
{
	class Dict;
}
#endif

class SpellChecker : public ConfigurationUiHandler
{
	Q_OBJECT

public:
#ifdef Q_WS_MAC
	typedef QMap<QString, MacSpellChecker *> Checkers;
#else
#ifdef HAVE_ASPELL
	typedef QMap<QString, AspellSpeller *> Checkers;
#else
	typedef QMap<QString, enchant::Dict *> Checkers;
#endif
#endif // Q_WS_MAC

private:
#ifdef HAVE_ASPELL
	AspellConfig *SpellConfig;
#endif

#ifdef Q_WS_MAC
	MacSpellChecker *MacSpellCheck;
#endif

	Checkers MyCheckers;

	QListWidget *AvailableLanguagesList;
	QListWidget *CheckedLanguagesList;

private slots:
	void configurationWindowApplied();

public:
	explicit SpellChecker(QObject *parent = 0);
	virtual ~SpellChecker();

	QStringList notCheckedLanguages();
	QStringList checkedLanguages();
	bool addCheckedLang(const QString &name);
	void removeCheckedLang(const QString &name);
	void buildMarkTag();
	void buildCheckers();
	bool checkWord(const QString &word);
	QStringList buildSuggestList(const QString &word);

public slots:
	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

	void chatCreated(ChatWidget *chatWidget);
	void configForward();
	void configBackward();
	void configForward2(QListWidgetItem *item);
	void configBackward2(QListWidgetItem *item);

};

#endif // SPELLCHECKER_H
