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

SoundManager* sound_manager;

extern "C" void esd_sound_info(ModuleInfo* i)
{
    i->description="ESD sound module";
    i->author="Joi";
    i->depends+="sound";
}

extern "C" int esd_sound_init()
{
	kdebugf();

	sound_manager=soundManager();
	if(sound_manager==NULL)
		return 1;

	slotsObj=new ESDPlayerSlots();
	if (slotsObj->sock<0)
	{
		delete slotsObj;
		return -1;
	}

	QObject::connect(sound_manager, SIGNAL(playOnTestSound(const QString &, bool, double)),
					 slotsObj, SLOT(playTestSound(const QString &, bool, double)));
	QObject::connect(sound_manager, SIGNAL(playOnMessage(UinsList, const QString &, const QString &, bool, double)),
					 slotsObj, SLOT(playMessage(UinsList, const QString &, const QString &, bool, double)));
	QObject::connect(sound_manager, SIGNAL(playOnChat(UinsList, const QString &, const QString &, bool, double)),
					 slotsObj, SLOT(playChat(UinsList, const QString &, const QString &, bool, double)));
	QObject::connect(sound_manager, SIGNAL(playOnNotify(const uin_t, const QString &, bool, double)),
					 slotsObj, SLOT(playNotify(const uin_t, const QString &, bool, double)));

	return 0;
}
extern "C" void esd_sound_close()
{
	kdebugf();

	QObject::disconnect(sound_manager, SIGNAL(playOnTestSound(const QString &, bool, double)),
						slotsObj, SLOT(playTestSound(const QString &, bool, double)));
	QObject::disconnect(sound_manager, SIGNAL(playOnMessage(UinsList, const QString &, const QString &, bool, double)),
						slotsObj, SLOT(playMessage(UinsList, const QString &, const QString &, bool, double)));
	QObject::disconnect(sound_manager, SIGNAL(playOnChat(UinsList, const QString &, const QString &, bool, double)),
						slotsObj, SLOT(playChat(UinsList, const QString &, const QString &, bool, double)));
	QObject::disconnect(sound_manager, SIGNAL(playOnNotify(const uin_t, const QString &, bool, double)),
						slotsObj, SLOT(playNotify(const uin_t, const QString &, bool, double)));
	delete slotsObj;
	slotsObj=NULL;
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

void ESDPlayerSlots::playTestSound(const QString &s, bool volCntrl, double vol)
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

void ESDPlayerSlots::playNotify(const uin_t uin, const QString &sound, bool volCntrl, double vol)
{
	kdebugf();
	play(sound, volCntrl, vol);
}

ESDPlayerSlots *slotsObj;
