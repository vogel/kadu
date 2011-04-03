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

KaduIcon::KaduIcon(const QString &path, const QString &size, const QString &name) :
		Path(path), IconSize(size), IconName(name)
{
}

bool KaduIcon::isNull() const
{
	return path().isEmpty();
}

void KaduIcon::setSize(const QString &size)
{
	if (IconSize != size)
	{
		IconSize = size;

		// clear cache
		FullPath.clear();
		Icon = QIcon();
	}
}

void KaduIcon::setPath(const QString &path)
{
	if (Path != path)
	{
		Path = path;

		// see comment to this method
		IconSize.clear();
		IconName.clear();

		// clear cache
		FullPath.clear();
		Icon = QIcon();
	}
}

void KaduIcon::setName(const QString &name)
{
	if (IconName != name)
	{
		IconName = name;

		// clear cache
		FullPath.clear();
		Icon = QIcon();
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
