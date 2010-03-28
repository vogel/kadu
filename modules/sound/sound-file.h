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

#ifndef KADU_SOUNDFILE_H
#define KADU_SOUNDFILE_H

#include <QtGlobal>

/** @ingroup sound
 * @{
 */
class SoundFile
{
	//klasa automagicznie przy pomocy libsndfile konwertuje wszystko na dzwiek 16 bitowy
	public:
	int length;
	short int *data;
	int channels;
	int speed;
	
	SoundFile(const char *path);
	~SoundFile();
	bool isOk();
	void setVolume(float vol);
	static void setVolume(short int *data, int length, float vol);
};
/** @} */
#endif // KADU_SOUNDFILE_H
