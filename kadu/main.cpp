/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : wto sie 21 18:35:52 CEST 2001
    copyright            : (C) 2001 by tomee
    email                : tomee@cpi.pl
 ***************************************************************************/

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
#include "../libgadu/lib/libgadu.h"

//
#include "kadu.h"
#include "pixmaps.h"
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
		"0.3.1", description, KAboutData::License_GPL,
		"(c) 2001-2002, tomee", 0, 0, "tomee@cpi.pl");
	aboutData.addAuthor("tomee",0, "tomee@cpi.pl");
	KCmdLineArgs::init( argc, argv, &aboutData );
	KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

	a = new KApplication;
	a->setDefaultCodec( QTextCodec::codecForName("ISO 8859-2"));
	kadu = new Kadu(0, "Kadu");
	QPixmap px((const char **)gg_inact_xpm);
	kadu->setIcon(px);
	a->setMainWidget(kadu);

	kadu->show();
	if (config.defaultstatus != GG_STATUS_NOT_AVAIL && config.defaultstatus != GG_STATUS_NOT_AVAIL_DESCR) {
		kadu->autohammer = true;
		own_description = config.defaultdescription;
		kadu->setStatus(config.defaultstatus);	
		}

	QObject::connect(a, SIGNAL(aboutToQuit()), kadu, SLOT(cleanUp()));
	return a->exec();
}
