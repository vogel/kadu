/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SAMPLE_RECORD_THREAD
#define SAMPLE_RECORD_THREAD

#include <QtCore/QThread>

#include "sound.h"

class SampleRecordThread : public QThread
{
	Q_OBJECT

	SoundDevice Device;
	qint16* Sample;
	int SampleLen;
	bool Stopped;
	QSemaphore RecordingSemaphore;
	QSemaphore SampleSemaphore;

protected:
	virtual void run();
	virtual bool event(QEvent *event);

public:
	SampleRecordThread(SoundDevice device);
	virtual ~SampleRecordThread();

	void recordSample(qint16 *data, int length);
	void stop();

signals:
	void sampleRecorded(SoundDevice device);

};

#endif // SAMPLE_RECORD_THREAD
