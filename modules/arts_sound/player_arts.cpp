/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "player_arts.h"
#include "debug.h"
#include <string>

extern "C" int arts_sound_init()
{
	kdebugf();

	arts_player_slots=new aRtsPlayerSlots(NULL, "arts_player_slots");
	if (arts_player_slots->server.isNull())
	{
		delete arts_player_slots;
		return -1;
	}
	
	kdebugf2();
	return 0;
}
extern "C" void arts_sound_close()
{
	kdebugf();
	delete arts_player_slots;
	arts_player_slots=NULL;
	kdebugf2();
}

aRtsPlayerSlots::aRtsPlayerSlots(QObject *parent, const char *name) : QObject(parent, name)
{
	kdebugf();
	server=Arts::Reference("global:Arts_SoundServerV2");

	connect(sound_manager, SIGNAL(playSound(const QString &, bool, double)),
			this, SLOT(playSound(const QString &, bool, double)));

	kdebugf2();
}

aRtsPlayerSlots::~aRtsPlayerSlots()
{
	kdebugf();
	disconnect(sound_manager, SIGNAL(playSound(const QString &, bool, double)),
			this, SLOT(playSound(const QString &, bool, double)));
	kdebugf2();
}

void aRtsPlayerSlots::playSound(const QString &s, bool volCntrl, double vol)
{
	//warunku server.isNull() i server.audioMethod()=="" nie mo¿na sprawdzaæ
	//jednocze¶nie, bo je¿eli najpierw zostanie sprawdzony drugi, a pierwszy
	//jest prawd±, to program siê wywróci
	kdebugf();
	kdebugmf(KDEBUG_INFO, "path:%s: null: %d\n", s.local8Bit().data(), server.isNull());

	if (!server.isNull())
		kdebugm(KDEBUG_INFO, "audioMethod: %s audioDevice: %s\n", server.audioMethod().c_str(), server.audioDevice().c_str());

	if (server.isNull())
		server=Arts::Reference("global:Arts_SoundServerV2");
	else if (server.audioMethod()=="")
		server=Arts::Reference("global:Arts_SoundServerV2");
	
	if (!server.isNull())
		if (server.audioMethod()!="")
		{
			if (volCntrl)
				server.outVolume().scaleFactor(vol);
			server.play(std::string(s.ascii()));
		}
	kdebugf2();
}

aRtsPlayerSlots *arts_player_slots;
