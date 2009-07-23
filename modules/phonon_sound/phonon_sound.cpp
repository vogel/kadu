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

#include "phonon_sound.h"

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

PhononPlayer::PhononPlayer()
{
	kdebugf();

	music  = new Phonon::MediaObject(this);
	output = new Phonon::AudioOutput(Phonon::NotificationCategory, this);
	Phonon::createPath(music, output);

	connect(sound_manager, SIGNAL(playSound(const QString &, bool, double)),
			this, SLOT(playSound(const QString &, bool, double)));

	kdebugf2();
}

PhononPlayer::~PhononPlayer()
{
	kdebugf();

	delete music;
	delete output;

	disconnect(sound_manager, SIGNAL(playSound(const QString &, bool, double)),
			this, SLOT(playSound(const QString &, bool, double)));

	kdebugf2();
}

void PhononPlayer::playSound(const QString &s, bool volCntrl, double vol)
{
	kdebugf();

	if (volCntrl)
		output->setVolume(vol);

	music->setCurrentSource(Phonon::MediaSource(s));
	music->play();

	kdebugf2();
}

PhononPlayer *phonon_player;

/** @} */

