/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QtGlobal>

#ifdef Q_OS_WIN
#include <windows.h>
#include <shlobj.h>
#include <QFile>
#endif

#include "parser/parser.h"
#include "debug.h"
#include "kadu-config.h"

#include "path-conversion.h"

#ifndef Q_OS_WIN
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pwd.h>

/**
	sprawdza czy wskazana �cie�ka jest linkiem symbolicznym	i je�eli jest,
	to pr�buje wyci�gn�� �cie�k� na kt�r� wskazuje ten link
	zwraca b��d tylko gdy wyst�pi jaki� b��d przy wywo�ywaniu readlink (co wskazuje na jaki� powa�ny b��d)
	uwaga: je�eli pliku nie ma, to funkcja zwraca _sukces_, bo plik nie jest linkiem
 **/
static bool delinkify(char *path, int maxlen)
{
	kdebugmf(KDEBUG_FUNCTION_START, "%s\n", path);
	struct stat st;
	if (lstat(path, &st) == -1)
	{
		kdebugf2();
		return true;
	}
	kdebugm(KDEBUG_INFO, "mode: %o\n", st.st_mode);
	if (!S_ISLNK(st.st_mode))
	{
		kdebugf2();
		return true;
	}
	char *path2 = new char[maxlen];
	ssize_t bytesFilled = readlink(path, path2, maxlen - 1);
	if (bytesFilled == -1)
	{
		fprintf(stderr, "readlink error: '%s'\n", strerror(errno));
		fflush(stderr);
		delete [] path2;
		kdebugf2();
		return false;
	}
	path2[bytesFilled] = 0;
	memcpy(path, path2, bytesFilled + 1);
	delete [] path2;
	kdebugf2();
	return true;
}

/**
	funkcja poszukuje binarki programu na podstawie argv[0] oraz zmiennej PATH
	je�eli j� znajdzie, to zapisuje �cie�k� pod adres wskazany przez path
	(o maksymalnej d�ugo�ci len) oraz zwraca path, kt�ry zaka�czany jest znakiem '/'
	je�eli binarka nie zostanie znaleziona, to zwracany jest NULL
	w obu przypadkach gwarantowane jest, �e path ko�czy si� znakiem 0
	(len musi by� > 2)
**/
static char *findMe(const char *argv0, char *path, int len)
{
	kdebugf();
	struct stat buf;
	char *lastslash;

	char *current;
	char *previous;
	int l;


	if (argv0[0] == '.' && argv0[1] == '/') //�cie�ka wzgl�dem bie��cego katalogu (./)
	{
		if (getcwd(path, len - 2) == NULL)
		{
			path[0] = 0;
			kdebugf2();
			return NULL;
		}
		strncat(path, argv0 + 1, len - 1);
		path[len - 1] = 0;
		if (!delinkify(path, len))
		{
			kdebugf2();
			return NULL;
		}
		lastslash = strrchr(path, '/');
		lastslash[1] = 0;
		kdebugf2();
		return path;
	}

	if (argv0[0] == '.' && argv0[1] == '.' && argv0[2] == '/') //�cie�ka wzgl�dem bie��cego katalogu (../)
	{
		if (getcwd(path, len - 2)==NULL)
		{
			path[0] = 0;
			kdebugf2();
			return NULL;
		}
		strncat(path, "/", len - 1);
		strncat(path, argv0, len - 1);
		path[len - 1] = 0;
		if (!delinkify(path, len))
		{
			kdebugf2();
			return NULL;
		}
		lastslash = strrchr(path, '/');
		lastslash[1] = 0;
		kdebugf2();
		return path;
	}

	if (argv0[0] == '/') //�cie�ka bezwzgl�dna
	{
		strncpy(path, argv0, len - 1);
		path[len - 1] = 0;
		if (!delinkify(path, len))
		{
			kdebugf2();
			return NULL;
		}
		lastslash = strrchr(path, '/');
		lastslash[1] = 0;
		kdebugf2();
		return path;
	}

	previous = getenv("PATH"); //szukamy we wszystkich katalogach, kt�re s� w PATH
	while((current = strchr(previous, ':')))
	{
		l = current - previous;
		if (l > len - 2)
		{
			path[0] = 0;
			kdebugf2();
			return NULL;
		}

		memcpy(path, previous, l);
		path[l] = '/';
		path[l + 1] = 0;
		strncat(path, argv0, len);
		path[len - 1] = 0;
		if (!delinkify(path, len))
		{
			kdebugf2();
			return NULL;
		}
		if (stat(path, &buf) != -1)
		{
			if (path[l - 1] == '/')
				path[l] = 0;
			else
				path[l + 1] = 0;
			kdebugf2();
			return path;
		}
		previous = current + 1;
	}
	//nie znale�li�my dot�d (bo szukali�my ':'), wi�c mo�e w pozosta�ej cz��ci co� si� znajdzie?
	strncpy(path, previous, len - 2);
	path[len - 2] = 0;

	l = strlen(path);
	path[l] = '/';
	path[l + 1] = 0;
	strncat(path, argv0, len);
	path[len - 1] = 0;
	if (!delinkify(path, len))
	{
		kdebugf2();
		return NULL;
	}
	if (stat(path, &buf) != -1)
	{
		if (path[l - 1] == '/')
			path[l] = 0;
		else
			path[l + 1] = 0;
		kdebugf2();
		return path;
	}
	else
	{
		path[0] = 0;
		kdebugf2();
		return NULL;
	}
}
#endif

#ifdef HAVE_EXECINFO
#include <execinfo.h>
#endif
void printBacktrace(const QString &header)
{
	if (header.isEmpty())
		fprintf(stderr, "\nbacktrace:\n");
	else
		fprintf(stderr, "\nbacktrace: ('%s')\n", qPrintable(header));
#ifdef HAVE_EXECINFO
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

QString ggPath(const QString &subpath)
{
	static QString path(QString::null);
	if (path == QString::null)
	{
		QString home;
#ifdef Q_OS_WIN
		// on win32 dataPath dont need real argv[0] so it's safe to use this
		// in such ugly way
		if(QFile::exists(dataPath("usbinst", ""))){
			path=dataPath("config/");
			Parser::globalVariables["KADU_CONFIG"] = path;
			return (path+subpath);
		}

		WCHAR *homepath=new WCHAR[MAX_PATH+1];
		if(!SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL,  0,  homepath))){
			delete homepath;
			homepath=_wgetenv(L"HOMEPATH");
		}
		home=QString::fromUtf16((const ushort*)homepath);

#else
		struct passwd *pw;
		if ((pw = getpwuid(getuid())))
			home = QString::fromLocal8Bit(pw->pw_dir);
		else
			home = QString::fromLocal8Bit(getenv("HOME"));
#endif
		Parser::globalVariables["HOME"] = home;
		QString config_dir = QString::fromLocal8Bit(getenv("CONFIG_DIR"));
#ifdef Q_OS_MACX
		if (config_dir.isNull())
			path = QString("%1/Library/Kadu/").arg(home);
		else
			path = QString("%1/%2/Kadu/").arg(home).arg(config_dir);
#elif defined(Q_OS_WIN)
		if (config_dir.isNull())
			path = QString("%1\\Kadu\\").arg(home);
		else
			path = QString("%1\\%2\\Kadu\\").arg(home).arg(config_dir);
#else
		if (config_dir.isNull())
			path = QString("%1/.kadu/").arg(home);
		else
			path = QString("%1/%2/kadu/").arg(home).arg(config_dir);
#endif
		Parser::globalVariables["KADU_CONFIG"] = path;
	}

	return (path + subpath);
}

static QString lib_path;
static QString data_path;

QString libPath(const QString &f)
{
#ifdef Q_OS_WIN
	QString fp=f;
	if(fp.startsWith("kadu")) fp.remove(0, 4);
	return lib_path + fp;
#else
	return lib_path + f;
#endif
}

QString dataPath(const QString &p, const char *argv0)
{
	QString path=p;

	if (argv0 != 0)
	{
#ifdef Q_OS_MACX
		char cpath[1024];
		if (findMe(argv0, cpath, 1024)==NULL)
		{
			fprintf(stderr, "we've got real problem here ;)\n");
			fflush(stderr);
			exit(10);
		}
		else
		{
			data_path = QString(cpath) + "../../";
			lib_path = QString(cpath) + "../../";
		}
#elif defined(Q_OS_WIN)
		WCHAR epath[MAX_PATH+1];
		GetModuleFileNameW(NULL, epath, MAX_PATH);

		data_path=QString::fromUtf16((const ushort*)epath);
		data_path.resize(data_path.lastIndexOf('\\')+1);
		lib_path=data_path;
#else
		QString datadir(DATADIR);
		QString bindir(BINDIR);
		QString libdir(LIBDIR);

		//je�eli �cie�ki nie ko�cz� si� na /share i /bin oraz gdy bez tych ko�c�wek
		//�cie�ki si� nie pokrywaj�, to znaczy �e kto� ustawi� r�cznie DATADIR lub BINDIR
		if (!datadir.endsWith("/share") || !bindir.endsWith("/bin") || !libdir.endsWith("/lib") ||
			datadir.left(datadir.length() - 6) != bindir.left(bindir.length() - 4) ||
			bindir.left(bindir.length() - 4) != libdir.left(libdir.length() - 4))
		{
			data_path = datadir + '/';
			lib_path = libdir + '/';
		}
		else
		{
			char cpath[1024];
			if (findMe(argv0, cpath, 1024) == NULL)
			{
				data_path = datadir + '/';
				lib_path = libdir + '/';
			}
			else
			{
				data_path = QString(cpath) + "../share/";
				lib_path = QString(cpath) + "../lib/";
			}
		}
#endif
		Parser::globalVariables["DATA_PATH"] = data_path;
		Parser::globalVariables["LIB_PATH"] = lib_path;
	}
	if (data_path.isEmpty())
	{
		kdebugm(KDEBUG_PANIC, "dataPath() called _BEFORE_ initial dataPath(\"\",argv[0]) (static object uses dataPath()?) !!!\n");
		printBacktrace("dataPath(): constructor of static object uses dataPath");
	}

#ifdef Q_OS_WIN
	// on windows remove kadu from path
	if(path.startsWith("kadu")) path.remove(0, 4);
#endif

	kdebugm(KDEBUG_INFO, "%s%s\n", qPrintable(data_path), qPrintable(path));

	return data_path + path;
}
