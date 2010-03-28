/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2004, 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <QtGui/QFileDialog>

#include "configuration/configuration-file.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/path-conversion.h"
#include "debug.h"

#include "modules/sound/sound-manager.h"

#include "player_external.h"

/**
 * @ingroup ext_sound
 * @{
 */
extern "C" KADU_EXPORT int ext_sound_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	kdebugf();

	external_player = new ExternalPlayer();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/ext_sound.ui"));

	kdebugf2();
	return 0;
}
extern "C" KADU_EXPORT void ext_sound_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/ext_sound.ui"));
	delete external_player;
	external_player = 0;

	kdebugf2();
}

ExternalPlayer::ExternalPlayer()
{
	kdebugf();

	createDefaultConfiguration();

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
	kdebugm(KDEBUG_INFO, "%s\n", t.toAscii().data());
	system(t.toAscii());
	kdebugf2();
}

void ExternalPlayer::playSound(const QString &s, bool volCntrl, double vol)
{
	kdebugf();
	QString player = QString::null;
	play(s, volCntrl, vol, player);
}

void ExternalPlayer::createDefaultConfiguration()
{
#ifdef Q_OS_MACX
	config_file.addVariable("Sounds", "SoundPlayer", "/Applications/Kadu.app/Contents/MacOS/playsound");
#else
	config_file.addVariable("Sounds", "SoundPlayer", "/usr/bin/play");
#endif
}

ExternalPlayer *external_player;

/** @} */

