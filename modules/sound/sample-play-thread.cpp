/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Longer (longer89@gmail.com)
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

#include "debug.h"

#include "sound-event.h"

#include "sample-play-thread.h"

SamplePlayThread::SamplePlayThread(SoundDevice device, QObject *parent) :
		QThread(parent), Device(device), Sample(0), SampleLen(0), Stopped(false),
		PlayingSemaphore(1), SampleSemaphore(1)
{
	kdebugf();
	PlayingSemaphore.acquire();
	kdebugf2();
}

SamplePlayThread::~SamplePlayThread()
{
}

void SamplePlayThread::run()
{
	kdebugf();

	while (!Stopped)
	{
		PlayingSemaphore.acquire();

		if (Stopped)
		{
			SampleSemaphore.release();
			break;
		}

		emit playSample(Device, Sample, SampleLen);
		QCoreApplication::postEvent(this, new SoundEvent(Device));
		SampleSemaphore.release();
	}

	kdebugf2();
}

bool SamplePlayThread::event(QEvent *event)
{
	if ((int)event->type() == SoundEvent::eventNumber())
		emit samplePlayed(((SoundEvent *)event)->data());
	return true;
}

void SamplePlayThread::playSample(const qint16 *data, int length)
{
	kdebugf();
	
	SampleSemaphore.acquire();
	Sample = data;
	SampleLen = length;
	PlayingSemaphore.release();

	kdebugf2();
}

void SamplePlayThread::stop()
{
	kdebugf();
	
	SampleSemaphore.acquire();
	Stopped = true;
	PlayingSemaphore.release();

	if (!wait(5000))
	{
		kdebugm(KDEBUG_ERROR, "deadlock :|, terminating SamplePlayThread\n");
		this->terminate();
		wait(1000);
	}

	kdebugf2();
}
