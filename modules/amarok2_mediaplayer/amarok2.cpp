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
#include "../mediaplayer/mediaplayer.h"
#include "amarok2.h"
#include "amarok2controller.h"

Amarok2MediaPlayer* amarok2;

// Kadu initializing functions
extern "C" int amarok2_mediaplayer_init()
{
	amarok2 = new Amarok2MediaPlayer();
	bool res = mediaplayer->registerMediaPlayer(amarok2, amarok2);
	return res ? 0 : 1;
}

extern "C" void amarok2_mediaplayer_close()
{
	mediaplayer->unregisterMediaPlayer();
	delete amarok2;
	amarok2 = NULL;
}

Amarok2MediaPlayer::Amarok2MediaPlayer()
{
	kdebugf();
	controller = new Amarok2Controller();
}

Amarok2MediaPlayer::~Amarok2MediaPlayer()
{
	kdebugf();
	delete controller;
	controller = 0;
}

void Amarok2MediaPlayer::send(QString obj, QString func, int val)
{
	QDBusInterface amarokApp(SERVICE, obj, "org.freedesktop.MediaPlayer");
	if (val != -1)
		amarokApp.call(func, val);
	else
		amarokApp.call(func);
}

QString Amarok2MediaPlayer::getString(QString obj, QString func)
{
	if (!isActive())
		return "";

	QDBusInterface amarokApp(SERVICE, obj, "org.freedesktop.MediaPlayer");
	QDBusReply<QString> reply = amarokApp.call(func);
                               
	if (reply.isValid())
	{
		return reply.value().simplified();
	}
	return "";
}

int Amarok2MediaPlayer::getInt(QString obj, QString func)
{
	if (!isActive())
		return 0;

	QDBusInterface amarokApp(SERVICE, obj, "org.freedesktop.MediaPlayer");
	QDBusReply<int> reply = amarokApp.call(func);
                               
	if (reply.isValid())
	{
		return reply.value();
	}
	return -1;
}

// PlayerInfo

QString Amarok2MediaPlayer::getPlayerName()
{
	return "Amarok";
}

QString Amarok2MediaPlayer::getPlayerVersion()
{
	kdebugf();

	return getString("/", "Identity");

	kdebugf2();
}

QStringList Amarok2MediaPlayer::getPlayListTitles()
{
	kdebugf();

	QStringList list;
	return list;

	kdebugf2();
}

QStringList Amarok2MediaPlayer::getPlayListFiles()
{
	kdebugf();

	QStringList list;
	return list;

	kdebugf2();
}

uint Amarok2MediaPlayer::getPlayListLength()
{
	kdebugf();

	return getInt("/TrackList", "GetLength");

	kdebugf2();
}

QString Amarok2MediaPlayer::getTitle(int position)
{
	kdebugf();

	if (!isPlaying()) return "";
	return controller->currentTrack().title;

	kdebugf2();
}

QString Amarok2MediaPlayer::getAlbum(int position)
{
	kdebugf();

	if (!isPlaying()) return "";
	return controller->currentTrack().album;

	kdebugf2();
}

QString Amarok2MediaPlayer::getArtist(int position)
{
	kdebugf();

	if (!isPlaying()) return "";
	return controller->currentTrack().artist;

	kdebugf2();
}

QString Amarok2MediaPlayer::getFile(int position)
{
	kdebugf();

	if (!isPlaying()) return "";
	return controller->currentTrack().file;

	kdebugf2();
}

int Amarok2MediaPlayer::getLength(int position)
{
	kdebugf();

	if (!isPlaying()) return 0;
	return controller->currentTrack().time;

	kdebugf2();
}

int Amarok2MediaPlayer::getCurrentPos()
{
	kdebugf();

	if (!isPlaying()) return 0;
	return (QDateTime::currentDateTime().toTime_t() - controller->currentTrack().started) * 1000;

	kdebugf2();
}

// PlayerCommands

void Amarok2MediaPlayer::nextTrack()
{
	kdebugf();

	send("/Player", "Next");

	kdebugf2();
}

void Amarok2MediaPlayer::prevTrack()
{
	kdebugf();

	send("/Player", "Prev");

	kdebugf2();
}

void Amarok2MediaPlayer::play()
{
	kdebugf();

	send("/Player", "Play");

	kdebugf2();
}

void Amarok2MediaPlayer::stop()
{
	kdebugf();

	send("/Player", "Stop");

	kdebugf2();
}

void Amarok2MediaPlayer::pause()
{
	kdebugf();

	send("/Player", "Pause");

	kdebugf2();
}

void Amarok2MediaPlayer::setVolume(int vol)
{
	kdebugf();

	send("/Player", "VolumeSet", vol);

	kdebugf2();
}

void Amarok2MediaPlayer::incrVolume()
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

void Amarok2MediaPlayer::decrVolume()
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

bool Amarok2MediaPlayer::isPlaying()
{
	kdebugf();

	return (controller->currentStatus().i1 == 0);

	kdebugf2();
}

bool Amarok2MediaPlayer::isActive()
{
	kdebugf();

	return controller->active();

	kdebugf2();
}
