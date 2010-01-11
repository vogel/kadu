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
	void playSample(SoundDevice device, const qint16 *data, int length);
};

#endif // SAMPLE_PLAY_THREAD_H
