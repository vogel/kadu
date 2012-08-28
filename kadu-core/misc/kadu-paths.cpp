/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QCoreApplication>

#ifdef Q_OS_WIN
#include <shlobj.h>
#include <windows.h>
#endif

#include "kadu-config.h"

#include "kadu-paths.h"

KaduPaths * KaduPaths::Instance;

void KaduPaths::createInstance()
{
	if (!Instance)
		Instance = new KaduPaths();
}

void KaduPaths::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

QString KaduPaths::homePath()
{
#ifdef Q_OS_WIN
	wchar_t homepath[MAX_PATH];

	// There is unfortunately no way to get this path using Qt4 API.
	if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, homepath)))
		return QDir(QString::fromWCharArray(homepath)).canonicalPath();
#endif

	return QDir::homePath();
}

QString KaduPaths::webKitPath(const QString &path)
{
#ifdef Q_OS_WIN
	QString winPath = path;
	if (winPath.startsWith(QLatin1String("file:///")))
		return winPath.remove(0, 8);
	if (winPath.startsWith(QLatin1String("file://")))
		return winPath.remove(0, 7);
	return winPath;
#else
	if (path.isEmpty())
		return path;
	if (path.startsWith(QLatin1String("file:///")))
		return path;
	if (path.startsWith('/'))
		return "file://" + path;
	return "file:///" + path;
#endif
}

KaduPaths::KaduPaths()
{
	initBasicPaths();
	initProfilePath();
}

void KaduPaths::initBasicPaths()
{
#ifdef Q_WS_X11
	DesktopFilePath = QCoreApplication::applicationDirPath() + QLatin1String("/" KADU_DESKTOP_FILE_PATH_RELATIVE_TO_BIN);
	DesktopFilePath = QFileInfo(DesktopFilePath).canonicalFilePath();
#endif

#ifdef Q_OS_MAC
	// TODO: Remove this OS X-specific code. Needs small changes to CMake and create_macosx_bundle.sh scripts.
	DataPath = QCoreApplication::applicationDirPath() + QLatin1String("/../../kadu");
#else
	DataPath = QCoreApplication::applicationDirPath() + QLatin1String("/" KADU_DATADIR_RELATIVE_TO_BIN);
#endif
	QString canonicalPath = QDir(DataPath).canonicalPath();
	if (!canonicalPath.isEmpty())
		DataPath = canonicalPath + '/';

#ifdef Q_OS_MAC
	// TODO: Remove this OS X-specific code. Needs small changes to CMake and create_macosx_bundle.sh scripts.
	PluginsLibPath = QCoreApplication::applicationDirPath() + QLatin1String("/../../kadu/plugins/");
#else
	PluginsLibPath = QCoreApplication::applicationDirPath() + QLatin1String("/" KADU_PLUGINS_LIBDIR_RELATIVE_TO_BIN);
#endif
	canonicalPath = QDir(PluginsLibPath).canonicalPath();
	if (!canonicalPath.isEmpty())
		PluginsLibPath = canonicalPath + '/';
}

void KaduPaths::initProfilePath()
{
#if defined(Q_OS_MAC)
	const QString defaultConfigDirRelativeToHome = QLatin1String("Library/Kadu");
	const QString oldMidConfigDir = QLatin1String("Kadu");
#elif defined(Q_OS_WIN)
	const QString defaultConfigDirRelativeToHome = QLatin1String("Kadu");
	const QString &oldMidConfigDir = defaultConfigDirRelativeToHome;
#else
	const QString defaultConfigDirRelativeToHome = QLatin1String(".kadu");
	const QString oldMidConfigDir = QLatin1String("kadu");
#endif

	QString customProfileDir = qgetenv("CONFIG_DIR");
	if (customProfileDir.isEmpty())
	{
		if (QFileInfo(dataPath() + QLatin1String("portable")).exists())
			ProfilePath = dataPath() + QLatin1String("config");
		else
			ProfilePath = homePath() + '/' + defaultConfigDirRelativeToHome;
	}
	else
	{
		if (customProfileDir.startsWith(QLatin1String("./"))
#ifdef Q_OS_WIN
				|| customProfileDir.startsWith(QLatin1String(".\\"))
#endif
				)
			ProfilePath = QDir::currentPath() + '/' + customProfileDir;
		else if (QDir(customProfileDir).isAbsolute())
			ProfilePath = customProfileDir;
		else if (QFileInfo(dataPath() + QLatin1String("portable")).exists())
			ProfilePath = dataPath() + customProfileDir;
		else
			ProfilePath = homePath() + '/' + customProfileDir;

		// compatibility with 0.6.5 and older versions
		if (QDir(ProfilePath + '/' + oldMidConfigDir).exists())
			ProfilePath += '/' + oldMidConfigDir;
	}

	// Do not cache QDir objects here unless you know what you are doing and
	// you have tested your changes thoroughly under Windows. Using official
	// Qt 4.8.1 MSVC 2010 build QDir thinks the dir does not exist if initially
	// it did not exist, even though we successfully call QDir::mkpath(). Hence
	// QDir::canonicalPath() returs empty string.
	if (!QDir(ProfilePath).exists())
	{
		QDir().mkpath(ProfilePath);
		// This equals to 0700 on Unix-like.
		QFile(ProfilePath).setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner | QFile::ReadUser | QFile::WriteUser | QFile::ExeUser);
	}

	QString canonicalPath = QDir(ProfilePath).canonicalPath();
	if (!canonicalPath.isEmpty())
		ProfilePath = canonicalPath;
	if (!ProfilePath.isEmpty() && !ProfilePath.endsWith(QLatin1String("/")))
		ProfilePath += '/';
}
