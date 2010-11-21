/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2008, 2009 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef SOUND_FILE_H
#define SOUND_FILE_H

#ifndef Q_OS_WIN
	#include <sndfile.h>
#endif

#include <QtCore/QString>

class SoundFile
{
	int Length;
	short int *Data;
	int Channels;
	int SampleRate;

	void loadData(const QString &path);
#ifndef Q_OS_WIN
	void loadFloatSamples(SNDFILE *f);
	void loadIntSamples(SNDFILE *f);
#endif

public:
	static void setVolume(short int *data, int length, float volume);

	SoundFile(const QString &path);
	~SoundFile();

	bool valid();
	void setVolume(float volume);

	int length() const { return Length; }
	short int * data() { return Data; }
	int channels() const { return Channels; }
	int sampleRate() const { return SampleRate; }

};

#endif // SOUND_FILE_H
