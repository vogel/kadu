/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <kcmdlineargs.h>
#include <kapplication.h>
#include <qapplication.h>
#include <qtextcodec.h>
#include <qmessagebox.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <locale.h>
#include <libintl.h>

//
#include "kadu.h"
#include "pixmaps.h"
#include "config.h"
#include "register.h"
#include "../config.h"
#ifdef HAVE_OPENSSL
#include "simlite.h"
#endif
//

Kadu *kadu;	
QApplication * a;
//KApplication * a;
bool mute = false;

int main(int argc, char *argv[])
{
	gg_debug_level = 255;

	setlocale(LC_ALL, "");
	bindtextdomain("kadu", (QString(DATADIR)+"/locale").local8Bit().data());
//	bind_textdomain_codeset("kadu", "ISO-8859-2");
	textdomain("kadu");

	KCmdLineArgs::init( argc, argv, "Kadu", "GG-Client for X-Win", VERSION);
	a = new KApplication;
//	a = new QApplication(argc, argv);
	a->setDefaultCodec( QTextCodec::codecForName("ISO 8859-2"));
	kadu = new Kadu(0, "Kadu");
	QPixmap px((const char **)gg_inact_xpm);
	kadu->setIcon(px);
	a->setMainWidget(kadu);
	
	// pokazanie okna przeniesione do konstruktora z powodu RunDocked
//	kadu->show();
	if (!config.uin) {
		QString path_;
		path_ = ggPath("");
		mkdir(path_.local8Bit(), 0700);
		path_.append("/history/");
		mkdir(path_.local8Bit(), 0700);
		switch (QMessageBox::information(kadu, "Kadu",
			i18n("You don't have a config file.\nWhat would you like to do?"),
			i18n("New UIN"), i18n("Configure"), i18n("Cancel"), 0, 1) ) {
			case 1: // Configure
				ConfigDialog *cd;
				cd = new ConfigDialog;
				cd->show();
				break;
			case 0: // Register
				Register *reg;
				reg = new Register;
				reg->show();
				break;
			case 2: // Nothing
				break;
			}
		kadu->setCaption(i18n("Kadu: new user"));
		}

	if (config.defaultstatus != GG_STATUS_NOT_AVAIL && config.defaultstatus != GG_STATUS_NOT_AVAIL_DESCR) {
		kadu->autohammer = true;
		own_description = config.defaultdescription;
		kadu->setStatus(config.defaultstatus);	
		}
#ifdef HAVE_OPENSSL
		sim_key_path=strdup(ggPath("keys/").local8Bit());
#endif

	QObject::connect(a, SIGNAL(aboutToQuit()), kadu, SLOT(cleanUp()));
	return a->exec();
}
