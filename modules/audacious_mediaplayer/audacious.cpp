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

MPRISMediaPlayer* audacious;

extern "C" int audacious_mediaplayer_init()
{
	audacious = new MPRISMediaPlayer("Audacious", "org.mpris.audacious");
	bool res = mediaplayer->registerMediaPlayer(audacious, audacious);
	return res ? 0 : 1;
}

extern "C" void audacious_mediaplayer_close()
{
	mediaplayer->unregisterMediaPlayer();
	delete audacious;
	audacious = NULL;
}
