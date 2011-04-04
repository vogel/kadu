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

#include "icons/icons-manager.h"
#include "misc/path-conversion.h"

#include "kadu-icon.h"

/**
 * @author Bartosz 'beevvy' Brachaczek
 * @short Creates KaduIconThemeChangeWatcher object.
 * @param icon pointer to KaduIcon object
 * 
 * Creates KaduIconThemeChangeWatcher object and sets it up for notifying
 * provided KaduIcon object about theme changes.
 */
KaduIconThemeChangeWatcher::KaduIconThemeChangeWatcher(const KaduIcon *icon) :
		QObject(), Icon(icon)
{
	connect(IconsManager::instance(), SIGNAL(themeChanged()), this, SLOT(themeChanged()));
}

KaduIconThemeChangeWatcher::~KaduIconThemeChangeWatcher()
{
}

/**
 * @author Bartosz 'beevvy' Brachaczek
 * @short Clears cache in parent KaduIcon object.
 * 
 * Clears cache in parent KaduIcon object. Called when the theme changes.
 */
void KaduIconThemeChangeWatcher::themeChanged() const
{
	Icon->clearCache();
}

/**
 * @author Bartosz 'beevvy' Brachaczek
 * @short Creates null KaduIcon object.
 * 
 * Creates null KaduIcon object. isNull() will return true.
 */
KaduIcon::KaduIcon() :
		Watcher(this)
{
}

/**
 * @author Bartosz 'beevvy' Brachaczek
 * @short Creates KaduIcon object.
 * @param path path to this icon
 * @param size requested size as string formatted like "WIDTHxHEIGHT"
 * @param name name of this icon
 * 
 * Creates KaduIcon object from given path and optionally size and name.
 */
KaduIcon::KaduIcon(const QString &path, const QString &size, const QString &name) :
		Path(path), IconSize(size), IconName(name), Watcher(this)
{
}

/**
 * @author Bartosz 'beevvy' Brachaczek
 * @short Creates KaduIcon object with the same contents as the one passed in parameter.
 * @param copyMe another KaduIcon object
 * 
 * Creates KaduIcon object with the same contents as the one passed in parameter.
 */
KaduIcon::KaduIcon(const KaduIcon &copyMe) :
		Path(copyMe.Path), IconSize(copyMe.IconSize), IconName(copyMe.IconName),
		FullPath(copyMe.FullPath), Icon(copyMe.Icon), Watcher(this)
{
}

/**
 * @author Bartosz 'beevvy' Brachaczek
 * @short Sets all data to be the same as in the KaduIcon object passed in parameter.
 * @param copyMe another KaduIcon object
 * @return reference to this object
 * 
 * Sets all data to be the same as in the KaduIcon object passed in parameter.
 */
KaduIcon & KaduIcon::operator = (const KaduIcon &copyMe)
{
	Path = copyMe.Path;
	IconSize = copyMe.IconSize;
	IconName = copyMe.IconName;
	FullPath = copyMe.FullPath;
	Icon = copyMe.Icon;

	return *this;
}

/**
 * @author Bartosz 'beevvy' Brachaczek
 * @short Returns true if this object is null.
 * @return true if this object is null, false otherwise
 * 
 * Returns true if this object is null. Every object with empty path() is considered null.
 */
bool KaduIcon::isNull() const
{
	return path().isEmpty();
}

/**
 * @author Bartosz 'beevvy' Brachaczek
 * @short Clears any cache stored by this KaduIcon object.
 * 
 * Clears any cache stored by this KaduIcon object.
 */
void KaduIcon::clearCache() const
{
	FullPath.clear();
	Icon = QIcon();
}

/**
 * @author Bartosz 'beevvy' Brachaczek
 * @short Sets the requested size of icon file.
 * @param size requested size as string formatted like "WIDTHxHEIGHT"
 * 
 * Sets the requested size of icon file returned by fullPath() and webKitPath() methods.
 * The size has the same meaning as second parameter in IconsManager::iconPath() method.
 */
void KaduIcon::setSize(const QString &size)
{
	if (IconSize != size)
	{
		IconSize = size;
		clearCache();
	}
}

/**
 * @author Bartosz 'beevvy' Brachaczek
 * @short Sets path to this icon and resets all other data.
 * @param path path to this icon
 * 
 * Sets path to this icon and resets all other data. The path must have the same meaning
 * as first parameter in IconsManager::iconPath() method, i.e. it can be either relative or
 * absolute and gets special meaning when size or name are set.
 * 
 * @note It resets size and name because new path may contain them.
 * @todo Resetting size and name could be avoided if one fixed IconsManager::iconPath() method.
 */ 
void KaduIcon::setPath(const QString &path)
{
	if (Path != path)
	{
		Path = path;
		clearCache();

		// see documentation to this method
		IconSize.clear();
		IconName.clear();
	}
}

/**
 * @author Bartosz 'beevvy' Brachaczek
 * @short Sets name of this icon if it is not provided by path.
 * @param name name of this icon
 * 
 * Sets name of this icon if it is not provided by path(). Note that size() must
 * be set as well for the name to have any meaning. The name has the same meaning
 * as third parameter in IconsManager::iconPath() method.
 */
void KaduIcon::setName(const QString &name)
{
	if (IconName != name)
	{
		IconName = name;
		clearCache();
	}
}

/**
 * @author Bartosz 'beevvy' Brachaczek
 * @short Returns full path to the icon file.
 * @return full path to file
 * 
 * Returns full path to the icon file. Equivalent to calling IconsManager::iconPath()
 * with this object's path(), size() and name() as parameters (skip parameter if empty).
 */
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

/**
 * @author Bartosz 'beevvy' Brachaczek
 * @short Returns WebKit-friendly representation of full path to the icon file.
 * @return WebKit-friendly representation of path
 * 
 * Return WebKit-friendly representation of full path to the icon file. Makes use
 * of webKitPath() global Kadu function.
 */
QString KaduIcon::webKitPath() const
{
	return ::webKitPath(fullPath());
}

/**
 * @author Bartosz 'beevvy' Brachaczek
 * @short Returns QIcon object representing this icon.
 * @return QIcon object of this icon
 * 
 * Returns QIcon object representing this icon. Equivalent to calling
 * IconsManager::iconByPath() with path() as parameter.
 */
const QIcon & KaduIcon::icon() const
{
	// defer getting it until we need it for the first time
	if (Icon.isNull())
		Icon = IconsManager::instance()->iconByPath(path());

	return Icon;
}
