/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>
#include <phonon/phononnamespace.h>

#include "modules/sound/sound-player.h"

/**
 * @defgroup phonon_sound Phonon sound
 * @{
 */
class PhononPlayer : public SoundPlayer
{
	Q_OBJECT

	Phonon::MediaObject *music;
	Phonon::AudioOutput *output;

public:
	PhononPlayer(QObject *parent = 0);
	virtual ~PhononPlayer();

	virtual bool isSimplePlayer() { return true; }

public slots:
	virtual void playSound(const QString &path, bool volumeControl, double volume);

	virtual SoundDevice openDevice(SoundDeviceType type, int sampleRate, int channels)
	{
		Q_UNUSED(type)
		Q_UNUSED(sampleRate)
		Q_UNUSED(channels)
		return 0;
	}

	virtual void closeDevice(SoundDevice device)
	{
		Q_UNUSED(device)
	}

	virtual bool playSample(SoundDevice device, const qint16 *data, int length)
	{
		Q_UNUSED(device)
		Q_UNUSED(data)
		Q_UNUSED(length)
		return false;
	}

	virtual bool recordSample(SoundDevice device, qint16 *data, int length)
	{
		Q_UNUSED(device)
		Q_UNUSED(data)
		Q_UNUSED(length)
		return false;
	}

	virtual void setFlushingEnabled(SoundDevice device, bool enabled)
	{
		Q_UNUSED(device)
		Q_UNUSED(enabled)
	}

};

extern PhononPlayer *phonon_player;

/** @} */

#endif // PHONON_SOUND_H
