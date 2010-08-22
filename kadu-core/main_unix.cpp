/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtCore/QFile>
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>

#include <errno.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include "configuration/configuration-file.h"
#include "configuration/xml-configuration-file.h"
#include "core/core.h"
#include "core/crash-aware-object.h"
#include "gui/windows/kadu-window.h"

#include "debug.h"
#include "kadu-config.h"
#include "misc/misc.h"
#include "modules.h"

#ifdef SIG_HANDLING_ENABLED
	#include <QtCore/QDateTime>
	#include <signal.h>
#ifdef HAVE_EXECINFO
	#include <execinfo.h>
#endif

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

		CrashAwareObject::notifyCrash();

		QString f = QString("kadu.conf.xml.backup.%1").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd.hh.mm.ss"));
		QString debug_file = QString("kadu.backtrace.%1").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd.hh.mm.ss"));

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

		dbgfile = fopen(qPrintable(profilePath(debug_file)), "w");
		if (dbgfile)
		{
			fprintf(dbgfile, "======= BEGIN OF BACKTRACE =====\n");
			for (int i = 0; i < num_entries; ++i)
				fprintf(dbgfile, "[%d] %s\n", i, bt_strings[i]);
			fprintf(dbgfile, "======= END OF BACKTRACE  ======\n");
			fflush(dbgfile);

			fprintf(dbgfile, "static modules:\n");
			QStringList modules = ModulesManager::instance()->staticModules();

			foreach(const QString &module, modules)
				fprintf(dbgfile, "> %s\n", qPrintable(module));
			fflush(dbgfile);

			fprintf(dbgfile, "loaded modules:\n");
			modules = ModulesManager::instance()->loadedModules();
			foreach(const QString &module, modules)
				fprintf(dbgfile, "> %s\n", qPrintable(module));
			fflush(dbgfile);
			fprintf(dbgfile, "Qt compile time version: %s\nQt runtime version: %s\n", QT_VERSION_STR, qVersion());
			fprintf(dbgfile, "Kadu version: %s\n", VERSION);
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
		xml_config_file->saveTo(profilePath(f.toLatin1()));
		abort();
	}
	else if (s == SIGUSR1)
	{
		kdebugm(KDEBUG_INFO, "ok, got a signal to show up\n");
		Core::instance()->kaduWindow()->show();
	}
	else if (s == SIGINT || s == SIGTERM)
		Core::instance()->quit();
}

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
	}
#endif
}
