/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMetaType>
#include <QtDBus/QDBusReply>
#include <QtCore/QDateTime>

#include "debug.h"
#include "../mediaplayer/mediaplayer.h"
#include "../mediaplayer/mpris_mediaplayer.h"

MPRISMediaPlayer* amarok2;

extern "C" int amarok2_mediaplayer_init()
{
	amarok2 = new MPRISMediaPlayer("Amarok", "org.kde.amarok");
	bool res = mediaplayer->registerMediaPlayer(amarok2, amarok2);
	return res ? 0 : 1;
}

extern "C" void amarok2_mediaplayer_close()
{
	mediaplayer->unregisterMediaPlayer();
	delete amarok2;
	amarok2 = NULL;
}
