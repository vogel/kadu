/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QFileInfo>
#include <QtCore/QStringList>

#include "core/application.h"
#include "misc/paths-provider.h"

#include "icon-theme-manager.h"

QString IconThemeManager::defaultTheme()
{
	return QLatin1String("default");
}

IconThemeManager::IconThemeManager(QObject *parent) :
		ThemeManager(parent)
{
}

IconThemeManager::~IconThemeManager()
{
}

QString IconThemeManager::defaultThemeName() const
{
	return defaultTheme();
}

QStringList IconThemeManager::defaultThemePaths() const
{
	// Allow local themes to override global ones.
	QStringList result = getSubDirs(Application::instance()->pathsProvider()->profilePath() + QLatin1String("icons"));
	result += getSubDirs(Application::instance()->pathsProvider()->dataPath() + QLatin1String("themes/icons"));

	return result;
}

bool IconThemeManager::isValidThemePath(const QString &themePath) const
{
	QString kaduIconFileName = themePath + "/kadu_icons/64x64/kadu.png";
	QFileInfo kaduIconFile(kaduIconFileName);

	return kaduIconFile.exists();
}

#include "moc_icon-theme-manager.cpp"
