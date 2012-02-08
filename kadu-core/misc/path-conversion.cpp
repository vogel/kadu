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

#include <cstdio>

#include <QtCore/QDir>
#include <QtCore/QtGlobal>
#include <QtCore/QCoreApplication>

#ifdef Q_OS_WIN
#include <QFile>
#include <shlobj.h>
#include <windows.h>
#endif

#include "parser/parser.h"
#include "debug.h"
#include "kadu-config.h"

#include "path-conversion.h"

#if HAVE_EXECINFO
#include <execinfo.h>
#endif

void printBacktrace(const QString &header)
{
	if (header.isEmpty())
		fprintf(stderr, "\nbacktrace:\n");
	else
		fprintf(stderr, "\nbacktrace: ('%s')\n", qPrintable(header));
#if HAVE_EXECINFO
	void *bt_array[100];
	char **bt_strings;
	int num_entries;
	if ((num_entries = backtrace(bt_array, 100)) < 0) {
		fprintf(stderr, "could not generate backtrace\n");
		return;
	}
	if ((bt_strings = backtrace_symbols(bt_array, num_entries)) == NULL) {
		fprintf(stderr, "could not get symbol names for backtrace\n");
		return;
	}
	fprintf(stderr, "======= BEGIN OF BACKTRACE =====\n");
	for (int i = 0; i < num_entries; ++i)
		fprintf(stderr, "[%d] %s\n", i, bt_strings[i]);
	fprintf(stderr, "======= END OF BACKTRACE  ======\n");
	free(bt_strings);
#else
	fprintf(stderr, "backtrace not available\n");
#endif
	fflush(stderr);
}

#ifdef Q_WS_X11
QString desktopFilePath()
{
	return QCoreApplication::applicationDirPath() + QLatin1String("/" KADU_DESKTOP_FILE_PATH_RELATIVE_TO_BIN);
}
#endif

QString homePath()
{
	static QString path;
	if (path.isNull())
	{
#ifdef Q_OS_WIN
		// TODO review this usbinst thing
		if (QFile::exists(dataPath("usbinst")))
			path = dataPath("config/");
		else
		{
			WCHAR homepath[MAX_PATH + 1];
			// there is unfortunately no way to get this path from Qt4 API
			if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, homepath)))
				path = QString::fromUtf16((const ushort *)homepath);
			else
				path = QDir::homePath();
		}
#else
		path = QDir::homePath();
#endif

		Parser::GlobalVariables["HOME"] = path;
	}

	return path;
}

QString profilePath(const QString &subpath)
{
	static QString path;
	if (path.isNull())
	{
#ifndef Q_OS_WIN
		QString config_dir = QString::fromLocal8Bit(getenv("CONFIG_DIR"));
#else
		QString config_dir;
		char buff[1024] = { 0 };
		if (GetEnvironmentVariable("CONFIG_DIR", buff, sizeof(buff) - 1) > 0)
			config_dir = buff;
#endif

		QString home = homePath();

#ifdef Q_OS_WIN
		if (config_dir.isEmpty() && QFile::exists(dataPath("usbinst")))
		{
			path = home;
			Parser::GlobalVariables["KADU_CONFIG"] = path;
			return path + subpath;
		}
#endif

		QString pwd = QDir::currentPath();

#ifdef Q_OS_MAC
		if (config_dir.isEmpty())
			path = QString("%1/Library/Kadu/").arg(home);
		else if (config_dir.startsWith("./"))
		{
			config_dir = config_dir.right(config_dir.length() - 2);
			if (QDir(QString("%1/%2/Kadu").arg(pwd).arg(config_dir)).exists())
				path = QString("%1/%2/Kadu/").arg(pwd).arg(config_dir); // compatibility with earlier versions
			else
				path = QString("%1/%2/").arg(pwd).arg(config_dir);
		}
		else if (QDir(config_dir).isAbsolute())
		{
			if (QDir(QString("%1/Kadu").arg(config_dir)).exists())
				path = QString("%1/Kadu/").arg(config_dir); // compatibility with earlier versions
			else
				path = QString("%1/").arg(config_dir);
		}
		else
		{
			if (QDir(QString("%1/%2/Kadu").arg(home).arg(config_dir)).exists())
				path = QString("%1/%2/Kadu/").arg(home).arg(config_dir); // compatibility with earlier versions
			else
				path = QString("%1/%2/").arg(home).arg(config_dir);
		}
#elif defined(Q_OS_WIN)
		if (config_dir.isEmpty())
			path = QString("%1/Kadu/").arg(home);
		else if (config_dir.startsWith("./") || config_dir.startsWith(".\\"))
		{
			config_dir = config_dir.right(config_dir.length() - 2);
			if (QDir(QString("%1/%2/Kadu").arg(pwd).arg(config_dir)).exists())
				path = QString("%1/%2/Kadu/").arg(pwd).arg(config_dir); // compatibility with earlier versions
			else
				path = QString("%1/%2/").arg(pwd).arg(config_dir);
		}
		else if (QDir(config_dir).isAbsolute())
		{
			if (QDir(QString("%1/Kadu").arg(config_dir)).exists())
				path = QString("%1/Kadu/").arg(config_dir); // compatibility with earlier versions
			else
				path = QString("%1/").arg(config_dir);
		}
		else
		{
			if (QDir(QString("%1/%2/Kadu").arg(home).arg(config_dir)).exists())
				path = QString("%1/%2/Kadu/").arg(home).arg(config_dir); // compatibility with earlier versions
			else
				path = QString("%1/%2/").arg(home).arg(config_dir);
		}
#else
		if (config_dir.isEmpty())
			path = QString("%1/.kadu/").arg(home);
		else if (config_dir.startsWith("./"))
		{
			config_dir = config_dir.right(config_dir.length() - 2);
			if (QDir(QString("%1/%2/kadu").arg(pwd).arg(config_dir)).exists())
				path = QString("%1/%2/kadu/").arg(pwd).arg(config_dir); // compatibility with earlier versions
			else
				path = QString("%1/%2/").arg(pwd).arg(config_dir);
		}
		else if (QDir(config_dir).isAbsolute())
		{
			if (QDir(QString("%1/kadu").arg(config_dir)).exists())
				path = QString("%1/kadu/").arg(config_dir); // compatibility with earlier versions
			else
				path = QString("%1/").arg(config_dir);
		}
		else
		{
			if (QDir(QString("%1/%2/kadu").arg(home).arg(config_dir)).exists())
				path = QString("%1/%2/kadu/").arg(home).arg(config_dir); // compatibility with earlier versions
			else
				path = QString("%1/%2/").arg(home).arg(config_dir);
		}
#endif

		Parser::GlobalVariables["KADU_CONFIG"] = path;
	}

	return path + subpath;
}

static QString pluginsLibDir;
static QString data_path;

QString pluginsLibPath(const QString &f)
{
	return pluginsLibDir + f;
}

QString dataPath(const QString &p)
{
	if (data_path.isNull())
	{
#ifdef Q_OS_MAC
		QString appDir = QCoreApplication::applicationDirPath();
		if (appDir.isEmpty())
		{
			fprintf(stderr, "we've got real problem here ;)\n");
			fflush(stderr);
			exit(10);
		}
		else
		{
			data_path = appDir + "/../../kadu";
			pluginsLibDir = appDir + "/../../kadu/plugins/";
		}
#elif defined(Q_OS_WIN)
		WCHAR epath[MAX_PATH+1];
		GetModuleFileNameW(NULL, epath, MAX_PATH);

		data_path = QString::fromUtf16((const ushort*)epath);
		data_path.resize(data_path.lastIndexOf('\\') + 1);
		pluginsLibDir = data_path + "/plugins";
#else
		QString appDir = QCoreApplication::applicationDirPath();
		data_path = appDir + QLatin1String("/" KADU_DATADIR_RELATIVE_TO_BIN);
		pluginsLibDir = appDir + QLatin1String("/" KADU_PLUGINS_LIBDIR_RELATIVE_TO_BIN);
#endif
		QDir dataDir(data_path);
		QDir libDir(pluginsLibDir);

		data_path = dataDir.canonicalPath() + '/';
		pluginsLibDir = libDir.canonicalPath() + '/';

		Parser::GlobalVariables["DATA_PATH"] = data_path;
	}

	return data_path + p;
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
