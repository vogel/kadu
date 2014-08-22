/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef GADU_EMOTICON_THEME_LOADER_H
#define GADU_EMOTICON_THEME_LOADER_H

#include <QtCore/QVector>

#include "emoticon.h"

#include "theme/emoticon-theme-loader.h"

/**
 * @addtogroup Emoticons
 * @{
 */

/**
 * @class GaduEmoticonThemeLoader
 * @short Loader for Gadu Gadu emoticon themes.
 */
class GaduEmoticonThemeLoader : public EmoticonThemeLoader
{
	QVector<Emoticon> Emoticons;
	QVector<Emoticon> Aliases;

	bool validGaduGaduEmoticonDirectory(const QString &dir);
	void loadEmoticons(const QString &path);

public:
	virtual ~GaduEmoticonThemeLoader() {}

	virtual EmoticonTheme loadEmoticonTheme(const QString &path);

};

/**
 * @}
 */

#endif // GADU_EMOTICON_THEME_LOADER_H
