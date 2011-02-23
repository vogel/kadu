/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2005, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2007, 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
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

#include "configuration/configuration-file.h"
#include "debug.h"

#include "modules/sound/sound-file.h"

#include "alsa-device.h"

#include "alsa-player.h"

AlsaPlayer * AlsaPlayer::Instance = 0;

void AlsaPlayer::createInstance()
{
	if (!Instance)
		Instance = new AlsaPlayer();
}

void AlsaPlayer::destroyInstance()
{
	if (Instance)
	{
		delete Instance;
		Instance = 0;
	}
}

AlsaPlayer * AlsaPlayer::instance()
{
	return Instance;
}

AlsaPlayer::AlsaPlayer()
{
	createDefaultConfiguration();
}

AlsaPlayer::~AlsaPlayer()
{
}

void AlsaPlayer::createDefaultConfiguration()
{
	config_file.addVariable("Sounds", "ALSAOutputDevice", "default");
}

void AlsaPlayer::playSound(const QString &path, bool volumeControl, double volume)
{
	SoundFile sound(qPrintable(path));

	if (!sound.valid())
	{
		kdebugm(KDEBUG_INFO, "broken sound file?\n");
		return;
	}

	if (volumeControl)
		sound.setVolume(volume);

	AlsaDevice device(config_file.readEntry("Sounds", "ALSAOutputDevice"), sound.sampleRate(), sound.channels());
	device.open();
	device.playSample(sound.data(), sound.length());
	device.close();
}
