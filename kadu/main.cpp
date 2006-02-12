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

#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

#include "kadu.h"
#include "kadu-config.h"

#include "config_file.h"
#include "misc.h"
#include "debug.h"
#include "groups_manager.h"
#include "icons_manager.h"
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
static void kadu_signal_handler(int s)
{
	kdebugmf(KDEBUG_WARNING, "%d\n", s);

	if (s == SIGSEGV)
	{
		kdebugm(KDEBUG_PANIC, "Kadu crashed :(\n");
		QString f = QString("kadu.conf.xml.backup.%1").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd.hh.mm.ss"));
		QString debug_file = QString("kadu.backtrace.%1").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd.hh.mm.ss"));

		if (lockFile)
		{
			// there might be another segmentation fault in this signal handler (because of total mess in memory)
			flock(lockFileHandle, LOCK_UN);
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

		dbgfile = fopen(ggPath(debug_file), "w");
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
		xml_config_file->saveTo(ggPath(f.latin1()));
		abort();
	}
	else if (s == SIGINT || s == SIGTERM)
		qApp->postEvent(qApp, new QEvent(QEvent::Quit));
}
#endif

int main(int argc, char *argv[])
{
	xml_config_file = new XmlConfigFile();
	config_file_ptr = new ConfigFile(ggPath(QString("kadu.conf")));
	config_file.addVariable("General", "DEBUG_MASK", KDEBUG_ALL & ~KDEBUG_FUNCTION_END);
	debug_mask=config_file.readNumEntry("General", "DEBUG_MASK");
	char *d = getenv("DEBUG_MASK");
	if (d)
		debug_mask=atol(d);
	gg_debug_level=debug_mask | ~255;

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
	}
#endif

	// delayed running, useful in gnome
	config_file.addVariable("General", "StartDelay", 0);
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


	new QApplication(argc, argv);
	defaultFontInfo = new QFontInfo(qApp->font());
	defaultFont = new QFont(defaultFontInfo->family(), defaultFontInfo->pointSize());
	// loading translation
	config_file.addVariable("General", "Language", QString(QTextCodec::locale()).mid(0,2));
	QTranslator qt_qm(0, "Translator_qt");
	QString lang = config_file.readEntry("General", "Language");
	qt_qm.load(dataPath(QString("kadu/translations/qt_") + lang), ".");
	qApp->installTranslator(&qt_qm);
	QTranslator kadu_qm(0, "Translator_kadu");
	kadu_qm.load(dataPath(QString("kadu/translations/kadu_") + lang), ".");
	qApp->installTranslator(&kadu_qm);
	qApp->setStyle(config_file.readEntry("Look", "QtStyle"));

	lockFile = new QFile(ggPath("lock"));
	if (lockFile->open(IO_ReadWrite))
	{
		lockFileHandle = lockFile->handle();
		if (flock(lockFileHandle, LOCK_EX | LOCK_NB) != 0)
		{
			kdebugm(KDEBUG_WARNING, "flock: %s\n", strerror(errno));
			if (QMessageBox::warning(NULL, "Kadu",
				qApp->translate("@default", QT_TR_NOOP("Another Kadu is running on this profile.")),
				qApp->translate("@default", QT_TR_NOOP("Force running Kadu (not recommended).")),
				qApp->translate("@default", QT_TR_NOOP("Quit.")), 0, 1, 1) == 1)
			{
				Kadu::setClosing();
				delete defaultFont;
				delete defaultFontInfo;
				delete emoticons;
				delete config_file_ptr;
				delete xml_config_file;
				lockFile->close();
				delete lockFile;
//				delete qApp;
				return 1;
			}
		}
	}

	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "General"), "GeneralTab");
	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "ShortCuts"), "ShortCutsTab");
	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "Chat"), "ChatTab");
	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "Look"), "LookTab");
	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "Network"), "NetworkTab");
	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "History"), "HistoryTab");

	ProtocolsManager::initModule();
	UserList::initModule();
	GroupsManager::initModule();
	emoticons = new EmoticonsManager();


	IconsManager::initModule();
	new Kadu(0, "Kadu");

	qApp->setMainWidget(kadu);
	QPixmap pix;
#ifdef Q_OS_MACX
	pix = icons_manager->loadIcon("BigOffline");
#else
	pix = icons_manager->loadIcon("Offline");
#endif
	kadu->setMainWindowIcon(pix);

	ModulesManager::initModule();

	kadu->startupProcedure();

	QObject::connect(qApp, SIGNAL(aboutToQuit()), kadu, SLOT(quitApplication()));

	// if someone is running Kadu from root account, let's remind him
	// that it's a "bad thing"(tm) ;) (usually for win32 users)
	if (geteuid() == 0)
		MessageBox::wrn(qApp->translate("@default", QT_TR_NOOP("Please do not run Kadu as a root!\nIt's a high security risk!")));
	QTimer::singleShot(15000, kadu, SLOT(deleteOldConfigFiles()));
	int ret = qApp->exec();
//	delete qApp; sometimes leads to segfault
	return ret;
}
