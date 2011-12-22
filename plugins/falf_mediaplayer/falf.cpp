/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtCore/QStringList>
#include <QtCore/QTextStream>

#include "debug.h"
#include "exports.h"

#include "plugins/mediaplayer/mediaplayer.h"

#include "falf.h"

FalfMediaPlayer::FalfMediaPlayer(QObject *parent) :
		QObject(parent), InfoFile(QDir::homePath() + QLatin1String("/.FALF/track.info"))
{
	kdebugf();
}

FalfMediaPlayer::~FalfMediaPlayer()
{
	kdebugf();
}

int FalfMediaPlayer::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	bool res = MediaPlayer::instance()->registerMediaPlayer(this, 0);
	return res ? 0 : 1;
}

void FalfMediaPlayer::done()
{
	MediaPlayer::instance()->unregisterMediaPlayer();
}

QString FalfMediaPlayer::getData(DataType type)
{
	if (!isActive())
		return QString();

	if (!InfoFile.open(QIODevice::ReadOnly))
		return QString();

	QTextStream sI(&InfoFile);
	sI.setCodec("UTF-8");

	QString buffer;

	switch (type)
	{
		case TypeAny:
			buffer = sI.readLine();
			break;
		case TypeTitle:
			buffer = sI.readLine();
			buffer = buffer.mid(7);
			break;
		case TypeAlbum:
			for (int i = 0 ; i < 2 ; i++)
				buffer = sI.readLine();
			buffer = buffer.mid(7);
			break;
		case TypeArtist:
			for (int i = 0 ; i < 3 ; i++)
				buffer = sI.readLine();
			buffer = buffer.mid(8);
			break;
		case TypeVersion:
			for (int i = 0 ; i < 5 ; i++)
				buffer = sI.readLine();
			buffer = buffer.mid(12);
			break;
	}

	InfoFile.close();

	return buffer.simplified();
}

QString FalfMediaPlayer::getPlayerName()
{
	kdebugf();
	return QLatin1String("FALF");
}

QString FalfMediaPlayer::getPlayerVersion()
{
	kdebugf();
	return getData(TypeVersion);
}

QStringList FalfMediaPlayer::getPlayListTitles()
{
	kdebugf();
	return QStringList();
}

QStringList FalfMediaPlayer::getPlayListFiles()
{
	kdebugf();
	return QStringList();
}

QString FalfMediaPlayer::getTitle()
{
	kdebugf();
	return getData(TypeTitle);
}

QString FalfMediaPlayer::getAlbum()
{
	kdebugf();
	return getData(TypeAlbum);
}

QString FalfMediaPlayer::getArtist()
{
	kdebugf();
	return getData(TypeArtist);
}

QString FalfMediaPlayer::getFile()
{
	kdebugf();
	return QString();
}

int FalfMediaPlayer::getLength()
{
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
	return (getData(TypeAny).indexOf("TITLE:") != -1);
}

bool FalfMediaPlayer::isActive()
{
	kdebugf();
	return InfoFile.exists();
}

Q_EXPORT_PLUGIN2(falf_mediaplayer, FalfMediaPlayer)
