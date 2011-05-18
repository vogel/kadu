/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2010 Tomasz Rostański (rozteck@interia.pl)
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

#ifndef QT4_SOUND_H
#define QT4_SOUND_H

#include "plugins/sound/sound-player.h"

class QtSound4Player : public SoundPlayer
{
	Q_OBJECT
	Q_DISABLE_COPY(QtSound4Player);

	static QtSound4Player *Instance;

	QtSound4Player();
	virtual ~QtSound4Player();

public:
	static void createInstance();
	static void destroyInstance();
	static QtSound4Player *instance();

	virtual void playSound(const QString &path);

};

#endif // QT4_SOUND_H
