/*
 * %kadu copyright begin%
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2003 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2009 Longer (longer89@gmail.com)
 * Copyright 2008, 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2003, 2004 Paweł Płuciennik (pawel_p@kadu.net)
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

#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <QtCore/QObject>
#include <QtCore/QTime>

#include "sound-exports.h"

class SoundPlayer;
class SoundPlayThread;

class SOUNDAPI SoundManager : public QObject
{
	Q_OBJECT

	static SoundManager *Instance;

	SoundPlayer *Player;

	bool Mute;

	SoundPlayThread *PlayThread;

	SoundManager();
	virtual ~SoundManager();

	void import_0_6_5_configuration();
	void createDefaultConfiguration();

public slots:
	void playFile(const QString &path, bool force = false);
	void playFile(const QString &path, bool volumeControl, double volume);
	void playSoundByName(const QString &soundName);

	void setMute(bool enable);

public:
	static void createInstance();
	static void destroyInstance();
	static SoundManager * instance();

	void setPlayer(SoundPlayer *player);

	bool isMuted() const;

public slots:
	void testSoundPlaying();

};

#endif // SOUND_MANAGER_H
