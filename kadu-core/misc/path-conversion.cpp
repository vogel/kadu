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

#include "path-conversion.h"

#ifdef Q_WS_X11
QString desktopFilePath()
{
	static QString path;
	if (path.isNull())
	{
		path = QCoreApplication::applicationDirPath() + QLatin1String("/" KADU_DESKTOP_FILE_PATH_RELATIVE_TO_BIN);
		path = QFileInfo(path).canonicalFilePath();
	}

	return path;
}
#endif

QString homePath()
{
#ifdef Q_OS_WIN
	wchar_t homepath[MAX_PATH];

	// There is unfortunately no way to get this path using Qt4 API.
	if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, homepath)))
		return QDir(QString::fromWCharArray(homepath)).canonicalPath();
#endif

	return QDir::homePath();
}

QString profilePath(const QString &subpath)
{
	static QString path;
	if (path.isNull())
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

		QString customConfigDir = qgetenv("CONFIG_DIR");

		if (customConfigDir.isEmpty())
		{
			if (QFileInfo(dataPath(QLatin1String("portable"))).exists())
				path = dataPath(QLatin1String("config"));
			else
				path = homePath() + '/' + defaultConfigDirRelativeToHome;
		}
		else
		{
			customConfigDir += '/';

			if (customConfigDir.startsWith(QLatin1String("./"))
#ifdef Q_OS_WIN
					|| customConfigDir.startsWith(QLatin1String(".\\"))
#endif
					)
				path = QDir::currentPath() + '/' + customConfigDir;
			else if (QDir(customConfigDir).isAbsolute())
				path = customConfigDir;
			else if (QFileInfo(dataPath(QLatin1String("portable"))).exists())
				path = dataPath(customConfigDir);
			else
				path = homePath() + '/' + customConfigDir;

			// compatibility with 0.6.5 and older versions
			if (QDir(path + oldMidConfigDir).exists())
				path += oldMidConfigDir;
		}

		QDir profileDir(path);
		if (!profileDir.exists())
		{
			profileDir.mkpath(QLatin1String("."));
			// This equals to 0700 on Unix-like.
			QFile(path).setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner | QFile::ReadUser | QFile::WriteUser | QFile::ExeUser);
		}

		QString canonicalPath = profileDir.canonicalPath();
		if (!canonicalPath.isEmpty())
			path = canonicalPath + '/';
	}

	return path + subpath;
}

QString pluginsLibPath(const QString &subpath)
{
	static QString pluginsLibDir;
	if (pluginsLibDir.isNull())
	{
#ifdef Q_OS_MAC
		// TODO: Remove this OS X-specific code. Needs small changes to CMake and create_macosx_bundle.sh scripts.
		pluginsLibDir = QCoreApplication::applicationDirPath() + QLatin1String("/../../kadu/plugins/");
#else
		pluginsLibDir = QCoreApplication::applicationDirPath() + QLatin1String("/" KADU_PLUGINS_LIBDIR_RELATIVE_TO_BIN);
#endif

		QString canonicalPath = QDir(pluginsLibDir).canonicalPath();
		if (!canonicalPath.isEmpty())
			pluginsLibDir = canonicalPath + '/';
	}

	return pluginsLibDir + subpath;
}

QString dataPath(const QString &subpath)
{
	static QString dataDir;
	if (dataDir.isNull())
	{
#ifdef Q_OS_MAC
		// TODO: Remove this OS X-specific code. Needs small changes to CMake and create_macosx_bundle.sh scripts.
		dataDir = QCoreApplication::applicationDirPath() + QLatin1String("/../../kadu");
#else
		dataDir = QCoreApplication::applicationDirPath() + QLatin1String("/" KADU_DATADIR_RELATIVE_TO_BIN);
#endif

		QString canonicalPath = QDir(dataDir).canonicalPath();
		if (!canonicalPath.isEmpty())
			dataDir = canonicalPath + '/';
	}

	return dataDir + subpath;
}

QString webKitPath(const QString &path)
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
