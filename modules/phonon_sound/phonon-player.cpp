/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
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

#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>
#include <phonon/phononnamespace.h>

#include "modules/sound/sound-manager.h"

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

PhononPlayer::PhononPlayer()
{
	kdebugf();

	Media = new Phonon::MediaObject(this);
	Output = new Phonon::AudioOutput(Phonon::NotificationCategory, this);
	Phonon::createPath(Media, Output);

	kdebugf2();
}

PhononPlayer::~PhononPlayer()
{
}

void PhononPlayer::playSound(const QString &path, bool volumeControl, double volumes)
{
	kdebugf();

	if (volumeControl)
		Output->setVolume(volumes);

	Media->setCurrentSource(Phonon::MediaSource(path));
	Media->play();

	kdebugf2();
}
