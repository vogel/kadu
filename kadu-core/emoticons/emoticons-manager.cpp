/*
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003, 2004, 2006 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2003, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2003, 2004 Dariusz Jagodzik (mast3r@kadu.net)
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QStack>
#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>
#include <QtGui/QTextDocument>

#include "configuration/configuration-file.h"
#include "emoticons/emots-walker.h"
#include "misc/misc.h"
#include "themes/emoticon-theme-manager.h"
#include "debug.h"

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
	QStringList iconPaths = config_file.readEntry("Chat", "EmoticonsPaths").split('&', QString::SkipEmptyParts);

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
	bool themeWasChanged = config_file.readEntry("Chat", "EmoticonsTheme") != ThemeManager->currentTheme().name();
	if (themeWasChanged)
	{
		ThemeManager->setCurrentTheme(config_file.readEntry("Chat", "EmoticonsTheme"));
		config_file.writeEntry("Chat", "EmoticonsTheme", ThemeManager->currentTheme().name());

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
	theme_stream.setCodec(QTextCodec::codecForName("CP1250"));
	while (!theme_stream.atEnd())
	{
		EmoticonsListItem item;
		QString line = theme_stream.readLine();
		kdebugm(KDEBUG_DUMP, "> %s\n", qPrintable(line));
		unsigned int lineLength = line.length();
		unsigned int i = 0;
		bool multi = false;
		QStringList aliases;
		if (i < lineLength && line.at(i) == '*')
			++i; // eat '*'
		if (i < lineLength && line.at(i) == '(')
		{
			multi = true;
			++i;
		}
		for (;;)
		{
			aliases.append(getQuoted(line, i));
			if (!multi || i >= lineLength || line.at(i) == ')')
				break;
			++i; // eat ','
		}
		if (multi)
			++i; // eat ')'
		++i; // eat ','
		item.anim = themeSubDirPath + '/' + fixFileName(themeSubDirPath, getQuoted(line, i));
		if (i < lineLength && line.at(i) == ',')
		{
			++i; // eat ','
			item.stat = themeSubDirPath + '/' + fixFileName(themeSubDirPath, getQuoted(line, i));
		}
		else
			item.stat = item.anim;

		foreach (const QString &alias, aliases)
		{
			item.alias = alias;
			Aliases.push_back(item);
		}

		item.alias = aliases.at(0);
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

QDomText EmoticonsManager::insertEmoticon(QDomDocument domDocument, QDomText textNode, const EmoticonsManager::EmoticonsListItem &emoticon, int index, bool animated)
{
	int emoticonLength = emoticon.alias.length();

	QDomText afterEmoticon = textNode.splitText(index + emoticonLength);
	textNode.setNodeValue(textNode.nodeValue().mid(0, index));

	QDomElement emoticonElement = domDocument.createElement("img");
	emoticonElement.setAttribute("emoticon", emoticon.alias);
	emoticonElement.setAttribute("title", emoticon.alias);
	emoticonElement.setAttribute("alt", emoticon.alias);
	emoticonElement.setAttribute("src", "file:///" + animated ? emoticon.anim : emoticon.stat);
	textNode.parentNode().insertBefore(emoticonElement, afterEmoticon);

	return afterEmoticon;
}

QDomText EmoticonsManager::expandFirstEmoticon(QDomDocument domDocument, QDomText textNode, bool animated)
{
	QString text = textNode.nodeValue().toLower();
	int textLength = text.length();

	if (0 == textLength)
		return QDomText();

	int currentEmoticonStart = -1;
	int currentEmoticonIndex = -1;

	walker->initWalking();
	for (int i = 0; i < textLength; i++)
	{
		int emoticonIndex = walker->checkEmotOccurrence(text.at(i), (i < textLength - 1) && text.at(i + 1).isLetter());
		if (emoticonIndex < 0)
			continue;

		int emoticonStart = i - Aliases.at(emoticonIndex).alias.length() + 1;
		if (currentEmoticonIndex < 0 || currentEmoticonStart >= emoticonStart)
		{
			currentEmoticonIndex = emoticonIndex;
			currentEmoticonStart = emoticonStart;
			continue;
		}

		const EmoticonsListItem &emoticon = Aliases.at(currentEmoticonIndex);
		return insertEmoticon(domDocument, textNode, emoticon, currentEmoticonStart, animated);
	}

	if (currentEmoticonIndex >= 0)
	{
		const EmoticonsListItem &emoticon = Aliases.at(currentEmoticonIndex);
		insertEmoticon(domDocument, textNode, emoticon, currentEmoticonStart, animated);
	}

	return QDomText();
}

void EmoticonsManager::expandEmoticons(QDomDocument domDocument, QDomText textNode, bool animated)
{
	while (!textNode.isNull())
		textNode = expandFirstEmoticon(domDocument, textNode, animated);
}

QString EmoticonsManager::expandEmoticons(const QString &html, EmoticonsStyle style)
{
	kdebugf();

	// prepare string for KaduWebView::convertClipboardHtml()
	const static QString emotTemplate("<img emoticon=\"1\" title=\"%1\" alt=\"%1\" src=\"file:///%2\" />");

	if (EmoticonsStyleNone == style)
		return html;

	if (!walker)
	{
		kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_WARNING, "end: EMOTICONS NOT LOADED!\n");
		return html;
	}

	// check in config if user wants animated emots
	bool animated = style == EmoticonsStyleAnimated;

	QDomDocument domDocument;
	// force content to be valid HTML with only one root
	domDocument.setContent(QString("<div>%1</div>").arg(html));

	QStack<QDomNode> nodes;
	nodes.push(domDocument.documentElement());

	while (!nodes.isEmpty())
	{
		QDomNode node = nodes.pop();

		QDomNodeList childNodes = node.childNodes();
		uint childNodesLength = childNodes.length();
		for (uint i = 0; i < childNodesLength; i++)
			nodes.append(childNodes.at(i));

		if (!node.isText())
			continue;

		expandEmoticons(domDocument, node.toText(), animated);
	}


	QString result = domDocument.toString(0);
	// remove <div></div>
	return result.mid(5, result.length() - 12);
}

int EmoticonsManager::selectorCount() const
{
	return Selector.count();
}

QString EmoticonsManager::selectorString(int emot_num) const
{
	if ((emot_num >= 0) && (emot_num < Selector.count()))
		return Selector.at(emot_num).alias;
	else
		return QString();
}

QString EmoticonsManager::selectorAnimPath(int emot_num) const
{
	if ((emot_num >= 0) && (emot_num < Selector.count()))
		return Selector.at(emot_num).anim;
	else
		return QString();
}

QString EmoticonsManager::selectorStaticPath(int emot_num) const
{
	if ((emot_num >= 0) && ((emot_num) < Selector.count()))
		return Selector.at(emot_num).stat;
	else
		return QString();
}
