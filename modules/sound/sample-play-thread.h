/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SAMPLE_PLAY_THREAD_H
#define SAMPLE_PLAY_THREAD_H

#include <QtCore/QThread>

#include "sound.h"

class SamplePlayThread : public QThread
{
	Q_OBJECT

	SoundDevice Device;
	const qint16 *Sample;
	int SampleLen;
	bool Stopped;
	QSemaphore PlayingSemaphore;
	QSemaphore SampleSemaphore;

protected:
	virtual void run();
	virtual bool event(QEvent *event);

public:
	SamplePlayThread(SoundDevice device, QObject *parent = 0);
	virtual ~SamplePlayThread();

	void playSample(const qint16 *data, int length);
	void stop();

signals:
	void samplePlayed(SoundDevice device);
	void playSampleTMP(SoundDevice device, const qint16 *data, int length);
};

#endif // SAMPLE_PLAY_THREAD_H
