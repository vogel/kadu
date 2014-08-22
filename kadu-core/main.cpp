/*
 * %kadu copyright begin%
 * Copyright 2010 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010, 2010, 2010, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011, 2014 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004, 2007, 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2002, 2003, 2004, 2005, 2007 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2004, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2002, 2003 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008, 2009 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
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
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QTimer>
#include <QtCore/QTranslator>
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>

#include <errno.h>
#include <time.h>
#ifndef Q_OS_WIN32
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#else // !Q_OS_WIN32
#include <winsock2.h>
#endif // !Q_OS_WIN32

#include "configuration/configuration-file.h"
#include "configuration/xml-configuration-file.h"
#include "core/core.h"
#include "gui/windows/message-dialog.h"
#include "os/qtsingleapplication/qtlocalpeer.h"
#include "protocols/protocols-manager.h"

#include "icons/icons-manager.h"
#include "misc/date-time.h"
#include "misc/kadu-paths.h"
#include "debug.h"
#include "kadu-application.h"
#include "kadu-config.h"

#ifndef Q_OS_WIN32
#if HAVE_EXECINFO
#include <execinfo.h>
#endif

static void printBacktrace(const QString &header)
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
		case QtCriticalMsg:
			fprintf(stderr, "\033[31;1mCritical: %s\033[0m\n", msg);
			fflush(stderr);
			printBacktrace("critical error from Qt (above)");
			break;
		case QtFatalMsg:
			fprintf(stderr, "\033[31;1mFatal: %s\033[0m\n", msg);
			fflush(stderr);
			printBacktrace("fatal error from Qt (above)");
			abort();
			break;
	}
}
#endif // !Q_OS_WIN32

#ifdef DEBUG_OUTPUT_ENABLED
extern KADUAPI bool showTimesInDebug;
#endif

static long int startTime, beforeExecTime, endingTime, exitingTime;
static bool measureTime;

// defined in main_unix.cpp and main_win32.cpp
void enableSignalHandling();

static void printVersion()
{
	printf("Kadu %s Copyright (c) 2001-2014 Kadu Team\n"
		"Compiled with Qt %s\nRunning on Qt %s\n",
		qPrintable(Core::version()), QT_VERSION_STR, qVersion());
}

static void printUsage()
{
	printf("Usage: kadu [General Options] [Options]\n\n"
		"Kadu Instant Messenger\n");
}

static void printKaduOptions()
{
	printf("\nGeneral Options:\n"
		"  --help                     Print Kadu options\n"
		"  --version                  Print Kadu and Qt version\n"
		"\nOptions:\n"
		"  --debug <mask>             Set debugging mask\n"
		"  --config-dir <path>        Set configuration directory\n"
		"                             (overwrites CONFIG_DIR variable)\n");
}

int main(int argc, char *argv[])
{
	bool ok;
	long msec;
	time_t sec;
	FILE *logFile = 0;
	QStringList ids;

	getTime(&sec, &msec);

	beforeExecTime = 0;
	endingTime = 0;
	exitingTime = 0;
	startTime = (sec % 1000) * 1000 + msec;

#ifndef Q_OS_WIN32
	// We want some sensible LC_COLLATE (i.e., not "C", if possible) to make
	// QString::localeAwareCompare() work as expected.
	QByteArray langEnv = qgetenv("LANG");
	QByteArray lcAllEnv = qgetenv("LC_ALL");
	if (langEnv.isEmpty() && lcAllEnv.isEmpty())
		qputenv("LC_COLLATE", "en_US");
	else if (lcAllEnv.isEmpty())
		qputenv("LC_COLLATE", langEnv);
#else // !Q_OS_WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 2;
#endif // !Q_OS_WIN32

	kdebugm(KDEBUG_INFO, "before creation of new KaduApplication\n");
	new KaduApplication(argc, argv);
	kdebugm(KDEBUG_INFO, "after creation of new KaduApplication\n");

	auto arguments = QCoreApplication::arguments();
	for (auto it = arguments.constBegin(); it != arguments.constEnd(); ++it)
	{
		// do not parse program name
		if (it == arguments.constBegin())
			continue;

		if (*it == QLatin1String("--version"))
		{
			printVersion();
			delete qApp;
#ifdef Q_OS_WIN32
			WSACleanup();
#endif
			return 0;
		}
		else if (*it == QLatin1String("--help"))
		{
			printUsage();
			printKaduOptions();
			delete qApp;
#ifdef Q_OS_WIN32
			WSACleanup();
#endif
			return 0;
		}
		else if (*it == QLatin1String("--debug") && ++it != arguments.constEnd())
		{
			it->toInt(&ok);
			if (ok)
				qputenv("DEBUG_MASK", it->toUtf8());
			else
				fprintf(stderr, "Ignoring invalid debug mask '%s'\n", it->toUtf8().constData());
		}
		else if (*it == QLatin1String("--config-dir") && ++it != arguments.constEnd())
			qputenv("CONFIG_DIR", it->toUtf8());
		else if (QRegExp("^[a-zA-Z]*:(/){0,3}.*").exactMatch(*it))
			ids.append(*it);
		else
			fprintf(stderr, "Ignoring unknown parameter '%s'\n", it->toUtf8().constData());
	}

	// It has to be called after putting CONFIG_DIR environment variable.
	KaduPaths::createInstance();

	if (0 != qgetenv("SAVE_STDERR").toInt())
	{
		const QByteArray logFilePath = QString(KaduPaths::instance()->profilePath() + QLatin1String("kadu.log.") + QDateTime::currentDateTime().toString("yyyy.MM.dd.hh.mm.ss")).toLocal8Bit();
		logFile = freopen(logFilePath.constData(), "w", stderr);
		if (!logFile)
			printf("freopen failed: %s\nstderr is now broken\n", strerror(errno));
		else
			printf("logging all stderr output to file: %s\n", logFilePath.constData());
	}

#ifndef Q_OS_WIN32
	// Qt version is better on win32
	qInstallMsgHandler(kaduQtMessageHandler);
#endif

	xml_config_file = new XmlConfigFile();
	if (!xml_config_file->isUsable())
	{
		QString errorMessage = QCoreApplication::translate("@default", "We're sorry, but Kadu cannot be loaded. "
				"Profile is inaccessible. Please check permissions in the '%1' directory.")
				.arg(KaduPaths::instance()->profilePath().left(KaduPaths::instance()->profilePath().length() - 1));
		QMessageBox::critical(0, QCoreApplication::translate("@default", "Profile Inaccessible"), errorMessage, QMessageBox::Abort);
		qFatal("%s", qPrintable(errorMessage));
	}
	config_file_ptr = new ConfigFile(KaduPaths::instance()->profilePath() + QLatin1String("kadu.conf"));

#ifdef DEBUG_OUTPUT_ENABLED
	showTimesInDebug = (0 != qgetenv("SHOW_TIMES").toInt());
#endif

	enableSignalHandling();

	const QString lang = config_file.readEntry("General", "Language", QLocale::system().name().left(2));
	QTranslator qt_qm, kadu_qm;
	qt_qm.load("qt_" + lang, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	kadu_qm.load("kadu_" + lang, KaduPaths::instance()->dataPath() + QLatin1String("translations"));
	QCoreApplication::installTranslator(&qt_qm);
	QCoreApplication::installTranslator(&kadu_qm);

	QtLocalPeer *peer = new QtLocalPeer(qApp, KaduPaths::instance()->profilePath());
	if (peer->isClient())
	{
		if (!ids.isEmpty())
			foreach (const QString &id, ids)
				peer->sendMessage(id, 1000);
		else
			peer->sendMessage("activate", 1000);

		delete config_file_ptr;
		delete xml_config_file;
		delete qApp;
#ifdef Q_OS_WIN32
		WSACleanup();
#endif

		return 1;
	}

	Core::instance()->createGui();
	Core::instance()->runGuiServices();
	QObject::connect(peer, SIGNAL(messageReceived(const QString &)),
			Core::instance(), SLOT(receivedSignal(const QString &)));

	Core::instance()->activatePlugins();

	foreach (const QString &id, ids)
		Core::instance()->receivedSignal(id);

	/* for testing of startup / close time */
	int closeAfter = qgetenv("CLOSE_AFTER").toInt(&ok);
	if (ok && closeAfter >= 0)
		QTimer::singleShot(closeAfter, qApp, SLOT(quit()));
	if (0 != qgetenv("MEASURE_TIME").toInt())
	{
		getTime(&sec, &msec);
		beforeExecTime = (sec % 1000) * 1000 + msec;
	}

	// it has to be called after loading modules (docking might want to block showing the window)
	Core::instance()->showMainWindow();
	Core::instance()->initialized();

	int ret = qApp->exec();
	kdebugm(KDEBUG_INFO, "after exec\n");

	delete xml_config_file;
	delete config_file_ptr;

	xml_config_file = 0;
	config_file_ptr = 0;

	// On some systems it leads to crash with sms module.
	// Reproducible by simply calling "delete new QScriptEngine();" in a module,
	// so it's probably a bug in Qt. Sigh.
	//delete qApp;

#ifdef Q_OS_WIN32
	WSACleanup();
#endif

	if (measureTime)
	{
		getTime(&sec, &msec);
		exitingTime = (sec % 1000) * 1000 + msec;
		fprintf(stderr, "init time: %ld, run time: %ld, ending time: %ld\n",
				beforeExecTime - startTime, endingTime - beforeExecTime, exitingTime - endingTime);
	}

	kdebugm(KDEBUG_INFO, "exiting main\n");

	if (logFile)
		fclose(logFile);
	KaduPaths::destroyInstance();

	return ret;
}
