/*
 * %kadu copyright begin%
 * Copyright 2007, 2008, 2009 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003, 2004, 2005, 2007 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2007, 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2004, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2002, 2003 Dariusz Jagodzik (mast3r@kadu.net)
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
#include <QtCore/QLibraryInfo>
#include <QtCore/QLocale>
#include <QtCore/QTimer>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QTranslator>
#include <QtGui/QApplication>

#include <time.h>
#include <errno.h>
#ifndef Q_WS_WIN
#include <sys/file.h>
#include <sys/stat.h>
#include <pwd.h>
#else // !Q_WS_WIN
#include <winsock2.h>
#endif // !Q_WS_WIN
#if defined(Q_OS_BSD4) || defined(Q_OS_LINUX)
#include <sys/types.h>
#endif // Q_OS_BSD4 || Q_OS_LINUX

#include "core/core.h"
#include "configuration/configuration-file.h"
#include "configuration/xml-configuration-file.h"
#include "gui/windows/message-dialog.h"
#include "os/qtsingleapplication/qtlocalpeer.h"
#include "protocols/protocols-manager.h"

#include "debug.h"
#include "kadu-config.h"
#include "icons-manager.h"
#include "kadu-application.h"
#include "misc/misc.h"
#include "modules.h"

#ifndef Q_WS_WIN
static void kaduQtMessageHandler(QtMsgType type, const char *msg)
{
	switch (type)
	{
		case QtDebugMsg:
			fprintf(stderr, "Debug: %s\n", msg);
			fflush(stderr);
			break;
		case QtWarningMsg:
			fprintf(stderr, "\033[34mWarning: %s\033[0m\n", msg);
			fflush(stderr);
			if (strstr(msg, "no mimesource for") == 0)
				printBacktrace("warning from Qt (above)");
			break;
		case QtFatalMsg:
			fprintf(stderr, "\033[31;1mFatal: %s\033[0m\n", msg);
			fflush(stderr);
			printBacktrace("fatal error from Qt (above)");
			abort();
			break;
		case QtCriticalMsg:
			fprintf(stderr, "\033[31;1mCritical: %s\033[0m\n", msg);
			fflush(stderr);
			printBacktrace("critical error from Qt (above)");
#if QT_VERSION != 0x040600 // TODO: remove after next Qt alpha
			abort();
#endif
			break;
		default:
			break;
	}
}
#endif // Q_WS_WIN

#ifdef DEBUG_ENABLED
extern KADUAPI bool showTimesInDebug;
#endif
extern KADUAPI char *SystemUserName;

// defined in main_unix.cpp and main_win32.cpp
void enableSignalHandling();

static void printVersion()
{
	printf("Kadu %s Copyright (c) 2001-2010 Kadu Team\n"
		"Compiled with Qt %s\nRunning on Qt %s\n",
		qPrintable(Core::version()), QT_VERSION_STR, qVersion());
}

static void printUsage()
{
	printf("Usage: kadu [Qt Options] [General Options] [Options]\n\n"
		"Kadu Instant Messenger\n");
}

static void printKaduOptions()
{
	printf("\nGeneral Options:\n"
		"  --help                     Print Kadu options\n"
		"  --help-qt                  Print Qt options\n"
		"  --help-all                 Print all options\n"
		"  --version                  Print Kadu and Qt version\n"
		"\nOptions:\n"
		"  --debug <mask>             Set debugging mask\n"
		"  --config-dir <path>        Set configuration directory\n"
		"                             (overwrites CONFIG_DIR variable)\n");
}

static void printQtOptions()
{
	printf("\nQt Options:\n"
#if defined(Q_WS_X11)
		"  -display <displayname>     Use the X-server display 'displayname'\n"
#elif defined(Q_WS_QWS)
		"  -display <displayname>     Use the QWS display 'displayname'\n"
#endif
		"  -session <sessionId>       Restore the application for the given 'sessionId'\n"
		"  -cmap                      Causes the application to install a private color\n"
		"                             map on an 8-bit display\n"
		"  -ncols <count>             Limits the number of colors allocated in the color\n"
		"                             cube on an 8-bit display, if the application is\n"
		"                             using the QApplication::ManyColor color \n"
		"                             specification\n"
		"  -nograb                    Tells Qt to never grab the mouse or the keyboard\n"
		"  -dograb                    Running under a debugger can cause an implicit\n"
		"                             -nograb, use -dograb to override\n"
		"  -sync                      Switches to synchronous mode for debugging\n"
		"  -fn, -font <fontname>      defines the application font\n"
		"  -bg, -background <color>   Sets the default background color and an\n"
		"                             application palette (light and dark shades are\n"
		"                             calculated)\n"
		"  -fg, -foreground <color>   Sets the default foreground color\n"
		"  -btn, -button <color>      Sets the default button color\n"
		"  -name <name>               Sets the application name\n"
		"  -title <title>             Sets the application title (caption)\n"
#ifdef Q_WS_X11
		"  -visual TrueColor          Forces the application to use a TrueColor visual on\n"
		"                             an 8-bit display\n"
		"  -inputstyle <inputstyle>   Sets XIM (X Input Method) input style. Possible\n"
		"                             values are onthespot, overthespot, offthespot and\n"
		"                             root\n"
		"  -im <XIM server>           Set XIM server\n"
		"  -noxim                     Disable XIM\n"
#endif
#ifdef Q_WS_QWS
		"  -qws                       Forces the application to run as QWS Server\n"
#endif
		"  -reverse                   Mirrors the whole layout of widgets\n");
}

int main(int argc, char *argv[])
{
	char *d = 0;
	int msec;
	time_t sec;
	time_t startTimeT = time(0);
	QStringList ids;

	getTime(&sec, &msec);

	beforeExecTime = 0;
	endingTime = 0;
	exitingTime = 0;
	startTime = (sec % 1000) * 1000 + msec;

#ifndef Q_WS_WIN
	char *env_lang = getenv("LANG");
	if (env_lang)
		setenv("LC_COLLATE", env_lang, true);
	else
		setenv("LC_COLLATE", "pl_PL", true);
#else // !Q_WS_WIN
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		return 1;
	}
#endif // !Q_WS_WIN
	debug_mask = -2;

	kdebugm(KDEBUG_INFO, "before creation of new KaduApplication\n");
	(void)new KaduApplication(argc, argv);
	kdebugm(KDEBUG_INFO, "after creation of new KaduApplication\n");

	for (int i = 1; i < qApp->argc(); ++i)
	{
		QString param = qApp->argv()[i];
		if (param == "--version")
		{
			printVersion();
			return 0;
		}
		else if (param == "--help")
		{
			printUsage();
			printKaduOptions();
			return 0;
		}
		else if (param == "--help-qt")
		{
			printUsage();
			printQtOptions();
			return 0;
		}
		else if (param == "--help-all")
		{
			printUsage();
			printKaduOptions();
			printQtOptions();
			return 0;
		}
		else if ((param == "--debug") && (argc > i + 1))
			debug_mask = atol(argv[++i]);
#ifndef Q_WS_WIN
		else if ((param == "--config-dir") && (argc > i + 1))
			setenv("CONFIG_DIR", argv[++i], 1);
#endif
		else if (QRegExp("^[a-zA-Z]*:(/){0,3}.*").exactMatch(param))
			ids.append(param);
		else
			fprintf(stderr, "Ignoring unknown parameter '%s'\n", qApp->argv()[i]);
	}

#ifndef Q_WS_WIN
	// Qt version is better on win32
	qInstallMsgHandler(kaduQtMessageHandler);
#endif

	xml_config_file = new XmlConfigFile();
	config_file_ptr = new ConfigFile(profilePath(QString("kadu.conf")));

	if (debug_mask == -2)
	{
		debug_mask = config_file.readNumEntry("General", "DEBUG_MASK", -1);
		d = getenv("DEBUG_MASK");
		if (d)
			debug_mask = atol(d);
	}

	bool saveStdErr = config_file.readBoolEntry("General", "SaveStdErr");
	d = getenv("SAVE_STDERR");
	if (d)
		saveStdErr = strcmp(d, "1") == 0;
	if (saveStdErr)
	{
		char path[1024];
		tm *t = localtime(&startTimeT);
#ifndef Q_WS_WIN
		passwd *p = getpwuid(getuid());
		if (t && p)
		{
			SystemUserName = strdup(p->pw_name);
			sprintf(path, "/tmp/kadu-%s-%04d-%02d-%02d-%02d-%02d-%02d.dbg",
					SystemUserName, 1900 + t->tm_year, 1 + t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
			if (freopen(path, "w+", stderr) == 0)
				fprintf(stdout, "freopen: %s\n", strerror(errno));
			else if (fchmod(fileno(stderr), 0600) != 0)
			{
				fclose(stderr);
				fprintf(stdout, "can't chmod output logfile (%s)!\n", path);
			}
		}
#else
		char *tmp = getenv("TEMP");
		if (!tmp)
			tmp='.';
		sprintf(path, "%s\\kadu-dbg-%04d-%02d-%02d-%02d-%02d-%02d.txt",
				tmp, 1900 + t->tm_year, 1 + t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
		if (freopen(path, "w+", stderr) == 0)
			fprintf(stdout, "freopen: %s\n", strerror(errno));
#endif
	}

#ifdef DEBUG_ENABLED
	d = getenv("SHOW_TIMES");
	if (d)
		showTimesInDebug = atoi(d);
#endif

	enableSignalHandling();

#ifndef Q_WS_WIN
	// delayed running, useful in gnome
	sleep(config_file.readNumEntry("General", "StartDelay"));
#endif
	QString data_dir = dataPath("kadu", argv[0]);
	if (!QDir(data_dir).isReadable())
	{
		fprintf(stderr, "data directory (%s) is NOT readable, exiting...\n", qPrintable(data_dir));
		fprintf(stderr, "look at: http://www.kadu.net/msgs/data_dir_not_readable/\n");
		fflush(stderr);

		delete xml_config_file;
		delete config_file_ptr;
		//delete qApp;

		return 10;
	}

	const QString lang = config_file.readEntry("General", "Language", QLocale::system().name());
	QTranslator qt_qm, kadu_qm;
	qt_qm.load("qt_" + lang, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	kadu_qm.load("kadu_" + lang, dataPath("kadu/translations"));
	qApp->installTranslator(&qt_qm);
	qApp->installTranslator(&kadu_qm);

	// plugins path (win32)
	qApp->addLibraryPath(libPath("qt"));

	QtLocalPeer *peer = new QtLocalPeer(qApp, profilePath());
	if (peer->isClient())
	{
		if (ids.count())
			foreach (const QString &id, ids)
				peer->sendMessage(id, 1000);
		else
			peer->sendMessage("activate", 1000);

		delete config_file_ptr;
		delete xml_config_file;
		//delete qApp;

		return 1;
	}

	Core::instance()->createGui();
	QObject::connect(peer, SIGNAL(messageReceived(const QString &)),
			Core::instance(), SLOT(receivedSignal(const QString &)));

	QString path_ = profilePath();
#ifndef Q_WS_WIN
	if (path_.endsWith(QLatin1String("/kadu/")) || path_.endsWith(QLatin1String("/Kadu/"))) // for profiles directory
		mkdir(qPrintable(path_.left(path_.length() - 6)), 0700);
	mkdir(qPrintable(path_), 0700);
#else
	QDir().mkdir(path_);
#endif

	ModulesManager::instance()->loadAllModules();

#ifndef Q_WS_WIN
	// if someone is running Kadu from root account, let's remind him
	// that it's a "bad thing"(tm) ;) (usually for win32 users)
	if (geteuid() == 0)
		MessageDialog::show("dialog-warning", qApp->translate("@default", "Kadu"), qApp->translate("@default", "Please do not run Kadu as a root!\n"
				"It's a high security risk!"));
#endif

	if (ids.count() >= 0)
		foreach (const QString &id, ids)
			Core::instance()->receivedSignal(id);

	/* for testing of startup / close time */
	char *close_after = getenv("CLOSE_AFTER");
	if (close_after)
	{
		int tm = atoi(close_after);
		if (tm >= 0)
			QTimer::singleShot(tm, Core::instance(), SLOT(quit()));
	}

	/* for testing of startup / close time */
	measureTime = (getenv("MEASURE_TIME") != 0);
	if (measureTime)
	{
		getTime(&sec, &msec);
		beforeExecTime = (sec % 1000) * 1000 + msec;
	}

	int ret = qApp->exec();
	kdebugm(KDEBUG_INFO, "after exec\n");

#ifdef Q_WS_WIN
	WSACleanup();
#endif

	// TODO 0.6.6: it causes segfault on exit with QGtkStyle, at least
	// on Ubuntu 10.10 (I tested it) --beevvy
	// it's a hackish WORKAROUND!
	//delete qApp;

	if (measureTime)
	{
		getTime(&sec, &msec);
		exitingTime = (sec % 1000) * 1000 + msec;
		fprintf(stderr, "init time: %ld, run time: %ld, ending time: %ld\n",
				beforeExecTime - startTime, endingTime - beforeExecTime, exitingTime - endingTime);
	}

	kdebugm(KDEBUG_INFO, "exiting main\n");
	return ret;
}
