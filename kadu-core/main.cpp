/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <qapplication.h>
#include <qmessagebox.h>
#include <qtextcodec.h>
#include <QByteArray>
#include <QTranslator>
#include <QCustomEvent>
#include <QString>
#include <QPixmap>
#include <QEvent>

#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/time.h>
#include <pwd.h>
#include <signal.h>

#include "kadu.h"
#include "kadu-config.h"

#include "config_file.h"
#include "misc.h"
#include "debug.h"
#include "groups_manager.h"
#include "icons_manager.h"
#include "misc.h"
#include "modules.h"
#include "emoticons.h"
#include "message_box.h"
#include "protocols_manager.h"

//look for comment in config_file.h
ConfigFile *config_file_ptr;

Kadu *kadu;

#ifdef SIG_HANDLING_ENABLED
#include <qdatetime.h>
#include <signal.h>
#ifdef HAVE_EXECINFO
#include <execinfo.h>
#endif
#define OPEN_CHAT_SIGNAL (SIGRTMIN + 7)

static int sigsegvCount = 0;
static void kadu_signal_handler(int s)
{
	kdebugmf(KDEBUG_WARNING, "%d\n", s);
	if (sigsegvCount > 1)
	{
		kdebugmf(KDEBUG_WARNING, "sigsegv recursion: %d\n", sigsegvCount);
		abort();
	}

	if (s == SIGSEGV)
	{
		++sigsegvCount;
		kdebugm(KDEBUG_PANIC, "Kadu crashed :(\n");
		QString f = QString("kadu.conf.xml.backup.%1").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd.hh.mm.ss"));
		QString debug_file = QString("kadu.backtrace.%1").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd.hh.mm.ss"));

		if (lockFile)
		{
			// there might be another segmentation fault in this signal handler (because of total mess in memory)
			lock_str->l_type = F_UNLCK;
			fcntl(lockFileHandle, F_SETLK, lock_str);
//			flock(lockFileHandle, LOCK_UN);
			kdebugm(KDEBUG_WARNING, "lock released\n");
			lockFile->close();
			kdebugm(KDEBUG_WARNING, "lockfile closed\n");
		}
#ifdef HAVE_EXECINFO
		void *bt_array[100];
		char **bt_strings;
		int num_entries;
		FILE *dbgfile;
		if ((num_entries = backtrace(bt_array, 100)) < 0)
		{
			kdebugm(KDEBUG_PANIC, "could not generate backtrace\n");
			abort();
		}
		if ((bt_strings = backtrace_symbols(bt_array, num_entries)) == NULL)
		{
			kdebugm(KDEBUG_PANIC, "could not get symbol names for backtrace\n");
			abort();
		}

		fprintf(stderr, "\n======= BEGIN OF BACKTRACE =====\n");
		for (int i = 0; i < num_entries; ++i)
			fprintf(stderr, "[%d] %s\n", i, bt_strings[i]);
		fprintf(stderr, "======= END OF BACKTRACE  ======\n");
		fflush(stderr);

		QByteArray p(ggPath(debug_file).local8Bit());
		dbgfile = fopen(p.data(), "w");
		if (dbgfile)
		{
			fprintf(dbgfile, "======= BEGIN OF BACKTRACE =====\n");
			for (int i = 0; i < num_entries; ++i)
				fprintf(dbgfile, "[%d] %s\n", i, bt_strings[i]);
			fprintf(dbgfile, "======= END OF BACKTRACE  ======\n");
			fflush(dbgfile);

			fprintf(dbgfile, "static modules:\n");
			QStringList modules = modules_manager->staticModules();

			CONST_FOREACH(module, modules)
				fprintf(dbgfile, "> %s\n", (*module).local8Bit().data());
			fflush(dbgfile);

			fprintf(dbgfile, "loaded modules:\n");
			modules = modules_manager->loadedModules();
			CONST_FOREACH(module, modules)
				fprintf(dbgfile, "> %s\n", (*module).local8Bit().data());
			fflush(dbgfile);
			fprintf(dbgfile, "Qt compile time version: %d.%d.%d\nQt runtime version: %s\n", (QT_VERSION&0xff0000)>>16, (QT_VERSION&0xff00)>>8, QT_VERSION&0xff, qVersion());
			fprintf(dbgfile, "Kadu version: %s %s\n", VERSION, detailed_version);
			#ifdef __DATE__
			fprintf(dbgfile, "Compile time: %s %s\n", __DATE__, __TIME__);
			#endif
			#ifdef __GNUC__
				//in gcc < 3.0 __GNUC_PATCHLEVEL__ is not defined
				#ifdef __GNUC_PATCHLEVEL__
					fprintf(dbgfile, "GCC version: %d.%d.%d\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
				#else
					fprintf(dbgfile, "GCC version: %d.%d\n", __GNUC__, __GNUC_MINOR__);
				#endif
			#endif
			fprintf(dbgfile, "EOF\n");

			fclose(dbgfile);
		}

		free(bt_strings);
#else
		kdebugm(KDEBUG_PANIC, "backtrace not available\n");
#endif
		xml_config_file->saveTo(ggPath(f.latin1()));
		abort();
	}
	else if (s == SIGUSR1)
	{
		kdebugm(KDEBUG_INFO, "ok, got a signal to show up\n");
		qApp->postEvent(kadu, new QCustomEvent(4321));
	}
	else if (s == SIGINT || s == SIGTERM)
		qApp->postEvent(qApp, new QEvent(QEvent::Quit));
}

#if defined (SIGRTMIN)
void kadu_realtime_signal(int sig, siginfo_t *info, void *)
{
	if (sig != OPEN_CHAT_SIGNAL)
		return;
	int ggnum = info->si_value.sival_int;
	if (ggnum > 0)
		qApp->postEvent(kadu, new OpenGGChatEvent(ggnum));
}
#endif

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
extern bool showTimesInDebug;
#endif
char SystemUserName[100];

int main(int argc, char *argv[])
{
	int ggnumber(0);
	struct timeval tv;
	struct timezone tz;
	time_t startTimeT = time(0);
	gettimeofday(&tv, &tz);
	startTime = (tv.tv_sec % 1000) * 1000000 + tv.tv_usec;

	char *env_lang = getenv("LANG");
	if (env_lang)
		setenv("LC_COLLATE", env_lang, true);
	else
		setenv("LC_COLLATE", "pl_PL", true);

//	debug_mask = -1;
	qInstallMsgHandler(kaduQtMessageHandler);
	xml_config_file = new XmlConfigFile();
	if (argc > 1)
	{
		ggnumber = QString(argv[1]).remove("gg:").remove("/").toInt();
		if (ggnumber < 0)
			ggnumber = 0;
	}

	config_file_ptr = new ConfigFile(ggPath(QString("kadu.conf")));
	debug_mask = config_file.readNumEntry("General", "DEBUG_MASK");
	char *d = getenv("DEBUG_MASK");
	if (d)
		debug_mask = atol(d);
	gg_debug_level = debug_mask | ~255;

	bool saveStdErr = config_file.readBoolEntry("General", "SaveStdErr");
	d = getenv("SAVE_STDERR");
	if (d)
		saveStdErr = strcmp(d, "1") == 0;
	if (saveStdErr)
	{
		char path[1024];
		struct tm *t = localtime(&startTimeT);
		struct passwd *p = getpwuid(geteuid());
		if (t && p)
		{
			strncpy(SystemUserName, p->pw_name, 99);
			SystemUserName[99] = 0;
			sprintf(path, "/tmp/kadu-%s-%04d-%02d-%02d-%02d-%02d-%02d-%ld.dbg", SystemUserName, 1900 + t->tm_year, 1 + t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, tv.tv_usec);
			if (freopen(path, "wx", stderr) == 0)
				fprintf(stdout, "freopen: %s\n", strerror(errno));
			else if (fchmod(fileno(stderr), 0600) != 0)
			{
				fclose(stderr);
				fprintf(stdout, "can't chmod output logfile (%s)!\n", path);
			}
		}
	}

#ifdef DEBUG_ENABLED
	d = getenv("SHOW_TIMES");
	if (d)
		showTimesInDebug = atoi(d);
#endif

	lock_str = (struct flock *) malloc(sizeof(struct flock));
	lock_str->l_type = F_WRLCK;
	lock_str->l_whence = SEEK_SET;
	lock_str->l_start = 0;
	lock_str->l_len = 0;

#ifdef SIG_HANDLING_ENABLED
	bool sh_enabled=true;
	d=getenv("SIGNAL_HANDLING");
	if (d)
		sh_enabled=(atoi(d)!=0);
	if (sh_enabled)
	{
		signal(SIGSEGV, kadu_signal_handler);
		signal(SIGINT, kadu_signal_handler);
		signal(SIGTERM, kadu_signal_handler);
		signal(SIGUSR1, kadu_signal_handler);
		signal(SIGPIPE, SIG_IGN);
#if defined (SIGRTMIN)
		struct sigaction action;
		action.sa_sigaction = kadu_realtime_signal;
		sigemptyset(&action.sa_mask);
		action.sa_flags = SA_SIGINFO;
		sigaction(OPEN_CHAT_SIGNAL, &action, 0);
#endif
	}
#endif

	// delayed running, useful in gnome
	sleep(config_file.readNumEntry("General", "StartDelay"));
	QString data_dir = dataPath("kadu", argv[0]);
	if (!QDir(data_dir).isReadable())
	{
		fprintf(stderr, "data directory (%s) is NOT readable, exiting...\n", data_dir.local8Bit().data());
		fprintf(stderr, "look at: http://www.kadu.net/msgs/data_dir_not_readable/\n");
		fflush(stderr);

		delete xml_config_file;
		delete config_file_ptr;

		exit(10);
	}

	kdebugm(KDEBUG_INFO, "before creation of new QApplication\n");
	new QApplication(argc, argv);
	kdebugm(KDEBUG_INFO, "after creation of new QApplication\n");

	defaultFont = new QFont(qApp->font());
	defaultFontInfo = new QFontInfo(*defaultFont);
	// loading translation
	QTranslator qt_qm(0, "Translator_qt");
	QString lang = config_file.readEntry("General", "Language", QString(QTextCodec::locale()).mid(0,2));
	qt_qm.load(dataPath(QString("kadu/translations/qt_") + lang), ".");
	qApp->installTranslator(&qt_qm);
	QTranslator kadu_qm(0, "Translator_kadu");
	kadu_qm.load(dataPath(QString("kadu/translations/kadu_") + lang), ".");
	qApp->installTranslator(&kadu_qm);
	qApp->setStyle(config_file.readEntry("Look", "QtStyle"));

	lockFile = new QFile(ggPath("lock"));
	if (lockFile->open(QIODevice::ReadWrite))
	{
		lockFileHandle = lockFile->handle();

		if (fcntl(lockFileHandle, F_SETLK, lock_str) == -1)
//		if (flock(lockFileHandle, LOCK_EX | LOCK_NB) != 0)
		{
			kdebugm(KDEBUG_WARNING, "fcntl: %s\n", strerror(errno));
#ifdef SIG_HANDLING_ENABLED
			bool gotPID = fcntl(lockFileHandle, F_GETLK, lock_str) != -1;
			if (sh_enabled)
			{
				if (gotPID)
				{
#if defined (SIGRTMIN)
					kdebugm(KDEBUG_INFO, "l_type: %d, l_pid: %d\n", lock_str->l_type, lock_str->l_pid);
					if (ggnumber)
					{
						sigval_t v;
						v.sival_int = ggnumber;
						sigqueue(lock_str->l_pid, OPEN_CHAT_SIGNAL, v);
					}
					else
#endif
						kill(lock_str->l_pid, SIGUSR1);
				}
				else
					kdebugm(KDEBUG_WARNING, "cannot get information about lock: %s\n", strerror(errno));
			}
			else
				gotPID = false;
#else
			bool gotPID(false);
#endif
			if (gotPID || QMessageBox::warning(NULL, "Kadu",
				qApp->translate("@default", QT_TR_NOOP("Another Kadu is running on this profile but I cannot get its process ID.")),
				qApp->translate("@default", QT_TR_NOOP("Force running Kadu (not recommended).")),
				qApp->translate("@default", QT_TR_NOOP("Quit.")), 0, 1, 1) == 1)
			{
				Kadu::setClosing();
				delete defaultFont;
				delete defaultFontInfo;
				delete config_file_ptr;
				delete xml_config_file;
				lockFile->close();
				delete lockFile;
//				delete qApp;
				return 1;
			}
		}
	}

	ProtocolsManager::initModule();
	UserList::initModule();
	IconsManager::initModule();
	GroupsManager::initModule();
	EmoticonsManager::initModule();

	new Kadu(0);

	qApp->setMainWidget(kadu);
	QPixmap pix;
#ifdef Q_OS_MACX
	pix = icons_manager->loadPixmap("BigOffline");
#else
	pix = icons_manager->loadPixmap("Offline");
#endif
	kadu->setMainWindowIcon(pix);

	QString path_ = ggPath(QString::null);
	if (path_.endsWith("/kadu/") || path_.endsWith("/Kadu/")) // for profiles directory
		mkdir(path_.left(path_.length() - 6).local8Bit().data(), 0700);
	mkdir(path_.local8Bit().data(), 0700);

	ModulesManager::initModule();

	kadu->startupProcedure();

	QObject::connect(qApp, SIGNAL(aboutToQuit()), kadu, SLOT(quitApplication()));

	// if someone is running Kadu from root account, let's remind him
	// that it's a "bad thing"(tm) ;) (usually for win32 users)
	if (geteuid() == 0)
		MessageBox::msg(qApp->translate("@default", QT_TR_NOOP("Please do not run Kadu as a root!\nIt's a high security risk!")), false, "Warning");
	QTimer::singleShot(15000, kadu, SLOT(deleteOldConfigFiles()));
	if (ggnumber)
		qApp->postEvent(kadu, new OpenGGChatEvent(ggnumber));

	/* for testing of startup / close time */
	char *close_after = getenv("CLOSE_AFTER");
	if (close_after)
	{
		int tm = atoi(close_after);
		if (tm >= 0)
			QTimer::singleShot(tm, kadu, SLOT(quit()));
	}

	/* for testing of startup / close time */
	measureTime = getenv("MEASURE_TIME") != 0;

	if (measureTime)
	{
		gettimeofday(&tv, &tz);
		beforeExecTime = (tv.tv_sec % 1000) * 1000000 + tv.tv_usec;
	}

	int ret = qApp->exec();
	kdebugm(KDEBUG_INFO, "after exec\n");
	qApp->removeTranslator(&qt_qm);
	qApp->removeTranslator(&kadu_qm);

	if (measureTime)
	{
		gettimeofday(&tv, &tz);
		exitingTime = (tv.tv_sec % 1000) * 1000000 + tv.tv_usec;
		fprintf(stderr, "init time:%lld, run time:%lld, ending time:%lld\n", beforeExecTime - startTime, endingTime - beforeExecTime, exitingTime - endingTime);
	}

//	delete qApp; //sometimes leads to segfault
//	qApp = 0;
	kdebugm(KDEBUG_INFO, "exiting main\n");
	return ret;
}
