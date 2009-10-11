/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QApplication>
#include <QtGui/QMessageBox>

#include <errno.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include "config_file.h"
#include "debug.h"
#include "misc.h"
#include "modules.h"
#include "kadu.h"

int lockFileHandle;
QFile *lockFile;
struct flock *lock_str;

#ifdef SIG_HANDLING_ENABLED
	#include <QtCore/QDateTime>
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

		dbgfile = fopen(qPrintable(ggPath(debug_file)), "w");
		if (dbgfile)
		{
			fprintf(dbgfile, "======= BEGIN OF BACKTRACE =====\n");
			for (int i = 0; i < num_entries; ++i)
				fprintf(dbgfile, "[%d] %s\n", i, bt_strings[i]);
			fprintf(dbgfile, "======= END OF BACKTRACE  ======\n");
			fflush(dbgfile);

			fprintf(dbgfile, "static modules:\n");
			QStringList modules = modules_manager->staticModules();

			foreach(const QString &module, modules)
				fprintf(dbgfile, "> %s\n", qPrintable(module));
			fflush(dbgfile);

			fprintf(dbgfile, "loaded modules:\n");
			modules = modules_manager->loadedModules();
			foreach(const QString &module, modules)
				fprintf(dbgfile, "> %s\n", qPrintable(module));
			fflush(dbgfile);
			fprintf(dbgfile, "Qt compile time version: %d.%d.%d\nQt runtime version: %s\n", (QT_VERSION&0xff0000)>>16, (QT_VERSION&0xff00)>>8, QT_VERSION&0xff, qVersion());
			fprintf(dbgfile, "Kadu version: %s %s\n", VERSION, DETAILED_VERSION);
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
		qApp->postEvent(kadu, new QEvent((QEvent::Type)4321));
	}
	else if (s == SIGINT || s == SIGTERM)
		//qApp->postEvent(qApp, new QEvent(QEvent::Quit), Qt::HighEventPriority);
		qApp->quit();
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

bool sh_enabled=true;

void enableSignalHandling()
{
#ifdef SIG_HANDLING_ENABLED
	char *d=getenv("SIGNAL_HANDLING");
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
}

bool isRuning(int ggnumber)
{
	lockFile = new QFile(ggPath("lock"));

	lock_str = (struct flock *) malloc(sizeof(struct flock));
	lock_str->l_type = F_WRLCK;
	lock_str->l_whence = SEEK_SET;
	lock_str->l_start = 0;
	lock_str->l_len = 0;

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
#if defined (__FreeBSD__) || defined (__sun__)
						typedef union sigval sigval_t;
#endif
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
				lockFile->close();
				delete lockFile;
				return true;
			}
		}
	}
	return false;
}

void disableLock()
{
	lock_str->l_type = F_UNLCK;
	fcntl(lockFileHandle, F_SETLK, lock_str);
//	flock(lockFileHandle, LOCK_UN);
	lockFile->close();
	delete lockFile;
	lockFile=NULL;
}

