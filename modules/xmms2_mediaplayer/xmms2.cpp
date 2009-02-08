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

MPRISMediaPlayer* xmms2;

extern "C" int xmms2_mediaplayer_init()
{
	xmms2 = new MPRISMediaPlayer("XMMS2", "org.mpris.xmms2");
	bool res = mediaplayer->registerMediaPlayer(xmms2, xmms2);
	return res ? 0 : 1;
}

extern "C" void xmms2_mediaplayer_close()
{
	mediaplayer->unregisterMediaPlayer();
	delete xmms2;
	xmms2 = NULL;
}
