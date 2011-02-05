/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtCore/QDir>
#include <QtCore/QTextStream>
#include <QtCore/QStringList>

#include "debug.h"
#include "exports.h"
#include "../mediaplayer/mediaplayer.h"
#include "falf.h"

#define MODULE_FALF_MEDIAPLAYER_VERSION 2.3

FalfMediaPlayer * falf = 0;

extern "C" KADU_EXPORT int falf_mediaplayer_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	falf = new FalfMediaPlayer();
	bool res = mediaplayer->registerMediaPlayer(falf, 0);
	return res ? 0 : 1;
}

extern "C" KADU_EXPORT void falf_mediaplayer_close()
{
	mediaplayer->unregisterMediaPlayer();
	delete falf;
	falf = 0;
}

FalfMediaPlayer::FalfMediaPlayer()
{
	kdebugf();
	
	infoFile.setFileName(QDir::homePath() + "/.FALF/track.info");
}

FalfMediaPlayer::~FalfMediaPlayer()
{
	kdebugf();
}

// Private members
QString FalfMediaPlayer::getData(dataType t)
{
	if (!isActive())
		return "";

	infoFile.open(QIODevice::ReadOnly);
	QTextStream sI(&infoFile);
	sI.setCodec("UTF-8");

	QString buffer;

	switch (t)
	{
		case ANY:
			buffer = sI.readLine();
		break;
		case TITLE:
			buffer = sI.readLine();
			buffer = buffer.mid(7);
		break;
		case ALBUM:
			for (unsigned short int i = 0 ; i < 2 ; i++)
				buffer = sI.readLine();
			buffer = buffer.mid(7);
		break;
		case ARTIST:
			for (unsigned short int i = 0 ; i < 3 ; i++)
				buffer = sI.readLine();
			buffer = buffer.mid(8);
		break;
		case VER:
			for (unsigned short int i = 0 ; i < 5 ; i++)
				buffer = sI.readLine();
			buffer = buffer.mid(12);
		break;
	}

	infoFile.close();

	return buffer.simplified();
}

// PlayerInfo
QString FalfMediaPlayer::getPlayerName()
{
	kdebugf();

	return "FALF";
}

QString FalfMediaPlayer::getPlayerVersion()
{
	kdebugf();

	return getData(VER);
}

QStringList FalfMediaPlayer::getPlayListTitles()
{
	kdebugf();
	QStringList ret;
	return ret;
}

QStringList FalfMediaPlayer::getPlayListFiles()
{
	kdebugf();
	QStringList ret;
	return ret;
}

uint FalfMediaPlayer::getPlayListLength()
{
	kdebugf();

	return 0;
}

QString FalfMediaPlayer::getTitle(int position)
{
	Q_UNUSED(position)
	kdebugf();

	return getData(TITLE);
}

QString FalfMediaPlayer::getAlbum(int position)
{
	Q_UNUSED(position)
	kdebugf();

	return getData(ALBUM);
}

QString FalfMediaPlayer::getArtist(int position)
{
	Q_UNUSED(position)
	kdebugf();

	return getData(ARTIST);
}

QString FalfMediaPlayer::getFile(int position)
{
	Q_UNUSED(position)
	kdebugf();

	return "";
}

int FalfMediaPlayer::getLength(int position)
{
	Q_UNUSED(position)
	kdebugf();

	return 0;
}

int FalfMediaPlayer::getCurrentPos()
{
	kdebugf();

	return 0;
}

bool FalfMediaPlayer::isPlaying()
{
	kdebugf();
	
	return (getData(ANY).indexOf("TITLE:") == -1 ? false : true);
}

bool FalfMediaPlayer::isActive()
{
	kdebugf();
	
	return (infoFile.exists());
}
