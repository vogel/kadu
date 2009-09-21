/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "debug.h"

#include "sound-event.h"

#include "sample-record-thread.h"

SampleRecordThread::SampleRecordThread(SoundDevice device) :
		Device(device), Sample(0), SampleLen(0), Stopped(false),
		RecordingSemaphore(1), SampleSemaphore(1)
{
	kdebugf();
	setTerminationEnabled(true);
	RecordingSemaphore.acquire();
	kdebugf2();
}

SampleRecordThread::~SampleRecordThread()
{
}

void SampleRecordThread::run()
{
	kdebugf();

	while (!Stopped)
	{
		RecordingSemaphore.acquire();

		if (Stopped)
		{
			SampleSemaphore.release();
			break;
		}

		sound_manager->recordSampleTMP(Device, Sample, SampleLen);
		QCoreApplication::postEvent(this, new SoundEvent(Device));
		SampleSemaphore.release();
	}

	kdebugf2();
}

bool SampleRecordThread::event(QEvent *event)
{
	if ((int)event->type() == SoundEvent::eventNumber())
		emit sampleRecorded((((SoundEvent *)event)->data()));
	return true;
}

void SampleRecordThread::recordSample(qint16 *data, int length)
{
	kdebugf();
	SampleSemaphore.acquire();
	Sample = data;
	SampleLen = length;
	RecordingSemaphore.release();
	kdebugf2();
}

void SampleRecordThread::stop()
{
	kdebugf();
	SampleSemaphore.acquire();
	Stopped = true;
	RecordingSemaphore.release();

	if (!wait(5000))
	{
		kdebugm(KDEBUG_ERROR, "deadlock :|, terminating SampleRecordThread\n");
		this->terminate();
		wait(1000);
	}

	kdebugf2();
}
