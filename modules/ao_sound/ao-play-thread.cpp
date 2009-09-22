/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <ao/ao.h>

#include "debug.h"

#include "../sound/sound-file.h"
#include "../sound/sound-params.h"

#include "ao-play-thread.h"

AOPlayThread::AOPlayThread()
{
	semaphore = new QSemaphore(100);
}

AOPlayThread::~AOPlayThread()
{
	delete semaphore;
}

void AOPlayThread::run()
{
	kdebugf();
	bool checkAgain = true;
	end = false;
	while (!end)
	{
		semaphore->acquire();
		mutex.lock();
		kdebugm(KDEBUG_INFO, "locked\n");
		if (end)
		{
			mutex.unlock();
			break;
		}

		if (!list.isEmpty())
		{
			SoundParams p = list.takeFirst();
			play(p.fileName().toLocal8Bit().data(), checkAgain, p.volumeControl(), p.volume());
		}
		mutex.unlock();
		kdebugm(KDEBUG_INFO, "unlocked\n");
	}//end while(!end)
	kdebugf2();
}


bool AOPlayThread::play(const char *path, bool &checkAgain, bool volumeControl, float volume)
{
	kdebugf();
	SoundFile *sound = new SoundFile(path);

	if (!sound->isOk())
	{
		kdebugmf(KDEBUG_ERROR, "sound is not ok?\n");
		delete sound;
		kdebugf2();
		return false;
	}
	kdebugm(KDEBUG_INFO, "\n");
	kdebugm(KDEBUG_INFO, "length:   %d\n", sound->length);
	kdebugm(KDEBUG_INFO, "speed:    %d\n", sound->speed);
	kdebugm(KDEBUG_INFO, "channels: %d\n", sound->channels);

	if (volumeControl)
		sound->setVolume(volume);

	int driver_id = ao_default_driver_id();

	checkAgain = false;
	if (-1 == driver_id)
	{
		checkAgain = true;
		kdebugmf(KDEBUG_WARNING, "cannot get default driver id!\n");
		delete sound;
		kdebugf2();
		return false;
	}

	ao_sample_format format;
	format.bits = 16;
	format.channels = sound->channels;
	format.rate = sound->speed;
	format.byte_format = AO_FMT_LITTLE;

	ao_device *device = ao_open_live(driver_id, &format, NULL);
	if (!device)
	{
		checkAgain = true;
		kdebugmf(KDEBUG_WARNING, "cannot open device!\n");
		delete sound;
		kdebugf2();
		return false;
	}

	int ret = ao_play(device, (char *)sound->data, sound->length * sizeof(short));
	if (0 == ret)
	{
		checkAgain = true;
		kdebugmf(KDEBUG_WARNING, "ao_play() == 0\n");
		ao_close(device);
		delete sound;
		kdebugf2();
		return false;
	}

	ao_close(device);
	delete sound;
	kdebugf2();
	return true;
}
