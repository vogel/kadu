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


#include "modules.h"
#include "config_file.h"
#include "debug.h"
#include "mpris.h"

MprisPlayer* mpris;

extern "C" int mpris_mediaplayer_init()
{
	mpris = new MprisPlayer("MPRIS", QString::null);

	MainConfigurationWindow::registerUiFile(
		dataPath("kadu/modules/configuration/mpris_mediaplayer.ui"), mpris);

	bool res = mediaplayer->registerMediaPlayer(mpris, mpris);
	return res ? 0 : 1;
}

extern "C" void mpris_mediaplayer_close()
{
	MainConfigurationWindow::unregisterUiFile(
		dataPath("kadu/modules/configuration/mpris_mediaplayer.ui"), mpris);

	mediaplayer->unregisterMediaPlayer();

	delete mpris;
	mpris = NULL;
}

MprisPlayer::MprisPlayer(QString name, QString service) : MPRISMediaPlayer(name, service)
{
	setService(config_file.readEntry("MediaPlayer", "MPRISService", QString::null));
}

MprisPlayer::~MprisPlayer()
{
}

void MprisPlayer::configurationUpdated()
{
	setService(config_file.readEntry("MediaPlayer", "MPRISService", QString::null));
}

void MprisPlayer::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
}

