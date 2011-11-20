/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include <QtCore/QScopedArrayPointer>

#include "debug.h"

#include "sound-file.h"

#define SAMPLE_MAX 0x7fff
#define SAMPLE_MIN -0x7ffe

void SoundFile::setVolume(qint16 *data, int length, float vol)
{
	short *end=data+length;
	while (data!=end)
	{
		float tmp = vol * *data;
		if (tmp > SAMPLE_MAX)
			tmp = SAMPLE_MAX;
		else if (tmp < SAMPLE_MIN)
			tmp = SAMPLE_MIN;
		*data++=(qint16)tmp;
	}
}

SoundFile::SoundFile(const QString &file) :
		Length(0), Data(0), Channels(0), SampleRate(0)
{
	loadData(file);
}


SoundFile::~SoundFile()
{
	if (Data)
	{
		delete []Data;
		Data = 0;
	}
}

void SoundFile::loadData(const QString &path)
{
#ifdef Q_OS_WIN
	Q_UNUSED(path)
#else
	SF_INFO info;
	memset(&info, 0, sizeof(info));
	SNDFILE *f = sf_open(path.toUtf8().constData(), SFM_READ, &info);
	if (!f)
	{
		fprintf(stderr, "cannot open file '%s'\n", qPrintable(path));
		return;
	}

	kdebugm(KDEBUG_INFO, "frames:\t\t%llu\n", (long long unsigned)info.frames);
	kdebugm(KDEBUG_INFO, "samplerate:\t%d\n", info.samplerate);
	kdebugm(KDEBUG_INFO, "channels:\t%d\n", info.channels);
	kdebugm(KDEBUG_INFO, "format:\t\t0x%x\n", info.format);
	kdebugm(KDEBUG_INFO, "sections:\t%d\n", info.sections);
	kdebugm(KDEBUG_INFO, "seekable:\t%d\n", info.seekable);

	Length = info.frames * info.channels;
	Channels = info.channels;
	SampleRate = info.samplerate;

	Data = new short int[Length];

	long format = info.format & SF_FORMAT_SUBMASK;

	if (format == SF_FORMAT_FLOAT || format == SF_FORMAT_DOUBLE)
		loadFloatSamples(f);
	else
		loadIntSamples(f);

	sf_close(f);
#endif
}

#ifndef Q_OS_WIN
void SoundFile::loadFloatSamples(SNDFILE *f)
{
	QScopedArrayPointer<float> buffer(new float[Length]);
	double scale;

	sf_command(f, SFC_CALC_SIGNAL_MAX, &scale, sizeof(scale)) ;
	if (scale < 1e-10)
		scale = 1.0 ;
	else
		scale = 32700.0 / scale;

	int readcount = sf_read_float(f, buffer.data(), Length);
	for (int m = 0; m < readcount; ++m)
		Data[m] = (short int)(scale * buffer[m]);
}

void SoundFile::loadIntSamples(SNDFILE *f)
{
	sf_read_short(f, Data, Length);
}
#endif

bool SoundFile::valid()
{
	return 0 != Length;
}

void SoundFile::setVolume(float vol)
{
	setVolume(Data, Length, vol);
}
