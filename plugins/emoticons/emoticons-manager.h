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
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QVector>

#include "configuration/configuration-holder.h"
#include "themes.h"

#include "emoticons.h"

class QDomDocument;
class QDomText;

class Emoticon;
class EmoticonExpanderDomVisitorProvider;
class EmoticonsConfigurationUiHandler;
class EmoticonThemeManager;
class InsertEmoticonAction;

/**
	Menad�er emotikon�w
**/
class KADUAPI EmoticonsManager : public ConfigurationHolder
{
	Q_OBJECT
	Q_DISABLE_COPY(EmoticonsManager)

	EmoticonExpanderDomVisitorProvider *ExpanderDomVisitorProvider;
	EmoticonThemeManager *ThemeManager;
	EmoticonsConfigurationUiHandler *ConfigurationUiHandler;
	InsertEmoticonAction *InsertAction;

	QVector<Emoticon> Aliases;
	QVector<Emoticon> Selector;

	static QString getQuoted(const QString &s, unsigned int &pos);

	bool loadGGEmoticonThemePart(const QString &themeSubDirPath);
	bool loadGGEmoticonTheme(const QString &themeDirPath);
	void loadTheme();

public:
	explicit EmoticonsManager(QObject *parent = 0);
	virtual ~EmoticonsManager();

	EmoticonThemeManager * themeManager() const;

	const QVector<Emoticon> & aliases() { return Aliases; }
	const QVector<Emoticon> & selectors() { return Selector; }

	void configurationUpdated();

};

#endif // EMOTICONS_MANAGER_H
