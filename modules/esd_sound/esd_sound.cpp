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

	kdebugf2();
	return 0;
}
extern "C" void esd_sound_close()
{
	kdebugf();
	delete esdPlayerObj;
	esdPlayerObj=NULL;
	kdebugf2();
}

ESDPlayerSlots::ESDPlayerSlots()
{
	kdebugf();
	sock=esd_open_sound(NULL);

	connect(sound_manager, SIGNAL(playSound(const QString &, bool, double)),
			this, SLOT(playSound(const QString &, bool, double)));
	connect(sound_manager, SIGNAL(playOnNewMessage(UinsList, const QString &, bool, double, const QString &)),
			this, SLOT(playNewMessage(UinsList, const QString &, bool, double, const QString &)));
	connect(sound_manager, SIGNAL(playOnNewChat(UinsList, const QString &, bool, double, const QString &)),
			this, SLOT(playNewChat(UinsList, const QString &, bool, double, const QString &)));
	connect(sound_manager, SIGNAL(playOnConnectionError(const QString &, bool, double, const QString &)),
			this, SLOT(playConnectionError(const QString &, bool, double, const QString &)));
	connect(sound_manager, SIGNAL(playOnNotifyAvail(const UinType, const QString &, bool, double)),
			this, SLOT(playNotify(const UinType, const QString &, bool, double)));
	connect(sound_manager, SIGNAL(playOnNotifyBusy(const UinType, const QString &, bool, double)),
			this, SLOT(playNotify(const UinType, const QString &, bool, double)));
	connect(sound_manager, SIGNAL(playOnNotifyNotAvail(const UinType, const QString &, bool, double)),
			this, SLOT(playNotify(const UinType, const QString &, bool, double)));
	connect(sound_manager, SIGNAL(playOnMessage(const QString &, bool, double, const QString &, const QString &, const QString &, const UserListElement *)),
			this, SLOT(playMessage(const QString &, bool, double, const QString &, const QString &, const QString &, const UserListElement *)));

	kdebugf2();
}

ESDPlayerSlots::~ESDPlayerSlots()
{
	kdebugf();

	disconnect(sound_manager, SIGNAL(playSound(const QString &, bool, double)),
			this, SLOT(playSound(const QString &, bool, double)));
	disconnect(sound_manager, SIGNAL(playOnNewMessage(UinsList, const QString &, bool, double, const QString &)),
			this, SLOT(playNewMessage(UinsList, const QString &, bool, double, const QString &)));
	disconnect(sound_manager, SIGNAL(playOnNewChat(UinsList, const QString &, bool, double, const QString &)),
			this, SLOT(playNewChat(UinsList, const QString &, bool, double, const QString &)));
	disconnect(sound_manager, SIGNAL(playOnConnectionError(const QString &, bool, double, const QString &)),
			this, SLOT(playConnectionError(const QString &, bool, double, const QString &)));
	disconnect(sound_manager, SIGNAL(playOnNotifyAvail(const UinType, const QString &, bool, double)),
			this, SLOT(playNotify(const UinType, const QString &, bool, double)));
	disconnect(sound_manager, SIGNAL(playOnNotifyBusy(const UinType, const QString &, bool, double)),
			this, SLOT(playNotify(const UinType, const QString &, bool, double)));
	disconnect(sound_manager, SIGNAL(playOnNotifyNotAvail(const UinType, const QString &, bool, double)),
			this, SLOT(playNotify(const UinType, const QString &, bool, double)));
	disconnect(sound_manager, SIGNAL(playOnMessage(const QString &, bool, double, const QString &, const QString &, const QString &, const UserListElement *)),
			this, SLOT(playMessage(const QString &, bool, double, const QString &, const QString &, const QString &, const UserListElement *)));

	esd_close(sock);
	sock=-1;
	kdebugf2();
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
	kdebugf2();
}

void ESDPlayerSlots::playSound(const QString &s, bool volCntrl, double vol)
{
	kdebugf();
	play(s, volCntrl, vol);
}

void ESDPlayerSlots::playNewMessage(UinsList senders, const QString &sound, bool volCntrl, double vol, const QString &msg)
{
	kdebugf();
	play(sound, volCntrl, vol);
}

void ESDPlayerSlots::playNewChat(UinsList senders, const QString &sound, bool volCntrl, double vol, const QString &msg)
{
	kdebugf();
	play(sound, volCntrl, vol);
}

void ESDPlayerSlots::playNotify(const UinType uin, const QString &sound, bool volCntrl, double vol)
{
	kdebugf();
	play(sound, volCntrl, vol);
}

void ESDPlayerSlots::playConnectionError(const QString &sound, bool volCntrl, double vol, const QString &msg)
{
	kdebugf();
	play(sound, volCntrl, vol);
}

void ESDPlayerSlots::playMessage(const QString &sound, bool volCntrl, double vol, const QString &from, const QString &type, const QString &msg, const UserListElement *ule)
{
	kdebugf();
	play(sound, volCntrl, vol);
}

ESDPlayerSlots *esdPlayerObj;
