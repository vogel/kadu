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

	esd_player_slots=new ESDPlayerSlots(NULL, "esd_player_slots");
	if (esd_player_slots->sock<0)
	{
		delete esd_player_slots;
		return -1;
	}

	kdebugf2();
	return 0;
}
extern "C" void esd_sound_close()
{
	kdebugf();
	delete esd_player_slots;
	esd_player_slots=NULL;
	kdebugf2();
}

ESDPlayerSlots::ESDPlayerSlots(QObject *parent, const char *name) : QObject(parent, name)
{
	kdebugf();
	sock=esd_open_sound(NULL);

	connect(sound_manager, SIGNAL(playSound(const QString &, bool, double)),
			this, SLOT(playSound(const QString &, bool, double)));

	kdebugf2();
}

ESDPlayerSlots::~ESDPlayerSlots()
{
	kdebugf();

	disconnect(sound_manager, SIGNAL(playSound(const QString &, bool, double)),
			this, SLOT(playSound(const QString &, bool, double)));

	esd_close(sock);
	sock=-1;
	kdebugf2();
}

void ESDPlayerSlots::playSound(const QString &s, bool volCntrl, double vol)
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

ESDPlayerSlots *esd_player_slots;
