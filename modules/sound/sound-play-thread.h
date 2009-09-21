/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SOUND_PLAY_THREAD_H
#define SOUND_PLAY_THREAD_H

#include <QtCore/QMutex>
#include <QtCore/QSemaphore>
#include <QtCore/QThread>

class SoundParams;

class SoundPlayThread : public QThread
{
	Q_OBJECT

	QMutex Mutex;
	QSemaphore Semaphore;
	bool End;
	QList<SoundParams> List;

	static bool play(const char *path, bool volumeControl = false, float volume = 1.0);

public:
	SoundPlayThread();
	virtual ~SoundPlayThread();

	void run();

	void tryPlay(const char *path, bool volumeControl = false, float volume = 1.0);
	void endThread();

};

#endif // SOUND_PLAY_THREAD_H
