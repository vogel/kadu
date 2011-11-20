/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/configuration-file.h"
#include "themes.h"

#include "sound-theme-manager.h"

SoundThemeManager * SoundThemeManager::Instance = 0;

void SoundThemeManager::createInstance()
{
	if (!Instance)
		Instance = new SoundThemeManager();
}

void SoundThemeManager::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

SoundThemeManager * SoundThemeManager::instance()
{
	return Instance;
}

SoundThemeManager::SoundThemeManager() :
		MyThemes(new Themes("sounds", "sound.conf"))
{
	MyThemes->setPaths(config_file.readEntry("Sounds", "SoundPaths").split('&', QString::SkipEmptyParts));

	QStringList soundThemes = themes()->themes();
	QString soundTheme = config_file.readEntry("Sounds", "SoundTheme");
	if (!soundThemes.isEmpty() && (soundTheme != "Custom") && !soundThemes.contains(soundTheme))
	{
		soundTheme = "default";
		config_file.writeEntry("Sounds", "SoundTheme", "default");
	}

	if (soundTheme != "custom")
		applyTheme(soundTheme);
}

SoundThemeManager::~SoundThemeManager()
{
	delete MyThemes;
	MyThemes = 0;
}

void SoundThemeManager::applyTheme(const QString &themeName)
{
	MyThemes->setTheme(themeName);
	QMap<QString, QString> entries = MyThemes->getEntries();
	QMap<QString, QString>::const_iterator i = entries.constBegin();

	while (i != entries.constEnd())
	{
		config_file.writeEntry("Sounds", i.key() + "_sound", MyThemes->themePath() + i.value());
		++i;
	}
}
