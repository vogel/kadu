/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <errno.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QDateTime>

#include "configuration/xml-configuration-file.h"
#include "core/core.h"
#include "core/crash-aware-object.h"
#include "gui/windows/kadu-window.h"
#include "kadu-application.h"
#include "misc/kadu-paths.h"
#include "plugins/plugin.h"
#include "plugins/plugins-manager.h"
#include "debug.h"
#include "kadu-config.h"

#if HAVE_EXECINFO
#include <execinfo.h>
#endif // HAVE_EXECINFO

static void badSignalHandler(int signal)
{
	// We are calling abort(3) in this handler and we want it to always
	// perform its default action (i.e., generate core dump and terminate).
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = SIG_DFL;
	sigaction(SIGABRT, &sa, 0);

	kdebugmf(KDEBUG_WARNING, "caught signal %d\n", signal);
	kdebugm(KDEBUG_PANIC, "Kadu crashed :(\n");

	CrashAwareObject::notifyCrash();

	QString backupFileName = QLatin1String("kadu.conf.xml.backup.") + QDateTime::currentDateTime().toString("yyyy.MM.dd.hh.mm.ss");
	QString backtraceFileName = QLatin1String("kadu.backtrace.") + QDateTime::currentDateTime().toString("yyyy.MM.dd.hh.mm.ss");

#if HAVE_EXECINFO
	void *backtraceArray[100];
	char **backtraceStrings;
	int numEntries;

	if ((numEntries = backtrace(backtraceArray, 100)) < 0)
	{
		kdebugm(KDEBUG_PANIC, "could not generate backtrace\n");
		abort();
	}
	if (!(backtraceStrings = backtrace_symbols(backtraceArray, numEntries)))
	{
		kdebugm(KDEBUG_PANIC, "could not get symbol names for backtrace\n");
		abort();
	}

	fprintf(stderr, "\n======= BEGIN OF BACKTRACE =====\n");
	for (int i = 0; i < numEntries; ++i)
		fprintf(stderr, "[%d] %s\n", i, backtraceStrings[i]);
	fprintf(stderr, "======= END OF BACKTRACE  ======\n");
	fflush(stderr);

	FILE *backtraceFile = fopen(qPrintable(QString(KaduPaths::instance()->profilePath() + backtraceFileName)), "w");
	if (backtraceFile)
	{
		fprintf(backtraceFile, "======= BEGIN OF BACKTRACE =====\n");
		for (int i = 0; i < numEntries; ++i)
			fprintf(backtraceFile, "[%d] %s\n", i, backtraceStrings[i]);
		fprintf(backtraceFile, "======= END OF BACKTRACE  ======\n");

		fprintf(backtraceFile, "loaded plugins:\n");
		QList<Plugin *> plugins = PluginsManager::instance()->activePlugins();
		foreach (Plugin *plugin, plugins)
			fprintf(backtraceFile, "> %s\n", qPrintable(plugin->name()));
		fprintf(backtraceFile, "Kadu version: %s\n", qPrintable(Core::version()));
		fprintf(backtraceFile, "Qt compile time version: %s\nQt runtime version: %s\n",
				QT_VERSION_STR, qVersion());
#ifdef __GNUC__
		fprintf(backtraceFile, "GCC version: %d.%d.%d\n",
				__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#endif // __GNUC__
		fprintf(backtraceFile, "EOF\n");

		fclose(backtraceFile);
	}

	free(backtraceStrings);
#else // HAVE_EXECINFO
	kdebugm(KDEBUG_PANIC, "backtrace not available\n");
#endif // HAVE_EXECINFO

	xml_config_file->saveTo(KaduPaths::instance()->profilePath() + backupFileName);
	abort();
}

static void quitSignalHandler(int signal)
{
	int saveErrno = errno;

	// Block the other signals here so that the ints (signal codes) will not be written interleaved.
	sigset_t newSet, oldSet;
	sigemptyset(&newSet);
	sigaddset(&newSet, SIGHUP);
	sigaddset(&newSet, SIGINT);
	sigaddset(&newSet, SIGTERM);
	sigprocmask(SIG_BLOCK, &newSet, &oldSet);

	int ret;
	do
	{
		ret = write(KaduApplication::QuitFd[1], &signal, sizeof(signal));
	}
	while (-1 == ret && EINTR == errno);

	sigprocmask(SIG_SETMASK, &oldSet, 0);

	errno = saveErrno;
}

void enableSignalHandling()
{
	char *d = getenv("SIGNAL_HANDLING");
	bool signalHandlingEnabled = d ? (atoi(d) != 0) : true;

	if (signalHandlingEnabled)
	{
		struct sigaction sa;
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = 0;

		// As required by libgadu.
		sa.sa_handler = SIG_IGN;
		sigaction(SIGPIPE, &sa, 0);

		sa.sa_handler = quitSignalHandler;
		sigaction(SIGHUP, &sa, 0);
		sigaction(SIGINT, &sa, 0);
		sigaction(SIGTERM, &sa, 0);

		sa.sa_handler = badSignalHandler;
		sigaction(SIGILL, &sa, 0);
		sigaction(SIGABRT, &sa, 0);
		sigaction(SIGFPE, &sa, 0);
		sigaction(SIGSEGV, &sa, 0);
		sigaction(SIGBUS, &sa, 0);
	}
}
