/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KADU_AO_PLAY_THREAD_H
#define KADU_AO_PLAY_THREAD_H

#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QSemaphore>
#include <QtCore/QList>

class SoundParams;

class AOPlayThread : public QThread
{
	static bool play(const char *path, bool &checkAgain, bool volCntrl = false, float vol = 1);

public:
	QMutex mutex;
	QSemaphore *semaphore; //sluzy do powiadamiania o dzwieku w kolejce
	bool end;
	QList<SoundParams> list;
		
	AOPlayThread();
	~AOPlayThread();
	void run();

};

#endif //AO_PLAY_THREAD
