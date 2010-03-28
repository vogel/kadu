/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2005, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2007, 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
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

#include <sys/time.h>

#include "alsa-player.h"
#include "configuration/configuration-file.h"
#include "debug.h"
#include "../sound/sound.h"
#include "../sound/sound-file.h"

#include "alsa-device.h"

/**
 * @ingroup alsa_sound
 * @{
 */

ALSAPlayerSlots::ALSAPlayerSlots(QObject *parent) : SoundPlayer(parent)
{
	kdebugf();

	createDefaultConfiguration();

	kdebugf2();
}

ALSAPlayerSlots::~ALSAPlayerSlots()
{
	kdebugf();

	kdebugf2();
}

void ALSAPlayerSlots::createDefaultConfiguration()
{
	config_file.addVariable("Sounds", "ALSAOutputDevice", "default");
}

void ALSAPlayerSlots::playSound(const QString &path, bool volumeControl, double volume)
{
	printf("play sound from alsa...\n");

	SoundFile sound(qPrintable(path));

	if (!sound.isOk())
	{
		kdebugm(KDEBUG_INFO, "broken sound file?\n");
		return;
	}

	if (volumeControl)
		sound.setVolume(volume);

	AlsaDevice device(config_file.readEntry("Sounds", "ALSAOutputDevice"), sound.speed, sound.channels);
	device.open();
	device.playSample(sound.data, sound.length);
	device.close();
}

/*	if (dev->player)
		snd_pcm_nonblock (dev->player, !enabled);*/

ALSAPlayerSlots *alsa_player_slots;

/** @} */

