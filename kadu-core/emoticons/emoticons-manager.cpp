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

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include "configuration/configuration-file.h"
#include "emoticons/emots-walker.h"
#include "misc/misc.h"
#include "themes/emoticon-theme-manager.h"
#include "debug.h"
#include "html_document.h"

#include "emoticons-manager.h"

EmoticonsManager * EmoticonsManager::Instance = 0;

EmoticonsManager * EmoticonsManager::instance()
{
	if (Instance == 0)
		Instance = new EmoticonsManager();
	return Instance;
}

EmoticonsManager::EmoticonsListItem::EmoticonsListItem()
{
}

EmoticonsManager::EmoticonsManager() :
		Aliases(), Selector(), walker(0)

{
	QStringList iconPaths = config_file.readEntry("Chat", "EmoticonsPaths").split(QRegExp("[;:&]"), QString::SkipEmptyParts);

	ThemeManager = new EmoticonThemeManager(this);
	ThemeManager->loadThemes(iconPaths);
	configurationUpdated();
}

EmoticonsManager::~EmoticonsManager()
{
	if (walker)
		delete walker;
}

EmoticonThemeManager * EmoticonsManager::themeManager() const
{
	return ThemeManager;
}

void EmoticonsManager::configurationUpdated()
{
	bool themeWasChanged = config_file.readEntry("Chat", "EmoticonsTheme") != ThemeManager->currentTheme().path();
	if (themeWasChanged)
	{
		ThemeManager->setCurrentTheme(config_file.readEntry("Chat", "EmoticonsTheme"));
		config_file.writeEntry("Chat", "EmoticonsTheme", ThemeManager->currentTheme().path());

		loadTheme();
	}
}

void EmoticonsManager::loadTheme()
{
	Aliases.clear();
	Selector.clear();
	delete walker;
	walker = 0;

	Theme theme = ThemeManager->currentTheme();
	if (theme.isValid())
		loadGGEmoticonTheme(theme.path());
}

QString EmoticonsManager::getQuoted(const QString &s, unsigned int &pos)
{
	QString r;
	++pos; // eat '"'

	int pos2 = s.indexOf('"', pos);
	if (pos2 >= 0)
	{
		r = s.mid(pos, uint(pos2) - pos);
		pos = uint(pos2) + 1;// eat '"'
	}
	else
	{
		r = s.mid(pos);
		pos = s.length();
	}
	return r;
}

bool EmoticonsManager::loadGGEmoticonThemePart(const QString &themeSubDirPath)
{
	QString dir = themeSubDirPath;

	if (!dir.isEmpty() && !dir.endsWith('/'))
		dir += '/';

	QFile theme_file(dir + "emots.txt");
	if (!theme_file.open(QIODevice::ReadOnly))
	{
		kdebugm(KDEBUG_FUNCTION_END|KDEBUG_WARNING, "Error opening %s file\n",
			qPrintable(theme_file.fileName()));
		return false;
	}
	QTextStream theme_stream(&theme_file);
	theme_stream.setCodec(codec_cp1250);
	while (!theme_stream.atEnd())
	{
		EmoticonsListItem item;
		QString line = theme_stream.readLine();
		kdebugm(KDEBUG_DUMP, "> %s\n", qPrintable(line));
		unsigned int lineLength = line.length();
		unsigned int i = 0;
		bool multi = false;
		QStringList aliases;
		if (i < lineLength && line[i] == '*')
			++i; // eat '*'
		if (i < lineLength && line[i] == '(')
		{
			multi = true;
			++i;
		}
		for (;;)
		{
			aliases.append(getQuoted(line, i));
			if (!multi || i >= lineLength || line[i] == ')')
				break;
			++i; // eat ','
		}
		if (multi)
			++i; // eat ')'
		++i; // eat ','
		item.anim = themeSubDirPath + '/' + fixFileName(themeSubDirPath, getQuoted(line, i));
		if (i < lineLength && line[i] == ',')
		{
			++i; // eat ','
			item.stat = themeSubDirPath + '/' + fixFileName(themeSubDirPath, getQuoted(line, i));
		}
		else
			item.stat = item.anim;

		foreach (const QString &alias, aliases)
		{
			item.alias = alias;
			item.escapedAlias = alias;
			HtmlDocument::escapeText(item.escapedAlias);
			Aliases.push_back(item);
		}

		item.alias = aliases[0];
		Selector.append(item);
	}
	theme_file.close();
	kdebugf2();
	return true;
}

bool EmoticonsManager::loadGGEmoticonTheme(const QString &themeDirPath)
{
	Aliases.clear();
	Selector.clear();

	bool something_loaded = false;
	if (loadGGEmoticonThemePart(themeDirPath))
		something_loaded = true;

	QDir themeDir(ThemeManager->currentTheme().path());
	QFileInfoList subDirs = themeDir.entryInfoList(QDir::Dirs);

	foreach (const QFileInfo &subDirInfo, subDirs)
	{
		if (subDirInfo.fileName().startsWith('.'))
			continue;

		QString subDir = subDirInfo.canonicalFilePath();
		if (EmoticonThemeManager::containsEmotsTxt(subDir))
			if (loadGGEmoticonThemePart(subDir))
				something_loaded = true;
	}

	if (something_loaded)
	{
		// delete previous dictionary of emots
		delete walker;
		walker = new EmotsWalker();
		int i = 0;

		// put all emots into dictionary, to allow easy finding
		// their occurrences in text
		foreach (const EmoticonsListItem &item, Aliases)
			walker->insertString(item.alias.toLower(), i++);
	}

	return something_loaded;
}

void EmoticonsManager::expandEmoticons(HtmlDocument &doc, EmoticonsStyle style)
{
	kdebugf();

	if (EmoticonsStyleNone == style)
		return;

	const static QString emotTemplate("<img emoticon=\"1\" alt=\"%1\" title=\"%1\" src=\"file:///%2\" />");

	if (!walker)
	{
		kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_WARNING, "end: EMOTICONS NOT LOADED!\n");
		return;
	}

	// check in config if user wants animated emots
	bool animated = style == EmoticonsStyleAnimated;

	kdebugm(KDEBUG_INFO, "Expanding emoticons...\n");
	// iterate through parsed html parts of message
	for (int e_i = 0; e_i < doc.countElements(); ++e_i)
	{
		// emots are not expanded in html tags
		if (doc.isTagElement(e_i))
			continue;

		// analyze text of this text part
		QString text = doc.elementText(e_i).toLower();
		// variables storing position of last occurrence
		// of emot matching current emots dictionary
		unsigned int lastBegin = 10000;
		int lastEmot = -1;
		// intitialize automata for checking occurrences
		// of emots in text
		walker -> initWalking();
		for (unsigned int j = 0, textlength = text.length(); j < textlength; ++j)
		{
			// find out if there is some emot occurrence when we
			// add current character
			int idx = walker -> checkEmotOccurrence(text[j]);
			// when some emot from dictionary is ending at current character
			if (idx >= 0)
			{
				// check if there already was some occurrence, whose
				// beginning is before beginning of currently found one
				if (lastEmot >= 0 && lastBegin < j - Aliases[idx].alias.length() + 1)
				{
					// if so, then replace that previous occurrence
					// with html tag
					QString new_text = emotTemplate.arg(Aliases[lastEmot].escapedAlias, animated ? Aliases[lastEmot].anim : Aliases[lastEmot].stat);

					doc.splitElement(e_i, lastBegin, Aliases[lastEmot].alias.length());
					doc.setElementValue(e_i, new_text, true);
					// our analysis will begin directly after
					// occurrence of previous emot
					lastEmot = -1;
					break;
				}
				else
				{
					// this is first occurrence in current text part
					lastEmot = idx;
					lastBegin = j - Aliases[lastEmot].alias.length() + 1;
				}
			}
		}
		// this is the case, when only one emot was found in current text part
		if (lastEmot >= 0)
		{
			QString new_text = emotTemplate.arg(Aliases[lastEmot].escapedAlias, animated ? Aliases[lastEmot].anim : Aliases[lastEmot].stat);

			doc.splitElement(e_i, lastBegin, Aliases[lastEmot].alias.length());
			doc.setElementValue(e_i, new_text, true);
		}
	}
	kdebugm(KDEBUG_DUMP, "Emoticons expanded, html is below:\n%s\n", qPrintable(doc.generateHtml()));
	kdebugf2();
}

int EmoticonsManager::selectorCount() const
{
	return Selector.count();
}

QString EmoticonsManager::selectorString(int emot_num) const
{
	if ((emot_num >= 0) && (emot_num < Selector.count()))
		return Selector[emot_num].alias;
	else
		return QString();
}

QString EmoticonsManager::selectorAnimPath(int emot_num) const
{
	if ((emot_num >= 0) && (emot_num < Selector.count()))
		return Selector[emot_num].anim;
	else
		return QString();
}

QString EmoticonsManager::selectorStaticPath(int emot_num) const
{
	if ((emot_num >= 0) && ((emot_num) < Selector.count()))
		return Selector[emot_num].stat;
	else
		return QString();
}
