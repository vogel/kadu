/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <esd.h>
#include "esd_sound.h"
#include "debug.h"

extern "C" int esd_sound_init()
{
	kdebugf();

	esdPlayerObj=new ESDPlayerSlots();
	if (esdPlayerObj->sock<0)
	{
		delete esdPlayerObj;
		return -1;
	}

	QObject::connect(sound_manager, SIGNAL(playSound(const QString &, bool, double)),
					 esdPlayerObj, SLOT(playSound(const QString &, bool, double)));
	QObject::connect(sound_manager, SIGNAL(playOnMessage(UinsList, const QString &, const QString &, bool, double)),
					 esdPlayerObj, SLOT(playMessage(UinsList, const QString &, const QString &, bool, double)));
	QObject::connect(sound_manager, SIGNAL(playOnChat(UinsList, const QString &, const QString &, bool, double)),
					 esdPlayerObj, SLOT(playChat(UinsList, const QString &, const QString &, bool, double)));
	QObject::connect(sound_manager, SIGNAL(playOnNotify(const UinType, const QString &, bool, double)),
					 esdPlayerObj, SLOT(playNotify(const UinType, const QString &, bool, double)));

	return 0;
}
extern "C" void esd_sound_close()
{
	kdebugf();

	QObject::disconnect(sound_manager, SIGNAL(playSound(const QString &, bool, double)),
						esdPlayerObj, SLOT(playSound(const QString &, bool, double)));
	QObject::disconnect(sound_manager, SIGNAL(playOnMessage(UinsList, const QString &, const QString &, bool, double)),
						esdPlayerObj, SLOT(playMessage(UinsList, const QString &, const QString &, bool, double)));
	QObject::disconnect(sound_manager, SIGNAL(playOnChat(UinsList, const QString &, const QString &, bool, double)),
						esdPlayerObj, SLOT(playChat(UinsList, const QString &, const QString &, bool, double)));
	QObject::disconnect(sound_manager, SIGNAL(playOnNotify(const UinType, const QString &, bool, double)),
						esdPlayerObj, SLOT(playNotify(const UinType, const QString &, bool, double)));
	delete esdPlayerObj;
	esdPlayerObj=NULL;
}

ESDPlayerSlots::ESDPlayerSlots()
{
	kdebugf();
	sock=esd_open_sound(NULL);
}

ESDPlayerSlots::~ESDPlayerSlots()
{
	kdebugf();
	esd_close(sock);
	sock=-1;
}

void ESDPlayerSlots::play(const QString &s, bool volCntrl, double vol)
{
	kdebugf();
	int id=esd_file_cache(sock, "Kadu", s.ascii());
	if (id<=0)
		return;

	int sc=int(ESD_VOLUME_BASE*vol);
	if (volCntrl)
		esd_set_default_sample_pan(sock, id, sc, sc);

	esd_sample_play(sock, id);
	esd_sample_free(sock, id);
}

void ESDPlayerSlots::playSound(const QString &s, bool volCntrl, double vol)
{
	kdebugf();
	play(s, volCntrl, vol);
}

void ESDPlayerSlots::playMessage(UinsList senders, const QString &sound, const QString &msg, bool volCntrl, double vol)
{
	kdebugf();
	play(sound, volCntrl, vol);
}

void ESDPlayerSlots::playChat(UinsList senders, const QString &sound, const QString &msg, bool volCntrl, double vol)
{
	kdebugf();
	play(sound, volCntrl, vol);
}

void ESDPlayerSlots::playNotify(const UinType uin, const QString &sound, bool volCntrl, double vol)
{
	kdebugf();
	play(sound, volCntrl, vol);
}

ESDPlayerSlots *esdPlayerObj;
