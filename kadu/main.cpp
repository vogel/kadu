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

#include "kadu.h"
#include "kadu-config.h"

#ifdef VOICE_ENABLED
#include "voice.h"
#endif

#include "config_file.h"
#include "config_dialog.h"
#include "register.h"
#include "misc.h"
#include "debug.h"
#include "modules.h"

Kadu *kadu;	

//#define HAVE_EXECINFO
#undef HAVE_EXECINFO

#ifdef SIG_HANDLING_ENABLED
#include <qdatetime.h>
#include <signal.h>
#include <stdlib.h>
#ifdef HAVE_EXECINFO
#include <execinfo.h>
#endif
void kadu_signal_handler(int s)
{
	kdebug("kadu_signal_handler: %d\n", s);
	if(lockFile){ // moze sie wywalic praktycznie po wylaczeniu i to tez trzeba uwzglednic	
		flock(lockFileHandle, LOCK_UN);
		kdebug("lock released\n");
		lockFile->close();
		kdebug("lockfile closed\n");
	}
	
	QString f=QString("kadu.conf.backup.%1").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd.hh.mm.ss"));
	if (s==SIGSEGV)
	{
		kdebug("Kadu crashed :(\n");
#ifdef HAVE_EXECINFO
		void *bt_array[100];
		char **bt_strings;
		int num_entries;
		if ((num_entries = backtrace(bt_array, 100)) < 0) {
			kdebug("could not generate backtrace\n");
			return;
		}
		if ((bt_strings = backtrace_symbols(bt_array, num_entries)) == NULL) {
			kdebug("could not get symbol names for backtrace\n");
			return;
		}
		fprintf(stderr, "\n======= BEGIN OF BACKTRACE =====\n");
		for (int i = 0; i < num_entries; i++) {
			fprintf(stderr, "[%d] %s\n", i, bt_strings[i]);
		}
		fprintf(stderr, "======= END OF BACKTRACE  ======\n");
		free(bt_strings);
#else
		kdebug("backtrace not available\n");		
#endif
		config_file.saveTo(ggPath(f.latin1()));
		abort();
	}
	else if (s==SIGINT || s==SIGTERM)
		config_file.saveTo(ggPath(f.latin1()));
	exit(0);
}
#endif

int main(int argc, char *argv[])
{
	gg_debug_level = 255;

#ifdef SIG_HANDLING_ENABLED
	signal(SIGSEGV, kadu_signal_handler);
	signal(SIGINT, kadu_signal_handler);
	signal(SIGTERM, kadu_signal_handler);
#endif

	new QApplication(argc, argv);

#ifdef VOICE_ENABLED
	VoiceManager::initModule();
#endif

	// ladowanie tlumaczenia
	QTranslator qt_qm(0);
	QString lang=config_file.readEntry("General", "Language", QTextCodec::locale());
	qt_qm.load(dataPath(QString("kadu/translations/qt_") + lang), ".");
	qApp->installTranslator(&qt_qm);
	QTranslator kadu_qm(0);
	kadu_qm.load(dataPath(QString("kadu/translations/kadu_") + lang), ".");
	qApp->installTranslator(&kadu_qm);

	lockFile=new QFile(ggPath("lock"));
	if (lockFile->open(IO_ReadWrite))
	{
		lockFileHandle=lockFile->handle();
		if (flock(lockFileHandle, LOCK_EX|LOCK_NB)!=0)
		{
			kdebug("flock: %s\n", strerror(errno));
			if (QMessageBox::warning(NULL, "Kadu lock",
				qApp->translate("@default", QT_TR_NOOP("Lock file in profile directory exists. Another Kadu probably running.")),
				qApp->translate("@default", QT_TR_NOOP("Force running Kadu (not recommended).")),
				qApp->translate("@default", QT_TR_NOOP("Quit.")), 0, 1)==1 )
			return 1;
		}
	}

	IconsManager::initModule();
	kadu = new Kadu(0, "Kadu");

	Kadu::InitModules();
	QPixmap pix = icons_manager.loadIcon("Offline");
	kadu->setIcon(pix);
	qApp->setMainWidget(kadu);

	ModulesManager::initModule();

	if (!config_file.readNumEntry("General","UIN")) {
		QString path_;
		path_ = ggPath("");
		mkdir(path_.local8Bit(), 0700);
		path_.append("/history/");
		mkdir(path_.local8Bit(), 0700);
		switch (QMessageBox::information(kadu, "Kadu",
			qApp->translate("@default", QT_TR_NOOP("You don't have a config file.\nWhat would you like to do?")),
			qApp->translate("@default", QT_TR_NOOP("New UIN")),
			qApp->translate("@default", QT_TR_NOOP("Configure")),
			qApp->translate("@default", QT_TR_NOOP("Cancel")), 0, 1) ) {
			case 1: // Configure
				ConfigDialog::showConfigDialog(qApp);
				break;
			case 0: // Register
				Register *reg;
				reg = new Register;
				reg->show();
				break;
			case 2: // Nothing
				break;
			}
		kadu->setCaption(qApp->translate("@default", QT_TR_NOOP("Kadu: new user")));
		}

	own_description = defaultdescriptions.first();
	int defaultStatus=config_file.readNumEntry("General","DefaultStatus",GG_STATUS_NOT_AVAIL);
	if (defaultStatus != GG_STATUS_NOT_AVAIL && defaultStatus != GG_STATUS_NOT_AVAIL_DESCR) {
		kadu->autohammer = true;
		kadu->setStatus(defaultStatus);
		}

	QObject::connect(qApp, SIGNAL(aboutToQuit()), kadu, SLOT(quitApplication()));


	return qApp->exec();
}
