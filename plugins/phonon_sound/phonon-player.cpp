/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <phonon/audiooutput.h>
#include <phonon/mediaobject.h>
#include <phonon/phononnamespace.h>

#include "plugins/sound/sound-manager.h"
#include "plugins/sound/sound-play-thread.h"

#include "debug.h"

#include "phonon-player.h"

PhononPlayer * PhononPlayer::Instance = 0;

void PhononPlayer::createInstance()
{
	if (!Instance)
		Instance = new PhononPlayer();
}

void PhononPlayer::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

PhononPlayer * PhononPlayer::instance()
{
	return Instance;
}

PhononPlayer::PhononPlayer() :
    Media(0)
{
	kdebugf();

	// Phonon produces Qt warnings when is run not in QApplication's thread.
	// It is a workaround.
	int type = QMetaType::type("MediaSource");
	if (type == 0 || !QMetaType::isRegistered(type))
		qRegisterMetaType<Phonon::MediaSource>("MediaSource");

	// Queued connection, bacause this signal will be emitted from different thread
	connect(this, SIGNAL(createRequest()), this, SLOT(createMediaObject()), Qt::QueuedConnection);

	kdebugf2();
}

PhononPlayer::~PhononPlayer()
{
	delete Media;
}

void PhononPlayer::createMediaObject()
{
	MediaObjectMutex.lock();

	// this methos is always called from main thread
	Media = Phonon::createPlayer(Phonon::NotificationCategory);

	MediaObjectCreation.wakeAll();
	MediaObjectMutex.unlock();

}

void PhononPlayer::playSound(const QString &path)
{
	kdebugf();

	if (!Media)
	{
		MediaObjectMutex.lock();

		// Double check of !Media is required. We are not locking whole playSound method but only when
		// media object doesn't exists. In theory it is possible that two thread will be checking this at the same time,
		// so the second check prevents possible race condition.
		// Probably it will be never a problem in Kadu, as single thread is accessing this method, but this code is technically
		// more correct
		if (!Media)
		{
			emit createRequest();

			MediaObjectCreation.wait(&MediaObjectMutex);
		}
		MediaObjectMutex.unlock();
	}

	Media->setCurrentSource(path);
	Media->play();

	kdebugf2();
}
