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

	artsPlayerObj=new ArtsPlayerSlots();
	if (artsPlayerObj->server.isNull())
	{
		delete artsPlayerObj;
		return -1;
	}
	
	kdebugf2();
	return 0;
}
extern "C" void arts_sound_close()
{
	kdebugf();
	delete artsPlayerObj;
	artsPlayerObj=NULL;
	kdebugf2();
}

ArtsPlayerSlots::ArtsPlayerSlots() : QObject(NULL, "artsPlayerObj")
{
	kdebugf();
	server=Arts::Reference("global:Arts_SoundServerV2");

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

ArtsPlayerSlots::~ArtsPlayerSlots()
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
	kdebugf2();
}

void ArtsPlayerSlots::play(const QString &s, bool volCntrl, double vol)
{
	//warunku server.isNull() i server.audioMethod()=="" nie mo¿na sprawdzaæ
	//jednocze¶nie, bo je¿eli najpierw zostanie sprawdzony drugi, a pierwszy
	//jest prawd±, to program siê wywróci
	kdebugf();
	kdebugm(KDEBUG_INFO, "ArtsPlayerSlots::play(%s): null: %d\n",s.local8Bit().data(), server.isNull());

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

void ArtsPlayerSlots::playSound(const QString &s, bool volCntrl, double vol)
{
	kdebugf();
	play(s, volCntrl, vol);
}

void ArtsPlayerSlots::playNewMessage(UinsList senders, const QString &sound, bool volCntrl, double vol, const QString &msg)
{
	kdebugf();
	play(sound, volCntrl, vol);
}

void ArtsPlayerSlots::playNewChat(UinsList senders, const QString &sound, bool volCntrl, double vol, const QString &msg)
{
	kdebugf();
	play(sound, volCntrl, vol);
}

void ArtsPlayerSlots::playConnectionError(const QString &sound, bool volCntrl, double vol, const QString &msg)
{
	kdebugf();
	play(sound, volCntrl, vol);
}

void ArtsPlayerSlots::playNotify(const UinType uin, const QString &sound, bool volCntrl, double vol)
{
	kdebugf();
	play(sound, volCntrl, vol);
}

void ArtsPlayerSlots::playMessage(const QString &sound, bool volCntrl, double vol, const QString &from, const QString &type, const QString &msg, const UserListElement *ule)
{
	kdebugf();
	play(sound, volCntrl, vol);
}

ArtsPlayerSlots *artsPlayerObj;
