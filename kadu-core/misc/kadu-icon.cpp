/*
 * %kadu copyright begin%
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

#include "icons-manager.h"
#include "misc/path-conversion.h"

#include "kadu-icon.h"

KaduIconThemeChangeWatcher::KaduIconThemeChangeWatcher(const KaduIcon *icon) :
		QObject(), Icon(icon)
{
	connect(IconsManager::instance(), SIGNAL(themeChanged()), this, SLOT(themeChanged()));
}

KaduIconThemeChangeWatcher::~KaduIconThemeChangeWatcher()
{
}

void KaduIconThemeChangeWatcher::themeChanged() const
{
	Icon->clearCache();
}

KaduIcon::KaduIcon() :
		Watcher(this)
{
}

KaduIcon::KaduIcon(const QString &path, const QString &size, const QString &name) :
		Path(path), IconSize(size), IconName(name), Watcher(this)
{
}

KaduIcon::KaduIcon(const KaduIcon &copyMe) :
		Path(copyMe.Path), IconSize(copyMe.IconSize), IconName(copyMe.IconName),
		FullPath(copyMe.FullPath), Icon(copyMe.Icon), Watcher(this)
{
}

KaduIcon & KaduIcon::operator = (const KaduIcon &copyMe)
{
	Path = copyMe.Path;
	IconSize = copyMe.IconSize;
	IconName = copyMe.IconName;
	FullPath = copyMe.FullPath;
	Icon = copyMe.Icon;

	return *this;
}

bool KaduIcon::isNull() const
{
	return path().isEmpty();
}

void KaduIcon::clearCache() const
{
	FullPath.clear();
	Icon = QIcon();
}

void KaduIcon::setSize(const QString &size)
{
	if (IconSize != size)
	{
		IconSize = size;
		clearCache();
	}
}

void KaduIcon::setPath(const QString &path)
{
	if (Path != path)
	{
		Path = path;
		clearCache();

		// see comment to this method
		IconSize.clear();
		IconName.clear();
	}
}

void KaduIcon::setName(const QString &name)
{
	if (IconName != name)
	{
		IconName = name;
		clearCache();
	}
}

const QString & KaduIcon::fullPath() const
{
	// defer getting it until we need it for the first time
	if (FullPath.isEmpty())
	{
		if (!IconName.isEmpty())
			FullPath = IconsManager::instance()->iconPath(path(), IconSize, IconName);
		else if (!IconSize.isEmpty())
			FullPath = IconsManager::instance()->iconPath(path(), IconSize);
		else
			FullPath = IconsManager::instance()->iconPath(path());
	}

	return FullPath;
}

QString KaduIcon::webKitPath() const
{
	return ::webKitPath(fullPath());
}

const QIcon & KaduIcon::icon() const
{
	// defer getting it until we need it for the first time
	if (Icon.isNull())
		Icon = IconsManager::instance()->iconByPath(path());

	return Icon;
}
