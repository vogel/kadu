/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QThread>
#include <QtGui/QApplication>

#include "debug.h"

#include "sound-player.h"

#include "sound-play-thread.h"

SoundPlayThread::SoundPlayThread() :
		End(false), CurrentlyNotWaiting(false), Play(false)
{
}

SoundPlayThread::~SoundPlayThread()
{
}

void SoundPlayThread::start()
{
	kdebugf();

	// Solution copied from QWaitCondition docs and adjusted.
	NewSoundToPlay.lock();
	while (!End)
	{
		WaitForNewSoundToPlay.wait(&NewSoundToPlay);
		CurrentlyNotWaiting = true;
		NewSoundToPlay.unlock();

		if (!End && Play)
		{
			if (Player)
			{
				PlayingMutex.lock();
				Player->playSound(Path);
				PlayingMutex.unlock();
			}

			Play = false;
		}

		NewSoundToPlay.lock();
		CurrentlyNotWaiting = false;
	}
	NewSoundToPlay.unlock();

	emit finished();

	deleteLater();

	kdebugf2();
}

void SoundPlayThread::end()
{
	// Solution copied from QWaitCondition docs and adjusted.

	End = true;

	NewSoundToPlay.lock();
	while (CurrentlyNotWaiting)
	{
		NewSoundToPlay.unlock();
// 		QThread::msleep(200);
		Q_ASSERT(QThread::currentThread() != thread());
		thread()->wait(200);
		NewSoundToPlay.lock();
	}
	WaitForNewSoundToPlay.wakeAll();
	NewSoundToPlay.unlock();
}

void SoundPlayThread::play(SoundPlayer *player, const QString &path)
{
	if (!PlayingMutex.tryLock())
		return; // one sound is played, we ignore next one

	Player = player;
	Path = path;
	Play = true;

	PlayingMutex.unlock();
	WaitForNewSoundToPlay.wakeAll();
}

#include "moc_sound-play-thread.cpp"
