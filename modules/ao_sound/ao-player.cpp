/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include <ao/ao.h>

#include "modules/sound/sound-file.h"

#include "debug.h"

#include "ao-player.h"

AOPlayer * AOPlayer::Instance = 0;

void AOPlayer::createInstance()
{
	if (!Instance)
		Instance = new AOPlayer();
}

void AOPlayer::destroyInstance()
{
	if (Instance)
	{
		delete Instance;
		Instance = 0;
	}
}

AOPlayer * AOPlayer::instance()
{
	return Instance;
}

AOPlayer::AOPlayer()
{
	ao_initialize();
}

AOPlayer::~AOPlayer()
{
	ao_shutdown();
}

void AOPlayer::playSound(const QString &path, bool volumeControl, double volume)
{
	SoundFile sound(qPrintable(path));

	if (!sound.valid())
	{
		kdebugm(KDEBUG_INFO, "broken sound file?\n");
		return;
	}

	if (volumeControl)
		sound.setVolume(volume);

	int driver_id = ao_default_driver_id();

	if (-1 == driver_id)
	{
		kdebugf2();
		return;
	}

	ao_sample_format format;
	format.bits = 16;
	format.channels = sound.channels();
	format.rate = sound.sampleRate();
	format.byte_format = AO_FMT_LITTLE;

	ao_device *device = ao_open_live(driver_id, &format, NULL);
	if (!device)
	{
		kdebugmf(KDEBUG_WARNING, "cannot open device!\n");
		kdebugf2();
		return;
	}

	int ret = ao_play(device, (char *)sound.data(), sound.length() * sizeof(short));
	if (0 == ret)
	{
		kdebugmf(KDEBUG_WARNING, "ao_play() == 0\n");
		ao_close(device);
		kdebugf2();
		return;
	}

	ao_close(device);

	kdebugf2();
}
