/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <qapplication.h>
#include <qtranslator.h>
#include <qtextcodec.h>
#include <qmessagebox.h>

#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "kadu.h"
#include "kadu-config.h"

#include "config_file.h"
#include "config_dialog.h"
#include "misc.h"
#include "debug.h"
#include "modules.h"
#include "emoticons.h"
#include "message_box.h"

Kadu *kadu;	

#ifdef SIG_HANDLING_ENABLED
#include <qdatetime.h>
#include <signal.h>
#ifdef HAVE_EXECINFO
#include <execinfo.h>
#endif
void kadu_signal_handler(int s)
{
	kdebugmf(KDEBUG_WARNING, "%d\n", s);
	
	if (s==SIGSEGV)
	{
		kdebugm(KDEBUG_PANIC, "Kadu crashed :(\n");
		QString f=QString("kadu.conf.backup.%1").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd.hh.mm.ss"));

		if(lockFile)
		{ // moze sie wywalic praktycznie po wylaczeniu i to tez trzeba uwzglednic	
			flock(lockFileHandle, LOCK_UN);
			kdebugm(KDEBUG_WARNING, "lock released\n");
			lockFile->close();
			kdebugm(KDEBUG_WARNING, "lockfile closed\n");
		}
#ifdef HAVE_EXECINFO
		void *bt_array[100];
		char **bt_strings;
		int num_entries;
		if ((num_entries = backtrace(bt_array, 100)) < 0) {
			kdebugm(KDEBUG_PANIC, "could not generate backtrace\n");
			abort();
		}
		if ((bt_strings = backtrace_symbols(bt_array, num_entries)) == NULL) {
			kdebugm(KDEBUG_PANIC, "could not get symbol names for backtrace\n");
			abort();
		}
		fprintf(stderr, "\n======= BEGIN OF BACKTRACE =====\n");
		for (int i = 0; i < num_entries; ++i)
			fprintf(stderr, "[%d] %s\n", i, bt_strings[i]);
		fprintf(stderr, "======= END OF BACKTRACE  ======\n");
		free(bt_strings);
#else
		kdebugm(KDEBUG_PANIC, "backtrace not available\n");		
#endif
		config_file.saveTo(ggPath(f.latin1()));
		abort();
	}
	else if (s==SIGINT || s==SIGTERM)
		qApp->postEvent(qApp, new QEvent(QEvent::Quit));
}
#endif

int main(int argc, char *argv[])
{
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

	//opó¼nienie uruchomienia, przydatne w GNOME
	config_file.addVariable("General", "StartDelay", 0);
	sleep(config_file.readNumEntry("General", "StartDelay"));

	dataPath("", argv[0]);
	emoticons=new EmoticonsManager();

	new QApplication(argc, argv);

	// ³adowanie t³umaczenia
	config_file.addVariable("General", "Language", QString(QTextCodec::locale()).mid(0,2));
	QTranslator qt_qm(0, "Translator_qt");
	QString lang=config_file.readEntry("General", "Language");
	qt_qm.load(dataPath(QString("kadu/translations/qt_") + lang), ".");
	qApp->installTranslator(&qt_qm);
	QTranslator kadu_qm(0, "Translator_kadu");
	kadu_qm.load(dataPath(QString("kadu/translations/kadu_") + lang), ".");
	qApp->installTranslator(&kadu_qm);
	qApp->setStyle(config_file.readEntry("Look", "QtStyle"));

	lockFile=new QFile(ggPath("lock"));
	if (lockFile->open(IO_ReadWrite))
	{
		lockFileHandle=lockFile->handle();
		if (flock(lockFileHandle, LOCK_EX|LOCK_NB)!=0)
		{
			kdebugm(KDEBUG_WARNING, "flock: %s\n", strerror(errno));
			if (QMessageBox::warning(NULL, "Kadu lock",
				qApp->translate("@default", QT_TR_NOOP("Lock file in profile directory exists. Another Kadu probably running.")),
				qApp->translate("@default", QT_TR_NOOP("Force running Kadu (not recommended).")),
				qApp->translate("@default", QT_TR_NOOP("Quit.")), 0, 1)==1 )
			return 1;
		}
	}

	IconsManager::initModule();
	kadu = new Kadu(0, "Kadu");

	QPixmap pix = icons_manager.loadIcon("Offline");
	kadu->setIcon(pix);
	qApp->setMainWidget(kadu);

	ModulesManager::initModule();

	kadu->startupProcedure();

	QObject::connect(qApp, SIGNAL(aboutToQuit()), kadu, SLOT(quitApplication()));

	// je¶li kto¶ uruchomi³ kadu jako root to przypomnijmy mu, ¿e
	// tak nie nale¿y postêpowaæ (leczymy nawyki z win32)
	if (geteuid() == 0)
		MessageBox::wrn(qApp->translate("@default", QT_TR_NOOP("Please do not run Kadu as a root!\nIt's a high security risk!")));

	return qApp->exec();
}
