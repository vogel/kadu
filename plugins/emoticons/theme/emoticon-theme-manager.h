/*
 * %kadu copyright begin%
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef EMOTICON_THEME_MANAGER_H
#define EMOTICON_THEME_MANAGER_H

#include "themes/theme-manager.h"

/**
 * @addtogroup Emoticons
 * @{
 */

/**
 * @class EmoticonThemeManager
 * @short Theme manager for emoticons.
 */
class EmoticonThemeManager : public ThemeManager
{
	Q_OBJECT

protected:
	virtual QString defaultThemeName() const;
	virtual QStringList defaultThemePaths() const;
	virtual bool isValidThemePath(const QString &themePath) const;

public:
	/**
	 * @short Return default theme name.
	 * @return default theme name
	 */
	static QString defaultTheme();

	/**
	 * @short Check if given directory contains emots.txt file.
	 * @param dir directory path to check
	 * @return true if directory dir contains emots.txt file
	 */
	static bool containsEmotsTxt(const QString &dir);

	explicit EmoticonThemeManager(QObject *parent = 0);
	virtual ~EmoticonThemeManager();

};

/**
 * @}
 */

#endif // EMOTICON_THEME_MANAGER_H
