/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qsound.h>
#include "nas_sound.h"
#include "debug.h"

SoundManager* sound_manager;

extern "C" void nas_sound_info(ModuleInfo* i)
{
    i->description="Network Audio System sound module";
    i->author="Joi";
    i->depends+="sound";
}

extern "C" int nas_sound_init()
{
	kdebugf();
	sound_manager=soundManager();
	if(sound_manager==NULL)
		return 1;

	slotsObj=new NASPlayerSlots();

	QObject::connect(sound_manager, SIGNAL(playSound(const QString &, bool, double)),
					 slotsObj, SLOT(playSound(const QString &, bool, double)));
	QObject::connect(sound_manager, SIGNAL(playOnMessage(UinsList, const QString &, const QString &, bool, double)),
					 slotsObj, SLOT(playMessage(UinsList, const QString &, const QString &, bool, double)));
	QObject::connect(sound_manager, SIGNAL(playOnChat(UinsList, const QString &, const QString &, bool, double)),
					 slotsObj, SLOT(playChat(UinsList, const QString &, const QString &, bool, double)));
	QObject::connect(sound_manager, SIGNAL(playOnNotify(const uin_t, const QString &, bool, double)),
					 slotsObj, SLOT(playNotify(const uin_t, const QString &, bool, double)));

	return 0;
}
extern "C" void nas_sound_close()
{
	kdebugf();

	QObject::disconnect(sound_manager, SIGNAL(playSound(const QString &, bool, double)),
						slotsObj, SLOT(playSound(const QString &, bool, double)));
	QObject::disconnect(sound_manager, SIGNAL(playOnMessage(UinsList, const QString &, const QString &, bool, double)),
						slotsObj, SLOT(playMessage(UinsList, const QString &, const QString &, bool, double)));
	QObject::disconnect(sound_manager, SIGNAL(playOnChat(UinsList, const QString &, const QString &, bool, double)),
						slotsObj, SLOT(playChat(UinsList, const QString &, const QString &, bool, double)));
	QObject::disconnect(sound_manager, SIGNAL(playOnNotify(const uin_t, const QString &, bool, double)),
						slotsObj, SLOT(playNotify(const uin_t, const QString &, bool, double)));
	delete slotsObj;
	slotsObj=NULL;
}

NASPlayerSlots::NASPlayerSlots()
{
}

void NASPlayerSlots::playSound(const QString &s, bool volCntrl, double vol)
{
	kdebugf();
	QSound::play(s);
}

void NASPlayerSlots::playMessage(UinsList senders, const QString &sound, const QString &msg, bool volCntrl, double vol)
{
	kdebugf();
	QSound::play(sound);
}

void NASPlayerSlots::playChat(UinsList senders, const QString &sound, const QString &msg, bool volCntrl, double vol)
{
	kdebugf();
	QSound::play(sound);
}

void NASPlayerSlots::playNotify(const uin_t uin, const QString &sound, bool volCntrl, double vol)
{
	kdebugf();
	QSound::play(sound);
}

NASPlayerSlots *slotsObj;
