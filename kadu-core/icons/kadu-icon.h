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

#ifndef KADU_ICON_H
#define KADU_ICON_H

#include <QtCore/QString>
#include <QtGui/QIcon>

#include "exports.h"

class KaduIcon;

/**
 * @addtogroup Icons
 * @{
 */

/**
 * @class KaduIconThemeChangeWatcher
 * @author Bartosz 'beevvy' Brachaczek
 * @short QObject used to notify KaduIcon about IconsManager::themeChanged() signal.
 * 
 * This class is needed to notify KaduIcon about IconsManager::themeChanged() signal.
 * KaduIcon itself cannot be QObject (QObjects lack copy constructors, etc.).
 */
class KADUAPI KaduIconThemeChangeWatcher : public QObject
{
	Q_OBJECT

	const KaduIcon *Icon;

private slots:
	void themeChanged() const;

public:
	explicit KaduIconThemeChangeWatcher(const KaduIcon *icon);
	virtual ~KaduIconThemeChangeWatcher();

};

/**
 * @class KaduIcon
 * @author Bartosz 'beevvy' Brachaczek
 * @short Object that stores Kadu-specific information about icon and allows for retrieving the icon itself.
 * 
 * Objects of this class store Kadu-specific information about icons and allow for retrieving the icons
 * themselves, as well as their paths and other IconsManager-related parameters (i.e., size and name)
 * and also full system paths and WebKit-friendly paths.
 * 
 * Creating objects of this class is relatively cheap as no data is verified or additionally retrieved
 * in the constructor. When the user ask for QIcon object or full path to icon file for the first time,
 * that data is retrieved by this time and cached for future use.
 * 
 * KaduIcon is aware of IconsManager::themeChanged() signals and deletes any cache it might have when
 * a them change occurs.
 * 
 * It is recommended for use everywhere using icons provided by IconsManager. It saves direct calls to
 * IconsManager and helps managing icons, especially when one wants to store for example both the icon
 * path and QIcon object.
 */
class KADUAPI KaduIcon
{
	QString Path;
	QString IconSize;
	QString IconName;
	mutable QString FullPath;
	mutable QIcon Icon;

	KaduIconThemeChangeWatcher Watcher;

public:
	KaduIcon();
	explicit KaduIcon(const QString &path, const QString &size = QString(), const QString &name = QString());
	KaduIcon(const KaduIcon &copyMe);

	KaduIcon & operator = (const KaduIcon &copyMe);

	bool isNull() const;

	void clearCache() const;

	/**
	 * @author Bartosz 'beevvy' Brachaczek
	 * @short Returns path to this icon.
	 * @return path to this icon
	 * 
	 * Returns path to this icon. Conforms to the meaning of first paramater
	 * of IconsManager::iconPath() method.
	 */
	const QString & path() const { return Path; }
	void setPath(const QString &path);

	/**
	 * @author Bartosz 'beevvy' Brachaczek
	 * @short Returns requested size of file pointed by fullPath() and webKitPath(), if set.
	 * @return size as string formatted like "WIDTHxHEIGHT"
	 * 
	 * Returns requested size of file pointed by fullPath() and webKitPath(), if set.
	 * Conforms to the meaning of second paramater of IconsManager::iconPath() method.
	 */
	const QString & size() const { return IconSize; }
	void setSize(const QString &size);

	/**
	 * @author Bartosz 'beevvy' Brachaczek
	 * @short Returns name of this icon, if it was set separately from path.
	 * @return name of this icon
	 * 
	 * Returns name of this icon, if it was set separately from path. Conforms to the meaning
	 * of third paramater of IconsManager::iconPath() method.
	 */
	const QString & name() const { return IconName; }
	void setName(const QString &name);

	const QString & fullPath() const;
	QString webKitPath() const;
	const QIcon & icon() const;

};

/**
 * @}
 */

#endif // KADU_ICON_H
