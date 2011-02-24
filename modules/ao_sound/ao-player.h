/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef AO_PLAYER_H
#define AO_PLAYER_H

#include "modules/sound/sound-player.h"

class AOPlayThread;

class AOPlayer : public SoundPlayer
{
	Q_OBJECT

	static AOPlayer *Instance;

	AOPlayer();
	virtual ~AOPlayer();
public:
	static void createInstance();
	static void destroyInstance();

	static AOPlayer * instance();

	virtual void playSound(const QString &path, bool volumeControl, double volume);

};

#endif // AO_PLAYER_H
