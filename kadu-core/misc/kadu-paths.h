/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef KADU_PATHS_H
#define KADU_PATHS_H

#include <QtCore/QString>
#include <QtCore/QtGlobal>

#include "exports.h"

/**
 * @class KaduPaths
 * @short Singleton which holds information about paths used by Kadu.
 *
 * This singleton class holds information about paths used by Kadu. It constructs and caches them
 * upon class instance creation, and path to the current user profile is created if it does not
 * exist yet. Details are described in the individual getter methods.
 */
class KADUAPI KaduPaths
{
	Q_DISABLE_COPY(KaduPaths)

	static KaduPaths *Instance;

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
	QString DesktopFilePath;
#endif
	QString ProfilePath;
	QString PluginsLibPath;
	QString DataPath;

	KaduPaths();

	void initBasicPaths();
	void initProfilePath();

public:
	static void createInstance();
	static void destroyInstance();
	static KaduPaths * instance() { return Instance; }

	/**
	 * @short Returns roaming persistent data storage path on Windows and QDir::homePath() on other platforms.
	 * @return absolute path to roaming persistent data storage path on Windows and QDir::homePath() on other platforms
	 *
	 * This functions returns roaming persistent data storage path (%AppData%) on Windows and QDir::homePath() on other platforms.
	 */
	static QString homePath();

	/**
	 * @short Returns fixed path ready to use with WebKit.
	 * @param path path to be fixed
	 * @return fixed path @p path ready to use with WebKit
	 * @todo Check whether this is really needed. Maybe all paths should start with "file://" and it could work everywhere?
	 *
	 * This function tries to fix path passed in @p path argument to be ready
	 * to use with WebKit. Basically on Windows it tries to remove any "file://" and "file:///"
	 * prefixes while on other platforms it ensures that the path actually begins with "file:///".
	 */
	static QString webKitPath(const QString &path);

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
	/**
	 * @short Returns absolute path to the .desktop file owned by Kadu.
	 * @return absolute path to the .desktop file owned by Kadu
	 *
	 * This function returns absolute path to the .desktop file owned by Kadu.
	 *
	 * Path construction at init:
	 * First desktop file path relative to the binary is taken from the configuration
	 * written at compilation time, and then absolute canonical path is constructed from it.
	 *
	 * Default desktop file path is $CMAKE_INSTALL_FULL_DATADIR/applications/kadu.desktop.
	 * It can be overwritten by KADU_DESKTOP_FILE_DIR and KADU_DESKTOP_FILE_NAME CMake
	 * arguments.
	 */
	const QString & desktopFilePath() const { return DesktopFilePath; }
#endif

	/**
	 * @short Returns absolute path to the data path used by Kadu.
	 * @return absolute path to the data path used by Kadu, ended with a trailing slash
	 *
	 * This function returns absolute path to the data path used by Kadu, ended with a trailing slash.
	 *
	 * Path construction at init:
	 * First Kadu data path relative to the binary is taken from the configuration
	 * written at compilation time, and then absolute canonical path is constructed from it.
	 *
	 * Default data path is $CMAKE_INSTALL_FULL_DATADIR/kadu on X11 and
	 * $CMAKE_INSTALL_PREFIX on Windows. It can be overwritten by
	 * KADU_INSTALL_DATA_DIR CMake argument.
	 */
	const QString & dataPath() const { return DataPath; }

	/**
	 * @short Returns absolute path to the library path used by Kadu plugins.
	 * @return absolute path to the library path used by Kadu plugins, ended with a trailing slash
	 *
	 * This function returns absolute path to the library path used by Kadu plugins, ended with a trailing slash.
	 *
	 * Path construction at init:
	 * First plugins library path relative to the binary is taken from the configuration
	 * written at compilation time, and then absolute canonical path is constructed from it.
	 *
	 * Default library path for plugins is $CMAKE_INSTALL_FULL_LIBDIR/kadu/plugins
	 * on X11 and $CMAKE_INSTALL_PREFIX/plugins on Windows. It can be overwritten by
	 * KADU_INSTALL_PLUGINS_LIB_DIR CMake argument.
	 */ 
	const QString & pluginsLibPath() const { return PluginsLibPath; }

	/**
	 * @short Creates and returns absolute path to the current user profile.
	 * @return absolute path to the current user profile, ended with a trailing slash
	 *
	 * This function returns absolute path to the current user profile, ended with a trailing slash.
	 *
	 * Path construction at init:
	 * Absolute path to the current user profile is constructed at init. By default it is
	 * $HOME/.kadu on X11, $HOME/Library/Kadu on Mac OS X, and %AppData%/Kadu on Windows.
	 * If this path does not exist yet, it is craeted and 0700 (or equivalent on Windwow)
	 * permissions are set.
	 *
	 * The default path to the current user profile may overwritten by existence
	 * of file named "portable" in directory returned by the dataPath() method. Then
	 * current user profile is located in the same location as KaduPaths::instance()->dataPath() + QLatin1String("config")
	 * would give. It may be also overwritten by CONFIG_DIR environment variable
	 * and --config-dir command-line option (the latter takes precedence). We will
	 * call it CONFIG_DIR in this description, not matter which way it was set.
	 * If CONFIG_DIR begins with a "./" (".\" is also supported on Windows), it is
	 * treated as relative to the application's current directory. Else, if it is
	 * a absolute path, it is treated literally. Else it is treated as relative to
	 * the path returned by the homePath() method or, if a file named "portable"
	 * in directory returned by the dataPath() method exists, it is treated as relative to
	 * the path returned by the dataPath() method.
	 *
	 * If CONFIG_DIR is used and a middle directory named "kadu" on X11 or "Kadu" on Windows
	 * and Mac OS X exists, this directory is used as the current user profile for compatibility
	 * with Kadu 0.6.5 and older.
	 */
	const QString & profilePath() const { return ProfilePath; }

};

#endif // KADU_PATHS_H
