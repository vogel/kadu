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
#include "core/core.h"
#include "dom/dom-processor.h"
#include "dom/dom-processor-service.h"
#include "dom/ignore-links-dom-visitor.h"
#include "emoticons/animated-emoticon-path-provider.h"
#include "emoticons/emoticon.h"
#include "emoticons/emoticon-expander.h"
#include "emoticons/emoticon-expander-dom-visitor-provider.h"
#include "emoticons/emoticon-prefix-tree-builder.h"
#include "emoticons/static-emoticon-path-provider.h"
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

EmoticonsManager::EmoticonsManager() :
		Aliases(), Selector()
{
	QStringList iconPaths = config_file.readEntry("Chat", "EmoticonsPaths").split('&', QString::SkipEmptyParts);

	ThemeManager = new EmoticonThemeManager(this);
	ThemeManager->loadThemes(iconPaths);
	ExpanderDomVisitorProvider = new EmoticonExpanderDomVisitorProvider();
	Core::instance()->domProcessorService()->registerVisitorProvider(ExpanderDomVisitorProvider, 2000);
	configurationUpdated();
}

EmoticonsManager::~EmoticonsManager()
{
	Core::instance()->domProcessorService()->unregisterVisitorProvider(ExpanderDomVisitorProvider);
	delete ExpanderDomVisitorProvider;
	ExpanderDomVisitorProvider = 0;
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

	ExpanderDomVisitorProvider->setStyle((EmoticonsStyle)config_file.readNumEntry("Chat", "EmoticonsStyle"));
}

void EmoticonsManager::loadTheme()
{
	Aliases.clear();
	Selector.clear();

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

		QString animatedPath = themeSubDirPath + '/' + fixFileName(themeSubDirPath, getQuoted(line, i));
		QString staticPath;
		if (i < lineLength && line.at(i) == ',')
		{
			++i; // eat ','
			staticPath = themeSubDirPath + '/' + fixFileName(themeSubDirPath, getQuoted(line, i));
		}
		else
			staticPath = animatedPath;

		foreach (const QString &alias, aliases)
			Aliases.push_back(Emoticon(alias, staticPath, animatedPath));

		Selector.append(Emoticon(aliases.at(0), staticPath, animatedPath));
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
		EmoticonPrefixTreeBuilder builder;
		foreach (const Emoticon &emoticon, Aliases)
			builder.addEmoticon(emoticon);

		ExpanderDomVisitorProvider->setEmoticonTree(builder.tree());
	}

	return something_loaded;
}
