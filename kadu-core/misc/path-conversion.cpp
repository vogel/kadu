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
#include <QtGui/QApplication>

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
	return QLatin1String(KADU_DESKTOP_FILE_PATH);
}
#endif

QString homePath()
{
	static QString path;
	if (path.isNull())
	{
#ifdef Q_OS_WIN
		// on win32 dataPath doesn't need real argv[0] so it's safe to use this
		// in such ugly way
		// TODO review this usbinst thing
		if (QFile::exists(dataPath("usbinst", "")))
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
		// on win32 dataPath doesn't need real argv[0] so it's safe to use this
		// in such ugly way
		if (config_dir.isEmpty() && QFile::exists(dataPath("usbinst", "")))
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

static QString lib_path;
static QString data_path;

QString libPath(const QString &f)
{
#ifdef Q_OS_WIN
	QString fp=f;
	if (fp.startsWith(QLatin1String("kadu")))
		fp.remove(0, 4);
	return lib_path + fp;
#else
	return lib_path + f;
#endif
}

QString dataPath(const QString &p, const char *argv0)
{
	QString path = p;

	if (argv0 != 0)
	{
#ifdef Q_OS_MAC
		QString appPath = qApp->applicationDirPath();
		if (appPath.isEmpty())
		{
			fprintf(stderr, "we've got real problem here ;)\n");
			fflush(stderr);
			exit(10);
		}
		else
		{
			data_path = appPath + "/../../";
			lib_path = appPath + "/../../";
		}
#elif defined(Q_OS_WIN)
		WCHAR epath[MAX_PATH+1];
		GetModuleFileNameW(NULL, epath, MAX_PATH);

		data_path = QString::fromUtf16((const ushort*)epath);
		data_path.resize(data_path.lastIndexOf('\\') + 1);
		lib_path = data_path;
#else
		QString datadir(KADU_DATADIR);
		QString bindir(KADU_BINDIR);
		QString libdir(KADU_LIBDIR);

		//jeżeli ścieżki nie kończą się na /share i /bin oraz gdy bez tych końcówek
		//ścieżki się nie pokrywają, to znaczy, że ktoś ustawił ręcznie KADU_DATADIR lub KADU_BINDIR
		if (!datadir.endsWith(QLatin1String("/share")) || !bindir.endsWith(QLatin1String("/bin")) || !libdir.endsWith(QLatin1String("/lib")) ||
			datadir.left(datadir.length() - 6) != bindir.left(bindir.length() - 4) ||
			bindir.left(bindir.length() - 4) != libdir.left(libdir.length() - 4))
		{
			data_path = datadir + '/';
			lib_path = libdir + '/';
		}
		else
		{
			QString appPath = qApp->applicationDirPath();
			if (appPath.isEmpty())
			{
				data_path = datadir + '/';
				lib_path = libdir + '/';
			}
			else
			{
				data_path = appPath + "/../share/";
				lib_path = appPath + "/../lib/";
			}
		}
#endif
		QDir dataDir(data_path);
		QDir libDir(lib_path);

		data_path = dataDir.canonicalPath() + '/';
		lib_path = libDir.canonicalPath() + '/';

		Parser::GlobalVariables["DATA_PATH"] = data_path;
		Parser::GlobalVariables["LIB_PATH"] = lib_path;
	}
	if (data_path.isEmpty())
	{
		kdebugm(KDEBUG_PANIC, "dataPath() called _BEFORE_ initial dataPath(\"\",argv[0]) (static object uses dataPath()?) !!!\n");
		printBacktrace("dataPath(): constructor of static object uses dataPath");
	}

#ifdef Q_OS_WIN
	// on windows remove kadu from path
	if (path.startsWith(QLatin1String("kadu")))
		path.remove(0, 4);
#endif

	//kdebugm(KDEBUG_INFO, "%s%s\n", qPrintable(data_path), qPrintable(path));

	return data_path + path;
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
