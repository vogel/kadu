/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef PATH_CONVERSION_H
#define PATH_CONVERSION_H

#include <QtCore/QString>
#include <QtCore/QtGlobal>

#include "exports.h"

#ifdef Q_WS_X11
/**
 * @short Returns absolute path to the .desktop file owned by Kadu.
 * @return absolute path to the .desktop file owned by Kadu
 *
 * This function returns absolute path to the .desktop file owned by Kadu.
 * First it gets path relative to the binary from the configuration written
 * at compilation time, and then constructs and returns absolute canonical
 * path from it.
 */
KADUAPI QString desktopFilePath();
#endif

/**
 * @short Returns roaming persistent data storage path on Windows and QDir::homePath() on other platforms.
 * @return absolute path to roaming persistent data storage path on Windows and QDir::homePath() on other platforms
 *
 * This functions returns roaming persistent data storage path (%AppData%) on Windows and QDir::homePath() on other platforms.
 */
KADUAPI QString homePath();

/**
 * @short Creates and returns absolute path to the current user profile.
 * @param subpath subpath that will be appended to the result
 * @return absolute path to the current user profile concatenated with @p subpath
 *
 * This function constructs absolute path to the current user profile
 * on its first run. By default it is $HOME/.kadu on X11, $HOME/Library/Kadu
 * on Mac OS X, and %AppData%/Kadu on Windows. Then it creates if it does not
 * exist, caches it (and never calculates it again) and returns it concatenated
 * with the @p subpath argument.
 *
 * The default path to the current user profile may overwritten by existence
 * of file named "portable" in directory returned by dataPath() function. Then
 * current user profile is located in the same location as dataPath("config") call
 * would return. It may be also overwritten by CONFIG_DIR environment variable
 * and --config-dir command-line option (the latter takes precedence). We will
 * call it CONFIG_DIR in this description, not matter which way it was set.
 * If CONFIG_DIR begins with a "./" (".\" is also supported on Windows), it is
 * treated as relative to the application's current directory. Else, if it is
 * a absolute path, it is treated literally. Else it is treated as relative to
 * the path returned by homePath() function or, if a file named "portable"
 * in directory returned by dataPath() function exists, it is treated as relative to
 * the path returned by dataPath() function.
 *
 * If CONFIG_DIR is used, if a middle directory named "kadu" on X11 or "Kadu" on Windows
 * and Mac OS X exists, it is used as the current user profile for compatibility
 * with Kadu 0.6.5 and older.
 */
KADUAPI QString profilePath(const QString &subpath = QString());

/**
 * @short Returns absolute path to the library path used by Kadu plugins.
 * @param subpath subpath that will be appended to the result
 * @return absolute path to the library path used by Kadu plugins
 *
 * This function constructs absolute path to the library path used by Kadu plugins.
 * First it gets path relative to the binary from the configuration written
 * at compilation time, and then constructs and caches (and never calculates
 * it again) absolute canonical path from it. Then it returns it concatenated
 * with the @p subpath argument.
 *
 * Default library path for plugins is $CMAKE_INSTALL_PREFIX/lib$LIB_SUFFIX/kadu/plugins
 * on X11 and $CMAKE_INSTALL_PREFIX/plugins on Windows. It can be overwritten by
 * KADU_PLUGINS_LIBDIR CMake argument.
 */ 
KADUAPI QString pluginsLibPath(const QString &subpath = QString());

/**
 * @short Returns absolute path to the data path used by Kadu.
 * @param subpath subpath that will be appended to the result
 * @return absolute path to the data path used by Kadu
 *
 * This function constructs absolute path to the data path used by Kadu.
 * First it gets path relative to the binary from the configuration written
 * at compilation time, and then constructs and caches (and never calculates
 * it again) absolute canonical path from it. Then it returns it concatenated
 * with the @p subpath argument.
 *
 * Default data path is $CMAKE_INSTALL_PREFIX/share/kadu on X11 and
 * $CMAKE_INSTALL_PREFIX on Windows. It can be overwritten by
 * KADU_DATADIR CMake argument.
 */
KADUAPI QString dataPath(const QString &subpath = QString());

/**
 * @short Returns fixed path ready to use with WebKit.
 * @param path path to be fixed
 * @return fixed @p path ready to use with WebKit
 * @todo Check whether this is really needed. Maybe all paths should start with "file://" and it could work everywhere?
 *
 * This function tries to fix path passed in @p path argument to be ready
 * to use with WebKit. Basically on Windows it tries to remove any "file://" and "file:///"
 * prefixes while on other platforms it ensures they actually begin with "file:///".
 */
KADUAPI QString webKitPath(const QString &path);

#endif // PATH_CONVERSION_H
