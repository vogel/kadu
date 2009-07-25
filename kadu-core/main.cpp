/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QDir>
#include <QtCore/QLocale>
#include <QtCore/QTimer>
#include <QtCore/QTranslator>
#include <QtGui/QApplication>

#include <time.h>
#include <errno.h>
#ifndef Q_WS_WIN
#include <sys/file.h>
#include <pwd.h>
#else
#include <winsock2.h>
#endif
#ifdef Q_OS_BSD4
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include "core/core.h"
#include "configuration/configuration-file.h"
#include "configuration/xml-configuration-file.h"
#include "gui/windows/message-box.h"
#include "protocols/protocols-manager.h"

#include "debug.h"
#include "emoticons.h"
#include "icons-manager.h"
#include "misc/misc.h"
#include "modules.h"

#ifdef Q_OS_MAC
#include <Carbon/Carbon.h>

static OSStatus appleEventProcessor(const AppleEvent *ae,
				AppleEvent *event, long handlerRefCon);
#endif

class KaduApplication: public QApplication
{
	public:
		KaduApplication(int &argc, char **argv): QApplication(argc, argv)
		{
#ifdef Q_OS_MAC
			/* Install Reopen Application Event (Dock Clicked) */
			m_appleEventProcessorUPP = AEEventHandlerUPP(appleEventProcessor);
			AEInstallEventHandler(kCoreEventClass, kAEReopenApplication,
				m_appleEventProcessorUPP, (long) this, true);
#endif
		};
		void commitData(QSessionManager & manager)
		{
			qApp->quit();
		}
#ifdef Q_OS_MAC
	private:
		AEEventHandlerUPP m_appleEventProcessorUPP;
#endif
};

#ifdef Q_WS_MAC
static OSStatus appleEventProcessor(const AppleEvent *ae,
				AppleEvent *event, long handlerRefCon)
{
	KaduApplication *app = (KaduApplication *) handlerRefCon;

	OSType aeID = typeWildCard;
	OSType aeClass = typeWildCard;

	AEGetAttributePtr(ae, keyEventClassAttr, typeType, 0,
                       &aeClass, sizeof(aeClass), 0);
	AEGetAttributePtr(ae, keyEventIDAttr, typeType, 0,
                       &aeID, sizeof(aeID), 0);

	if (aeClass == kCoreEventClass)
	{
		if (aeID == kAEReopenApplication)
		{
			kadu->show();
		}
		return noErr;
	}

	return eventNotHandledErr;
}
#endif

void kaduQtMessageHandler(QtMsgType type, const char *msg)
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
		case QtCriticalMsg:
			fprintf(stderr, "\033[31;1mCritical: %s\033[0m\n", msg);
			fflush(stderr);
			printBacktrace("critical error from Qt (above)");
			abort();
	}
}

#ifdef DEBUG_ENABLED
extern KADUAPI bool showTimesInDebug;
#endif
extern KADUAPI char* SystemUserName;

void enableSignalHandling();
void disableLock();
bool isRuning(int);

void printVersion()
{
	printf("Kadu %s Copyright (c) 2001-2009 Kadu Team\n"
		"Compiled with Qt %s\nRunning on Qt %s\n",
		VERSION, QT_VERSION_STR, qVersion());
}

void printUsage()
{
	printf("Usage: kadu [Qt Options] [General Options] [Options]\n\n"
		"Kadu Instant Messenger\n");
}

void printKaduOptions()
{
	printf("\nGeneral Options:\n"
		"  --help                     Print Kadu options\n"
		"  --help-qt                  Print Qt options\n"
		"  --help-all                 Print all options\n"
		"  --version                  Print Kadu and Qt version\n"
		"\nOptions:\n"
		"  --debug <mask>             Set debugging mask\n"
		"  --config-dir <path>        Set configuration directory\n"
		"                             (overwrites CONFIG_DIR variable)\n"
		"  gg://<number>              Open chat with GG number\n");
}

void printQtOptions()
{
	printf("\nQt Options:\n"
#ifdef Q_WS_X11
		"  -display <displayname>     Use the X-server display 'displayname'\n"
#elif defined(Q_WS_QWS)
		"  -display <displayname>     Use the QWS display 'displayname'\n"
#else
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
	int ggnumber(0);
	time_t sec;
	int msec;
	int i;
	char *d;
	QString param;
	time_t startTimeT = time(0);
	beforeExecTime = endingTime = exitingTime = 0;
	getTime(&sec, &msec);
	startTime = (sec % 1000) * 1000 + msec;
	// na Windowsie to nie ma znaczenia
#ifndef Q_WS_WIN
	char *env_lang = getenv("LANG");
	if (env_lang)
		setenv("LC_COLLATE", env_lang, true);
	else
		setenv("LC_COLLATE", "pl_PL", true);
#else
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		return 1;
	}

#endif
        debug_mask = -2;

	kdebugm(KDEBUG_INFO, "before creation of new KaduApplication\n");
	new KaduApplication(argc, argv);
	kdebugm(KDEBUG_INFO, "after creation of new KaduApplication\n");

        for (i = 1; i < qApp->argc(); ++i)
	{
                param = qApp->argv()[i];
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
#ifndef Q_OS_WIN
                else if ((param == "--config-dir") && (argc > i + 1))
                        setenv("CONFIG_DIR", argv[++i], 1);
#endif
                else if (param.contains("gg:"))
                {
                        ggnumber = QString(argv[1]).remove("gg:").remove("/").toInt();
                        if (ggnumber < 0)
                                ggnumber = 0;
                }
                else
                {
                        fprintf(stderr, "Ignoring unknown parameter '%s'\n", qApp->argv()[i]);
                }

	}
        qInstallMsgHandler(kaduQtMessageHandler);
        xml_config_file = new XmlConfigFile();

        config_file_ptr = new ConfigFile(ggPath(QString("kadu.conf")));
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
		struct tm *t = localtime(&startTimeT);
#ifndef Q_OS_WIN
		struct passwd *p = getpwuid(getuid());
		if (t && p)
		{
			SystemUserName = strdup(p->pw_name);
			sprintf(path, "/tmp/kadu-%s-%04d-%02d-%02d-%02d-%02d-%02d.dbg", SystemUserName, 1900 + t->tm_year, 1 + t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
			if (freopen(path, "w+", stderr) == 0)
				fprintf(stdout, "freopen: %s\n", strerror(errno));
			else if (fchmod(fileno(stderr), 0600) != 0)
			{
				fclose(stderr);
				fprintf(stdout, "can't chmod output logfile (%s)!\n", path);
			}
		}
#else
		char *tmp=getenv("TEMP");
		if(!tmp) tmp=".";
		sprintf(path, "%s\\kadu-dbg-%04d-%02d-%02d-%02d-%02d-%02d.txt", tmp, 1900 + t->tm_year, 1 + t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
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

	// delayed running, useful in gnome
#ifndef Q_OS_WIN
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

		exit(10);
	}

	// loading translation
	// Translator_qt
	QTranslator qt_qm(0);
	QString lang = config_file.readEntry("General", "Language", QLocale::system().name().mid(0,2));
	qt_qm.load(dataPath(QString("kadu/translations/qt_") + lang), ".");
	qApp->installTranslator(&qt_qm);
	// Translator_kadu
	QTranslator kadu_qm(0);
	kadu_qm.load(dataPath(QString("kadu/translations/kadu_") + lang), ".");
	qApp->installTranslator(&kadu_qm);

	// plugins path (win32)
	qApp->addLibraryPath(libPath("qt"));

	if (isRuning(ggnumber))
	{
		// setClosing? TODO: 0.6.6
		delete config_file_ptr;
		delete xml_config_file;
//		delete qApp;
		return 1;
	}

	qApp->setApplicationName("Kadu");

	Core::instance()->createGui();

	QString path_ = ggPath(QString::null);
#ifndef Q_OS_WIN
	if (path_.endsWith("/kadu/") || path_.endsWith("/Kadu/")) // for profiles directory
		mkdir(qPrintable(path_.left(path_.length() - 6)), 0700);
	mkdir(qPrintable(path_), 0700);
#else
	QDir().mkdir(path_);
#endif

	ModulesManager::instance()->loadAllModules();

	// if someone is running Kadu from root account, let's remind him
	// that it's a "bad thing"(tm) ;) (usually for win32 users)
	// and disable this feature for win32 ;)
#ifndef Q_OS_WIN
	if (geteuid() == 0)
		MessageBox::msg(qApp->translate("@default", QT_TR_NOOP("Please do not run Kadu as a root!\nIt's a high security risk!")), false, "Warning");
#endif

// 	if (ggnumber)
// 		qApp->postEvent(kadu, new OpenGGChatEvent(ggnumber));

	/* for testing of startup / close time */
	char *close_after = getenv("CLOSE_AFTER");
	if (close_after)
	{
		int tm = atoi(close_after);
		if (tm >= 0)
			QTimer::singleShot(tm, Core::instance(), SLOT(quit()));
	}

	/* for testing of startup / close time */
	measureTime = getenv("MEASURE_TIME") != 0;

	if (measureTime)
	{
		getTime(&sec, &msec);
		beforeExecTime = (sec % 1000) * 1000 + msec;
	}

	int ret = qApp->exec();
	kdebugm(KDEBUG_INFO, "after exec\n");

	// clear lockfile
	disableLock();

	qApp->removeTranslator(&qt_qm);
	qApp->removeTranslator(&kadu_qm);
#ifdef Q_WS_WIN
	WSACleanup();
#endif

	if (measureTime)
	{
		getTime(&sec, &msec);
		exitingTime = (sec % 1000) * 1000 + msec;
		fprintf(stderr, "init time: %ld, run time: %ld, ending time: %ld\n", beforeExecTime - startTime, endingTime - beforeExecTime, exitingTime - endingTime);
	}

//	delete qApp; //sometimes leads to segfault
	qApp->deleteLater();
//	qApp = 0;
	kdebugm(KDEBUG_INFO, "exiting main\n");
	return ret;
}
