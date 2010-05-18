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
#ifdef Q_WS_X11
#include <QtGui/QX11Info>
#include <X11/Xatom.h>
#include <X11/extensions/Xfixes.h>
#undef Bool
#undef Status
#endif
#include "core/core.h"
#include "configuration/configuration-file.h"
#include "configuration/xml-configuration-file.h"
#include "gui/windows/message-dialog.h"
#include "os/generic/compositing-aware-object.h"
#include "os/qtsingleapplication/qtlocalpeer.h"
#include "protocols/protocols-manager.h"

#include "debug.h"
#include "kadu-config.h"
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
#ifdef Q_WS_X11
		Atom net_wm_state;
		int xfixes_event_base;
#endif
	public:
		KaduApplication(int &argc, char **argv) : QApplication(argc, argv)
		{
#ifdef Q_OS_MAC
			/* Install Reopen Application Event (Dock Clicked) */
			m_appleEventProcessorUPP = AEEventHandlerUPP(appleEventProcessor);
			AEInstallEventHandler(kCoreEventClass, kAEReopenApplication,
				m_appleEventProcessorUPP, (long) this, true);
#endif

#ifdef Q_WS_X11
			xfixes_event_base = -1;
			int dummy;
			if (XFixesQueryExtension(QX11Info::display(), &xfixes_event_base, &dummy))
			{
				net_wm_state = XInternAtom(QX11Info::display(), "_NET_WM_CM_S0", False);
				XFixesSelectSelectionInput(QX11Info::display(), QX11Info::appRootWindow(0) , net_wm_state,
				XFixesSetSelectionOwnerNotifyMask |
				XFixesSelectionWindowDestroyNotifyMask |
				XFixesSelectionClientCloseNotifyMask);
			}
			if (QX11Info::isCompositingManagerRunning())
				CompositingAwareObject::compositingStateChanged();
#endif
		}

#ifdef Q_WS_X11
		bool x11EventFilter(XEvent *event)
		{
			if (xfixes_event_base != -1 && event->type == xfixes_event_base + XFixesSelectionNotify)
			{
				XFixesSelectionNotifyEvent* ev = reinterpret_cast<XFixesSelectionNotifyEvent* >(event);
				if (ev->selection == net_wm_state)
					CompositingAwareObject::compositingStateChanged();
			}
			return false;
		}
#endif
		void commitData(QSessionManager & manager)
		{
			Q_UNUSED(manager)

			qApp->quit();
		}
#ifdef Q_OS_MAC
	private:
		AEEventHandlerUPP m_appleEventProcessorUPP;
#endif
};

#ifdef Q_WS_MAC
#include "kadu-core/gui/windows/kadu-window.h"
static OSStatus appleEventProcessor(const AppleEvent *ae,
				AppleEvent *event, long handlerRefCon)
{
	Q_UNUSED(event)
	Q_UNUSED(handlerRefCon)

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
			Core::instance()->kaduWindow()->show();
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
#if QT_VERSION != 0x040600 // TODO: remove after next Qt alpha
			abort();
#endif
	}
}

#ifdef DEBUG_ENABLED
extern KADUAPI bool showTimesInDebug;
#endif
extern KADUAPI char* SystemUserName;

void enableSignalHandling();

void printVersion()
{
	printf("Kadu %s Copyright (c) 2001-2010 Kadu Team\n"
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
		"                             (overwrites CONFIG_DIR variable)\n");
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
	QStringList ids;
	QRegExp idRegExp("^[a-zA-Z]*:(/){0,3}.*");

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
		else if (idRegExp.exactMatch(param))
			ids.append(param);
                else
                {
                        fprintf(stderr, "Ignoring unknown parameter '%s'\n", qApp->argv()[i]);
                }

	}
#ifndef Q_OS_WIN // Qt version is better on win32
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
		qApp->deleteLater();

		return 1;
	}

	qApp->setApplicationName("Kadu");

	Core::instance()->createGui();
	QObject::connect(peer, SIGNAL(messageReceived(const QString &)), Core::instance(), SLOT(receivedSignal(const QString &)));

	QString path_ = profilePath(QString::null);
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
		MessageDialog::msg(qApp->translate("@default", QT_TR_NOOP("Please do not run Kadu as a root!\nIt's a high security risk!")), false, "32x32/dialog-warning.png");
#endif

	if (ids.count())
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
	measureTime = getenv("MEASURE_TIME") != 0;

	if (measureTime)
	{
		getTime(&sec, &msec);
		beforeExecTime = (sec % 1000) * 1000 + msec;
	}

	int ret = qApp->exec();
	kdebugm(KDEBUG_INFO, "after exec\n");

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

	kdebugm(KDEBUG_INFO, "exiting main\n");
	return ret;
}
