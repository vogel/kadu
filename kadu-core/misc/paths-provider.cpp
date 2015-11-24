/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#ifdef Q_OS_WIN
#include <shlobj.h>
#include <windows.h>
#endif

#include "kadu-config.h"

#include "paths-provider.h"

QString PathsProvider::homePath()
{
#ifdef Q_OS_WIN
	wchar_t homepath[MAX_PATH];

	// There is unfortunately no way to get this path using Qt4 API.
	if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, homepath)))
		return QDir(QString::fromWCharArray(homepath)).canonicalPath();
#endif

	return QDir::homePath();
}

QString PathsProvider::webKitPath(const QString &path)
{
	if (path.isEmpty())
		return path;
	if (path.startsWith(QLatin1String("file:///")))
		return path;
	if (path.startsWith('/'))
		return "file://" + path;
	return "file:///" + path;
}

PathsProvider::PathsProvider(const QString &customProfileDir, QObject *parent) :
		QObject{parent}
{
	initBasicPaths();
	initProfilePath(customProfileDir);
}

PathsProvider::~PathsProvider()
{
}

void PathsProvider::initBasicPaths()
{
#if defined(Q_OS_UNIX)
	DesktopFilePath = QCoreApplication::applicationDirPath() + QLatin1String("/" KADU_DESKTOP_FILE_PATH_RELATIVE_TO_BIN);
	DesktopFilePath = QFileInfo(DesktopFilePath).canonicalFilePath();
#endif

	DataPath = QCoreApplication::applicationDirPath() + QLatin1String("/" KADU_DATADIR_RELATIVE_TO_BIN);
	QString canonicalPath = QDir(DataPath).canonicalPath();
	if (!canonicalPath.isEmpty())
		DataPath = canonicalPath + '/';

	PluginsLibPath = QCoreApplication::applicationDirPath() + QLatin1String("/" KADU_PLUGINS_LIBDIR_RELATIVE_TO_BIN);
	canonicalPath = QDir(PluginsLibPath).canonicalPath();
	if (!canonicalPath.isEmpty())
		PluginsLibPath = canonicalPath + '/';
}

void PathsProvider::initProfilePath(const QString &customProfileDir)
{
#if defined(Q_OS_WIN)
	const QString defaultConfigDirRelativeToHome = QLatin1String("Kadu");
	const QString &oldMidConfigDir = defaultConfigDirRelativeToHome;
#else
	const QString defaultConfigDirRelativeToHome = QLatin1String(".kadu");
	const QString oldMidConfigDir = QLatin1String("kadu");
#endif

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
