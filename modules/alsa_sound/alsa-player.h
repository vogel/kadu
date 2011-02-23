/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2005 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2007, 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
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

#ifndef ALSA_PLAYER_H
#define ALSA_PLAYER_H

#define ALSA_PCM_NEW_HW_PARAMS_API
#define ALSA_PCM_NEW_SW_PARAMS_API

#include <alsa/asoundlib.h>

#include "modules/sound/sound-player.h"

class AlsaPlayer : public SoundPlayer
{
	Q_OBJECT

	static AlsaPlayer *Instance;

	void createDefaultConfiguration();

	AlsaPlayer();
	virtual ~AlsaPlayer();

public:
	static void createInstance();
	static void destroyInstance();

	static AlsaPlayer * instance();

	virtual void playSound(const QString &path, bool volumeControl, double volume);

};

#endif // ALSA_PLAYER_H
