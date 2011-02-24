/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
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

#ifndef PHONON_SOUND_H
#define PHONON_SOUND_H

#include <QtCore/QObject>
#include <QtCore/QString>

#include "modules/sound/sound-player.h"

namespace Phonon
{
	class AudioOutput;
	class MediaObject;
}

class PhononPlayer : public SoundPlayer
{
	Q_OBJECT
	Q_DISABLE_COPY(PhononPlayer);

	static PhononPlayer * Instance;

	Phonon::MediaObject *Media;
	Phonon::AudioOutput *Output;

	PhononPlayer();
	virtual ~PhononPlayer();

public:
	static void createInstance();
	static void destroyInstance();
	static PhononPlayer * instance();

	virtual void playSound(const QString &path, bool volumeControl, double volume);

};

#endif // PHONON_SOUND_H
