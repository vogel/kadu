/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "nas_sound.h"
#include "debug.h"
#include <qapplication.h>

#ifdef INTERNAL_QT_SOUND_SUPPORT
#include <qsound.h>
#else
#define AuFixedPointFromFraction2(nnn,ddd) \
    ((((audiolib::AuInt32) (nnn)) * AU_FIXED_POINT_SCALE) / ((audiolib::AuInt32) (ddd)))
#endif

extern "C" int nas_sound_init()
{
	kdebugf();

	nasPlayerObj=new NASPlayerSlots();
	if (!nasPlayerObj->isConnected())
	{
		delete nasPlayerObj;
		return 2;
	}

	kdebugf2();
	return 0;
}
extern "C" void nas_sound_close()
{
	kdebugf();

	delete nasPlayerObj;
	nasPlayerObj=NULL;

	kdebugf2();
}

NASPlayerSlots::NASPlayerSlots()
{
	kdebugf();
#ifndef INTERNAL_QT_SOUND_SUPPORT
	auserver=audiolib::AuOpenServer(NULL, 0, NULL, 0, NULL, NULL);
	if (auserver)
	{
		sn=new QSocketNotifier(AuServerConnectionNumber(auserver), QSocketNotifier::Read);
		QObject::connect(sn, SIGNAL(activated(int)), this, SLOT(dataReceived()));
	}
#endif

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

NASPlayerSlots::~NASPlayerSlots()
{
	kdebugf();
#ifndef INTERNAL_QT_SOUND_SUPPORT
	if (auserver)
	{
		audiolib::AuCloseServer(auserver);
		QObject::disconnect(sn, SIGNAL(activated(int)), this, SLOT(dataReceived()));
		delete sn;
	}
#endif

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

bool NASPlayerSlots::isConnected()
{
#ifdef INTERNAL_QT_SOUND_SUPPORT
	return QSound::available();
#else
	return (auserver!=NULL);
#endif
}

void NASPlayerSlots::playSound(const QString &s, bool volCntrl, double vol)
{
	kdebugf();
	if (!volCntrl)
		vol=1;
	if (isConnected())
	{
#ifdef INTERNAL_QT_SOUND_SUPPORT
		QSound::play(s);
#else
		audiolib::AuFixedPoint volume=AuFixedPointFromFraction2(int(vol*100),100);
		audiolib::AuSoundPlayFromFile(auserver, (const char *)s.local8Bit(), AuNone, volume, NULL, NULL, NULL, NULL, NULL, NULL);
		audiolib::AuFlush(auserver);
		dataReceived();
		audiolib::AuFlush(auserver);
		qApp->flushX();
#endif
	}
	else
		kdebugm(KDEBUG_WARNING, "not connected\n");
	kdebugf2();
}

void NASPlayerSlots::dataReceived()
{
#ifndef INTERNAL_QT_SOUND_SUPPORT
	audiolib::AuHandleEvents(auserver);
#endif
}

void NASPlayerSlots::playNewMessage(UinsList senders, const QString &sound, bool volCntrl, double vol, const QString &msg)
{
	kdebugf();
	playSound(sound, volCntrl, vol);
}

void NASPlayerSlots::playNewChat(UinsList senders, const QString &sound, bool volCntrl, double vol, const QString &msg)
{
	kdebugf();
	playSound(sound, volCntrl, vol);
}

void NASPlayerSlots::playNotify(const UinType uin, const QString &sound, bool volCntrl, double vol)
{
	kdebugf();
	playSound(sound, volCntrl, vol);
}

void NASPlayerSlots::playConnectionError(const QString &sound, bool volCntrl, double vol, const QString &msg)
{
	kdebugf();
	playSound(sound, volCntrl, vol);
}

void NASPlayerSlots::playMessage(const QString &sound, bool volCntrl, double vol, const QString &from, const QString &type, const QString &msg, const UserListElement *ule)
{
	kdebugf();
	playSound(sound, volCntrl, vol);
}

NASPlayerSlots *nasPlayerObj;
