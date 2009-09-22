/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//dla starego ao
#include <stdio.h>

#include <ao/ao.h>
#include "ao_sound.h"
#include "debug.h"
#include "../sound/sound.h"
#include "../sound/sound-file.h"

/*
  pier... arts wyk³ada ca³e kadu na ao_default_driver_id, 
  wiêc napisa³em w³asn± wersjê tej funkcji pomijaj±c± artsa
*/
int my_ao_default_driver_id(bool forceCheck = false)
{
	kdebugf();
	static int lastSelected = -1;
	if (!forceCheck && lastSelected != -1)
	{
		kdebugf2();
		return lastSelected;
	}

	int driver_count, selected, nulldriver;
	nulldriver = ao_driver_id("null");
	selected = nulldriver;
	if (selected == -1)
		return -1;
	ao_sample_format format;
	format.bits = 16;
	format.channels = 2;
	format.rate = 44100;
	format.byte_format = AO_FMT_LITTLE;

	ao_info **infos = ao_driver_info_list(&driver_count);
	for (int i = 0; i < driver_count; i++)
		if (infos[i]->priority >= infos[selected]->priority && infos[i]->type == AO_TYPE_LIVE && strcmp("arts", infos[i]->short_name) != 0)
		{
			ao_device *dev = ao_open_live(ao_driver_id(infos[i]->short_name), &format, NULL);
			if (dev != NULL)
				selected = i;
			ao_close(dev);
		}
	kdebugm(KDEBUG_INFO, "selected driver:%d\n", selected);
	kdebugm(KDEBUG_INFO, "'%s' '%s' '%s' %d\n", infos[selected]->name, infos[selected]->short_name, infos[selected]->comment, infos[selected]->priority);
	if (nulldriver == selected)
		selected = -1;
	lastSelected = selected;
	kdebugf2();
	return selected;
}

extern "C" int ao_sound_init()
{
	kdebugf();

	ao_player_slots = new AOPlayerSlots;
	if (!ao_player_slots->isOk())
	{
		delete ao_player_slots;
		return -1;
	}

	kdebugf2();
	return 0;
}

extern "C" void ao_sound_close()
{
	kdebugf();
	delete ao_player_slots;
	ao_player_slots = NULL;
	kdebugf2();
}

bool AOPlayerSlots::isOk()
{
	return (thread != NULL);
}

bool AOPlayThread::play(const char *path, bool &checkAgain, bool volumeControl, float volume)
{
	kdebugf();
	SoundFile *sound=new SoundFile(path);
	
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

//	int driver_id=ao_default_driver_id();
	int driver_id = my_ao_default_driver_id(checkAgain);
	checkAgain = false;
	if (driver_id == -1)
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

	ao_device *device=ao_open_live(driver_id, &format, NULL);
	if (device == NULL)
	{
		checkAgain = true;
		kdebugmf(KDEBUG_WARNING, "cannot open device!\n");
		delete sound;
		kdebugf2();
		return false;
	}
	
	int ret = ao_play(device, (char *)sound->data, sound->length*sizeof(short));
	if (ret == 0)
	{
		checkAgain = true;
		kdebugmf(KDEBUG_WARNING, "ao_play()==0\n");
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

AOPlayerSlots::AOPlayerSlots(QObject *parent) : QObject(parent)
{
	kdebugf();
	ao_initialize();
	
	thread = new AOPlayThread();
	if (!thread)
		return;
	thread->start();
	
	connect(sound_manager, SIGNAL(playSound(const QString &, bool, double)),
			this, SLOT(playSound(const QString &, bool, double)));

	kdebugf2();
}

AOPlayerSlots::~AOPlayerSlots()
{
	kdebugf();

	disconnect(sound_manager, SIGNAL(playSound(const QString &, bool, double)),
			this, SLOT(playSound(const QString &, bool, double)));

	if (thread)
	{
		thread->mutex.lock();
		thread->end = true;
		thread->mutex.unlock();
		thread->semaphore->release();
		thread->wait();
		delete thread;
		thread = NULL;
	}

	ao_shutdown();
	kdebugf2();
}

void AOPlayerSlots::playSound(const QString &path, bool volCntrl, double vol)
{
	kdebugf();

	if (thread->mutex.tryLock())
	{
		thread->list << SndParams2(path, volCntrl, vol);
		thread->mutex.unlock();
		thread->semaphore->release();
	}

	kdebugf2();
}

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
			SndParams2 p = list.takeFirst();
			play(p.filename.toLocal8Bit().data(), checkAgain, p.volumeControl, p.volume);
		}
		mutex.unlock();
		kdebugm(KDEBUG_INFO, "unlocked\n");
	}//end while(!end)
	kdebugf2();
}

AOPlayerSlots *ao_player_slots;
