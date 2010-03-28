/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <ao/ao.h>

#include "debug.h"

#include "modules/sound/sound-manager.h"
#include "modules/sound/sound-params.h"

#include "ao-play-thread.h"

#include "ao-sound.h"


extern "C" int ao_sound_init()
{
	kdebugf();

	ao_player = new AOPlayer;
	if (!ao_player->isOk())
	{
		delete ao_player;
		return -1;
	}
	SoundManager::instance()->setPlayer(ao_player);

	kdebugf2();
	return 0;
}

extern "C" void ao_sound_close()
{
	kdebugf();

	SoundManager::instance()->setPlayer(0);

	delete ao_player;
	ao_player = 0;
	kdebugf2();
}

bool AOPlayer::isOk()
{
	return (thread);
}

AOPlayer::AOPlayer(QObject *parent) : SoundPlayer(parent)
{
	kdebugf();
	ao_initialize();
	
	thread = new AOPlayThread();
	if (!thread)
		return;
	thread->start();
	
	kdebugf2();
}

AOPlayer::~AOPlayer()
{
	kdebugf();

	if (thread)
	{
		thread->mutex.lock();
		thread->end = true;
		thread->mutex.unlock();
		thread->semaphore->release();
		thread->wait();
		delete thread;
		thread = 0;
	}

	ao_shutdown();
	kdebugf2();
}

void AOPlayer::playSound(const QString &path, bool volCntrl, double vol)
{
	kdebugf();

	if (thread->mutex.tryLock())
	{
		thread->list << SoundParams(path, volCntrl, vol);
		thread->mutex.unlock();
		thread->semaphore->release();
	}

	kdebugf2();
}

AOPlayer *ao_player;
