/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef KADU_AO_SOUND_H
#define KADU_AO_SOUND_H

#include "modules/sound/sound-player.h"

class AOPlayThread;

class AOPlayer : public SoundPlayer
{
	Q_OBJECT

	AOPlayThread *thread;

public:
	AOPlayer(QObject *parent = 0);
	~AOPlayer();
	bool isOk();

	virtual bool isSimplePlayer() { return true; }

public slots:
	virtual void playSound(const QString &path, bool volumeControl, double volume);

};

extern AOPlayer *ao_player;

#endif

