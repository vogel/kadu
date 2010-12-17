/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include "modules.h"
#include "configuration/configuration-file.h"
#include "misc/path-conversion.h"
#include "debug.h"
#include "mpris.h"

MprisPlayer* mpris;

extern "C" int mpris_mediaplayer_init()
{
	mpris = new MprisPlayer("MPRIS", QString());

	MainConfigurationWindow::registerUiFile(
		dataPath("kadu/modules/configuration/mpris_mediaplayer.ui"));

	bool res = mediaplayer->registerMediaPlayer(mpris, mpris);
	return res ? 0 : 1;
}

extern "C" void mpris_mediaplayer_close()
{
	MainConfigurationWindow::unregisterUiFile(
		dataPath("kadu/modules/configuration/mpris_mediaplayer.ui"));

	mediaplayer->unregisterMediaPlayer();

	delete mpris;
	mpris = NULL;
}

MprisPlayer::MprisPlayer(QString name, QString service) : MPRISMediaPlayer(name, service)
{
	setService(config_file.readEntry("MediaPlayer", "MPRISService", QString()));
}

MprisPlayer::~MprisPlayer()
{
}

void MprisPlayer::configurationUpdated()
{
	setService(config_file.readEntry("MediaPlayer", "MPRISService", QString()));
}
