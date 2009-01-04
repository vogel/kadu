/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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

MPRISMediaPlayer::MPRISMediaPlayer(QString n, QString s) : name(n), service(s)
{
	kdebugf();
	controller = new MPRISController(service);
}

MPRISMediaPlayer::~MPRISMediaPlayer()
{
	kdebugf();
	delete controller;
	controller = 0;
}

void MPRISMediaPlayer::send(QString obj, QString func, int val)
{
	QDBusInterface amarokApp(service, obj, "org.freedesktop.MediaPlayer");
	if (val != -1)
		amarokApp.call(func, val);
	else
		amarokApp.call(func);
}

QString MPRISMediaPlayer::getString(QString obj, QString func)
{
	if (!isActive())
		return "";

	QDBusInterface amarokApp(service, obj, "org.freedesktop.MediaPlayer");
	QDBusReply<QString> reply = amarokApp.call(func);
                               
	if (reply.isValid())
	{
		return reply.value().simplified();
	}
	return "";
}

int MPRISMediaPlayer::getInt(QString obj, QString func)
{
	if (!isActive())
		return 0;

	QDBusInterface amarokApp(service, obj, "org.freedesktop.MediaPlayer");
	QDBusReply<int> reply = amarokApp.call(func);
                               
	if (reply.isValid())
	{
		return reply.value();
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

QStringList MPRISMediaPlayer::getPlayListTitles()
{
	kdebugf();

	QStringList list;
	return list;

	kdebugf2();
}

QStringList MPRISMediaPlayer::getPlayListFiles()
{
	kdebugf();

	QStringList list;
	return list;

	kdebugf2();
}

uint MPRISMediaPlayer::getPlayListLength()
{
	kdebugf();

	return getInt("/TrackList", "GetLength");

	kdebugf2();
}

QString MPRISMediaPlayer::getTitle(int position)
{
	kdebugf();

	if (!isPlaying()) return "";
	return controller->currentTrack().title;

	kdebugf2();
}

QString MPRISMediaPlayer::getAlbum(int position)
{
	kdebugf();

	if (!isPlaying()) return "";
	return controller->currentTrack().album;

	kdebugf2();
}

QString MPRISMediaPlayer::getArtist(int position)
{
	kdebugf();

	if (!isPlaying()) return "";
	return controller->currentTrack().artist;

	kdebugf2();
}

QString MPRISMediaPlayer::getFile(int position)
{
	kdebugf();

	if (!isPlaying()) return "";
	return controller->currentTrack().file;

	kdebugf2();
}

int MPRISMediaPlayer::getLength(int position)
{
	kdebugf();

	if (!isPlaying()) return 0;
	return controller->currentTrack().time;

	kdebugf2();
}

int MPRISMediaPlayer::getCurrentPos()
{
	kdebugf();

	if (!isPlaying()) return 0;
	return (QDateTime::currentDateTime().toTime_t() - controller->currentTrack().started) * 1000;

	kdebugf2();
}

// PlayerCommands

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

	return (controller->currentStatus().i1 == 0);

	kdebugf2();
}

bool MPRISMediaPlayer::isActive()
{
	kdebugf();

	return controller->active();

	kdebugf2();
}
