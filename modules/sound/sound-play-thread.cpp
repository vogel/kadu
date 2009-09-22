/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "debug.h"

#include "sound.h"
#include "sound-file.h"
#include "sound-params.h"

#include "sound-play-thread.h"

SoundPlayThread::SoundPlayThread() :
		QThread(), Mutex(), Semaphore(100), End(false)
{
	setTerminationEnabled(true);
	Semaphore.acquire(100);
}

SoundPlayThread::~SoundPlayThread()
{
}

void SoundPlayThread::tryPlay(const char *path, bool volumeControl, float volume)
{
	kdebugf();

	if (Mutex.tryLock())
	{
		List.push_back(SoundParams(path, volumeControl, volume));
		Mutex.unlock();
		Semaphore.release();
	}

	kdebugf2();
}

void SoundPlayThread::run()
{
	kdebugf();

	while (!End)
	{
		Semaphore.acquire();
		Mutex.lock();
		kdebugmf(KDEBUG_INFO, "locked\n");

		if (End)
		{
			Mutex.unlock();
			break;
		}

		SoundParams params = List.first();
		List.pop_front();
		
		play(qPrintable(params.fileName()), params.volumeControl(), params.volume());
		Mutex.unlock();
		kdebugmf(KDEBUG_INFO, "unlocked\n");
	}

	kdebugf2();
}

bool SoundPlayThread::play(const char *path, bool volumeControl, float volume)
{
	bool ret = false;
	SoundFile *sound = new SoundFile(path);
	
	if (!sound->isOk())
	{
		kdebugm(KDEBUG_INFO, "broken sound file?\n");
		delete sound;
		return false;
	}
	
	kdebugm(KDEBUG_INFO, "\n");
	kdebugm(KDEBUG_INFO, "length:   %d\n", sound->length);
	kdebugm(KDEBUG_INFO, "speed:    %d\n", sound->speed);
	kdebugm(KDEBUG_INFO, "channels: %d\n", sound->channels);
	
	if (volumeControl)
		sound->setVolume(volume);

	SoundDevice dev;
	dev = sound_manager->openDevice(SoundDevicePlayOnly, sound->speed, sound->channels);
	sound_manager->setFlushingEnabled(dev, true);
	ret = sound_manager->playSample(dev, sound->data, sound->length*sizeof(sound->data[0]));
	sound_manager->closeDevice(dev);

	delete sound;
	return ret;
}

void SoundPlayThread::endThread()
{
	Mutex.lock();
	End = true;
	Mutex.unlock();
	Semaphore.release();
}
