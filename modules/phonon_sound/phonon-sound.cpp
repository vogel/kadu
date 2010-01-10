/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
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

#include "../sound/sound.h"

#include "debug.h"

#include "phonon-sound.h"

/**
 * @ingroup phonon_sound
 * @{
 */
extern "C" KADU_EXPORT int phonon_sound_init(bool firstLoad)
{
	kdebugf();

	phonon_player = new PhononPlayer();
	sound_manager->setPlayer(phonon_player);

	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void phonon_sound_close()
{
	kdebugf();

	sound_manager->setPlayer(0);
	delete phonon_player;
	phonon_player = 0;

	kdebugf2();
}

PhononPlayer::PhononPlayer(QObject *parent) :
		SoundPlayer(parent)
{
	kdebugf();

	music  = new Phonon::MediaObject(this);
	output = new Phonon::AudioOutput(Phonon::NotificationCategory, this);
	Phonon::createPath(music, output);

	kdebugf2();
}

PhononPlayer::~PhononPlayer()
{
}

void PhononPlayer::playSound(const QString &path, bool volumeControl, double volumes)
{
	kdebugf();

	if (volumeControl)
		output->setVolume(volumes);

	music->setCurrentSource(Phonon::MediaSource(path));
	music->play();

	kdebugf2();
}

PhononPlayer *phonon_player;

/** @} */
