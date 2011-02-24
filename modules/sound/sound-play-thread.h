/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef SOUND_PLAY_THREAD_H
#define SOUND_PLAY_THREAD_H

#include <QtCore/QMutex>
#include <QtCore/QThread>
#include <QtCore/QWaitCondition>

class SoundPlayer;

class SoundPlayThread : public QThread
{
	Q_OBJECT

	bool End;
	QMutex PlayingMutex;
	QWaitCondition NewSoundToPlay;

	bool Play;

	SoundPlayer *Player;
	QString Path;
	bool VolumeControl;
	float Volume;

private slots:
	void playerDestroyed();

protected:
	virtual void run();

public:
	explicit SoundPlayThread(QObject *parent = 0);
	virtual ~SoundPlayThread();

	void end();

	void play(SoundPlayer *player, const QString &path, bool volumeControl = false, float volume = 1.0);

};

#endif // SOUND_PLAY_THREAD_H
