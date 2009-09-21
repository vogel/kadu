/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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

	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void phonon_sound_close()
{
	kdebugf();

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
