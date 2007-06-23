/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdlib.h>
#include <qfiledialog.h>

#include "../sound/sound.h"

#include "config_file.h"
#include "debug.h"

#include "player_external.h"

/**
 * @ingroup ext_sound
 * @{
 */
extern "C" int ext_sound_init()
{
	kdebugf();

	external_player = new ExternalPlayer();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/ext_sound.ui"), 0);

	kdebugf2();
	return 0;
}
extern "C" void ext_sound_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/ext_sound.ui"), 0);
	delete external_player;
	external_player = 0;

	kdebugf2();
}

ExternalPlayer::ExternalPlayer()
{
	kdebugf();

#ifdef Q_OS_MACX
	config_file.addVariable("Sounds", "SoundPlayer", "/Applications/Kadu.app/Contents/MacOS/playsound");
#else
	config_file.addVariable("Sounds", "SoundPlayer", "/usr/bin/play");
#endif
	connect(sound_manager, SIGNAL(playSound(const QString &, bool, double)),
			this, SLOT(playSound(const QString &, bool, double)));

	kdebugf2();
}

ExternalPlayer::~ExternalPlayer()
{
	kdebugf();

	disconnect(sound_manager, SIGNAL(playSound(const QString &, bool, double)),
			this, SLOT(playSound(const QString &, bool, double)));

	kdebugf2();
}

void ExternalPlayer::play(const QString &s, bool volCntrl, double vol, QString player)
{
	kdebugf();
	QString t;

	if (player.isEmpty())
		t = config_file.readEntry("Sounds", "SoundPlayer");
	else
		t = player;

	if (t.isEmpty())
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: player path is empty\n");
		return;
	}

	if (volCntrl)
		t.append(" -v "+QString::number(vol, 'g', 2));
	t.append(" "+s+" >/dev/null &");
	kdebugm(KDEBUG_INFO, "%s\n", t.ascii());
	system(t.ascii());
	kdebugf2();
}

void ExternalPlayer::playSound(const QString &s, bool volCntrl, double vol)
{
	kdebugf();
	QString player = QString::null;
	play(s, volCntrl, vol, player);
}

ExternalPlayer *external_player;

/** @} */

