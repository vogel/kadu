/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <qtextcodec.h>
#include <qmessagebox.h>
#include <sys/stat.h>
#include <sys/types.h>

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

static const char *description =
	I18N_NOOP("Kadu");
	// INSERT A DESCRIPTION FOR YOUR APPLICATION HERE
	
	
static KCmdLineOptions options[] =
{
	{ 0, 0, 0 }
	// INSERT YOUR COMMANDLINE OPTIONS HERE
};

KApplication * a;
bool mute = false;

int main(int argc, char *argv[])
{
	gg_debug_level = 255;

	KAboutData aboutData( "kadu", I18N_NOOP("Kadu"),
		"0.3.3", description, KAboutData::License_GPL,
		"(c) 2001-2002, Kadu Team", 0, 0, "www.kadu.net");
	aboutData.addAuthor("Kadu Team",0, "www.kadu.net");
	KCmdLineArgs::init( argc, argv, &aboutData );
	KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

	a = new KApplication;
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
		switch (QMessageBox::information( kadu, "Kadu",
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
		kadu->setCaption("Kadu: new user");
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
