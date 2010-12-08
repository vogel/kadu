/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include "debug.h"

#include "sound-player.h"

#include "sound-play-thread.h"

SoundPlayThread::SoundPlayThread(QObject *parent) :
		QThread(parent), End(false), Play(false), Player(0)
{
	setTerminationEnabled(true);
}

SoundPlayThread::~SoundPlayThread()
{
}

void SoundPlayThread::run()
{
	kdebugf();

	NewSoundMutex.lock();

	while (!End)
	{
		NewSoundMutex.lock();

		if (End)
		{
			NewSoundMutex.unlock();
			break;
		}

		if (!Play)
		{
			NewSoundMutex.unlock();
			continue;
		}

		if (Player)
		{
			PlayingMutex.lock();
			Player->playSound(Path, VolumeControl, Volume);
			PlayingMutex.unlock();
		}

		Play = false;
	}

	kdebugf2();
}

void SoundPlayThread::end()
{
	NewSoundMutex.unlock();
	End = true;
}

void SoundPlayThread::play(SoundPlayer *player, const QString &path, bool volumeControl, float volume)
{
	if (!PlayingMutex.tryLock())
		return; // one sound is played, we ignore next one

	if (Player)
		disconnect(Player, SIGNAL(destroyed()), this, SLOT(playerDestroyed()));

	Player = player;

	if (Player)
		connect(Player, SIGNAL(destroyed()), this, SLOT(playerDestroyed()));

	Path = path;
	VolumeControl = volumeControl;
	Volume = volume;

	Play = true;

	PlayingMutex.unlock();
	NewSoundMutex.unlock();
}

void SoundPlayThread::playerDestroyed()
{
	Player = 0;
}
