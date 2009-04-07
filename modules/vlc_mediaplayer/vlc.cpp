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

MPRISMediaPlayer* vlc;

extern "C" int vlc_mediaplayer_init()
{
	vlc = new MPRISMediaPlayer("VLC", "org.mpris.vlc");
	bool res = mediaplayer->registerMediaPlayer(vlc, vlc);
	return res ? 0 : 1;
}

extern "C" void vlc_mediaplayer_close()
{
	mediaplayer->unregisterMediaPlayer();
	delete vlc;
	vlc = NULL;
}
