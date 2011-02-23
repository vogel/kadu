/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef SOUND_THEME_MANAGER_H
#define SOUND_THEME_MANAGER_H

#include <QtCore/QString>

class Themes;

class SoundThemeManager
{
	static SoundThemeManager * Instance;

	Themes *MyThemes;

	SoundThemeManager();
	~SoundThemeManager();

public:
	static void createInstance();
	static void destroyInstance();
	static SoundThemeManager * instance();

	void applyTheme(const QString &themeName);

	Themes * themes() { return MyThemes; }

};

#endif // SOUND_THEME_MANAGER_H
