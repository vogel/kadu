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

/* Dorr: Amarok2 implements the MPRIS standard so this module should work
 * also with Audacious, BMPx, DragonPLayer, VLC, XMMS2 and maybe others.
 *
 * See http://mpris.org/ for more details about the standard
 */

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMetaType>
#include <QtDBus/QDBusReply>
#include <QtCore/QDateTime>

#include "debug.h"
#include "mediaplayer.h"
#include "mpris_mediaplayer.h"
#include "mpris_controller.h"

MPRISMediaPlayer::MPRISMediaPlayer(QString n, QString s) :
		service(s), name(n)
{
	kdebugf();
	controller = new MPRISController(service, this);

	if (name == "Audacious")
		mediaplayer->setInterval(5);
	else
		mediaplayer->setInterval(0);
}

MPRISMediaPlayer::~MPRISMediaPlayer()
{
	kdebugf();
}

void MPRISMediaPlayer::setService(QString service)
{
	this->service = service;
}

void MPRISMediaPlayer::send(QString obj, QString func, int val)
{
	if (!service.isEmpty())
	{
		QDBusInterface mprisApp(service, obj, "org.freedesktop.MediaPlayer");
		if (val != -1)
			mprisApp.call(func, val);
		else
			mprisApp.call(func);
	}
}

QString MPRISMediaPlayer::getString(QString obj, QString func)
{
	if (!isActive() || service.isEmpty())
		return QString();

	QDBusInterface mprisApp(service, obj, "org.freedesktop.MediaPlayer");
	QDBusReply<QString> reply = mprisApp.call(func);

	if (reply.isValid())
	{
		return reply.value().simplified();
	}
	return QString();
}

int MPRISMediaPlayer::getInt(QString obj, QString func)
{
	if (!isActive() || service.isEmpty())
		return 0;

	QDBusInterface mprisApp(service, obj, "org.freedesktop.MediaPlayer");
	QDBusReply<int> reply = mprisApp.call(func);

	if (reply.isValid())
	{
		return reply.value();
	}
	return -1;
}

QString MPRISMediaPlayer::getStringMapValue(QString obj, QString func, int param, QString field)
{
	if (!service.isEmpty())
	{
		QDBusInterface mprisApp(service, obj, "org.freedesktop.MediaPlayer");
		QDBusReply<QVariantMap> reply = mprisApp.call(func, param);
		if (reply.isValid())
		{
			QVariantMap map = reply.value();
			return map.value(field).toString();
		}
	}
	return QString();
}

int MPRISMediaPlayer::getIntMapValue(QString obj, QString func, int param, QString field)
{
	if (!service.isEmpty())
	{
		QDBusInterface mprisApp(service, obj, "org.freedesktop.MediaPlayer");
		QDBusReply<QVariantMap> reply = mprisApp.call(func, param);
		if (reply.isValid())
		{
			QVariantMap map = reply.value();
			return map.value(field).toInt();
		}
	}
	return -1;
}

// PlayerInfo

QString MPRISMediaPlayer::getPlayerName()
{
	return name;
}

QString MPRISMediaPlayer::getPlayerVersion()
{
	kdebugf();
	return getString("/", "Identity");
	kdebugf2();
}

// TrackList

QStringList MPRISMediaPlayer::getPlayListTitles()
{
	kdebugf();
	int i, num = getPlayListLength();
	QVariantMap map;
	QStringList list;

	for (i = 0; i < num; ++i)
	{
		list << getStringMapValue("/TrackList", "GetMetadata", i, "title");
	}

	return list;
}

QStringList MPRISMediaPlayer::getPlayListFiles()
{
	kdebugf();
	int i, num = getPlayListLength();
	QVariantMap map;
	QStringList list;

	for (i = 0; i < num; ++i)
	{
		/* MPRIS according to the MPRIS specification the file location is stored in location field */
		QString file = getStringMapValue("/TrackList", "GetMetadata", i, "location");
		/* However the audacious (and maybe others) uses URI field instead */
		if (file.isEmpty())
			file = getStringMapValue("/TrackList", "GetMetadata", i, "URI");

		list << file;
	}
	return list;
}

uint MPRISMediaPlayer::getPlayListLength()
{
	kdebugf();
	return getInt("/TrackList", "GetLength");
}

QString MPRISMediaPlayer::getTitle(int position)
{
	kdebugf();
	if (!isPlaying()) return QString();

	if (position == -1)
		return controller->currentTrack().title;
	else
		return getStringMapValue("/TrackList", "GetMetadata", position, "title");
	kdebugf2();
}

QString MPRISMediaPlayer::getAlbum(int position)
{
	kdebugf();
	if (!isPlaying()) return QString();

	if ((position == -1) && !controller->currentTrack().album.isEmpty())
		return controller->currentTrack().album;

	return getStringMapValue("/TrackList", "GetMetadata", position, "album");
	kdebugf2();
}

QString MPRISMediaPlayer::getArtist(int position)
{
	kdebugf();
	if (!isPlaying()) return QString();

	if ((position == -1) && !controller->currentTrack().artist.isEmpty())
		return controller->currentTrack().artist;

	return getStringMapValue("/TrackList", "GetMetadata", position, "artist");
	kdebugf2();
}

QString MPRISMediaPlayer::getFile(int position)
{
	kdebugf();
	if (!isPlaying()) return QString();

	if ((position == -1) && !controller->currentTrack().file.isEmpty())
		return controller->currentTrack().file;

	QString file = getStringMapValue("/TrackList", "GetMetadata", position, "location");
	/* audacious goes different way... */
	if (file.isEmpty())
		file = getStringMapValue("/TrackList", "GetMetadata", position, "URI");

	return file;
}

int MPRISMediaPlayer::getLength(int position)
{
	kdebugf();
	if (!isPlaying()) return 0;

	if (position == -1)
		return controller->currentTrack().time;

	int len = getIntMapValue("/TrackList", "GetMetadata", position, "mtime");
	/* here again audacious uses different filed in metafile... */
	if (len == -1)
		len = getIntMapValue("/TrackList", "GetMetadata", position, "length");

	return len;
	kdebugf2();
}

// Player

int MPRISMediaPlayer::getCurrentPos()
{
	kdebugf();

	if (!isPlaying()) return 0;
	return getInt("/Player", "PositionGet");

	kdebugf2();
}

void MPRISMediaPlayer::nextTrack()
{
	kdebugf();

	send("/Player", "Next");

	kdebugf2();
}

void MPRISMediaPlayer::prevTrack()
{
	kdebugf();

	send("/Player", "Prev");

	kdebugf2();
}

void MPRISMediaPlayer::play()
{
	kdebugf();

	send("/Player", "Play");

	kdebugf2();
}

void MPRISMediaPlayer::stop()
{
	kdebugf();

	send("/Player", "Stop");

	kdebugf2();
}

void MPRISMediaPlayer::pause()
{
	kdebugf();

	send("/Player", "Pause");

	kdebugf2();
}

void MPRISMediaPlayer::setVolume(int vol)
{
	kdebugf();

	send("/Player", "VolumeSet", vol);

	kdebugf2();
}

void MPRISMediaPlayer::incrVolume()
{
	kdebugf();

	int vol = getInt("/Player", "VolumeGet");
	if (vol < 100)
		vol += 2;

	if (vol > 100)
		vol = 100;

	send("/Player", "VolumeSet", vol);

	kdebugf2();
}

void MPRISMediaPlayer::decrVolume()
{
	kdebugf();

	int vol = getInt("/Player", "VolumeGet");
	if (vol > 0)
		vol -= 2;

	if (vol < 0)
		vol = 0;

	send("/Player", "VolumeSet", vol);

	kdebugf2();
}

bool MPRISMediaPlayer::isPlaying()
{
	kdebugf();

	/* refresh the status for audacious */
	if (name == "Audacious")
		controller->getStatus();

	return (controller->currentStatus().i1 == 0);

	kdebugf2();
}

bool MPRISMediaPlayer::isActive()
{
	kdebugf();

	return controller->active();

	kdebugf2();
}
