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
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMetaType>
#include <QtDBus/QDBusReply>
#include <QtCore/QDateTime>

#include "debug.h"
#include "../mediaplayer/mediaplayer.h"
#include "amarok2.h"
#include "amarok2controller.h"

QDBusArgument &operator<<(QDBusArgument& arg, const PlayerStatus& ps)
{
	arg.beginStructure();
	arg << ps.i1;
	arg << ps.i2;
	arg << ps.i3;
	arg << ps.i4;
	arg.endStructure();
	return arg;
}

const QDBusArgument &operator>>(const QDBusArgument& arg, PlayerStatus& ps)
{
	arg.beginStructure();
	arg >> ps.i1;
	arg >> ps.i2;
	arg >> ps.i3;
	arg >> ps.i4;
	arg.endStructure();
	return arg;
}

Amarok2Controller::Amarok2Controller()
{
	active_ = false;

	QDBusConnection bus = QDBusConnection::sessionBus();

	qDBusRegisterMetaType<PlayerStatus>();
	bus.connect(
			SERVICE,
			"/Player",
			"org.freedesktop.MediaPlayer",
			"StatusChange",
			"(iiii)",
			this,
			SLOT(statusChanged(PlayerStatus)));

	bus.connect(
			SERVICE,
			"/Player",
			"org.freedesktop.MediaPlayer",
			"TrackChange",
			"a{sv}",
			this,
			SLOT(trackChanged(QVariantMap)));

	currentTrack_.title  = "";
	currentTrack_.album  = "";
	currentTrack_.artist = "";
	currentTrack_.file   = "";
	currentTrack_.track  = "";
	currentTrack_.time   =  0;
	currentStatus_.i1    =  2;
}

Amarok2Controller::~Amarok2Controller()
{
	QDBusConnection bus = QDBusConnection::sessionBus();

//	qDBusRegisterMetaType<PlayerStatus>();
	bus.disconnect(
			SERVICE,
			"/Player",
			"org.freedesktop.MediaPlayer",
			"StatusChange",
			"(iiii)",
			this,
			SLOT(statusChanged(PlayerStatus)));

	bus.disconnect(
			SERVICE,
			"/Player",
			"org.freedesktop.MediaPlayer",
			"TrackChange",
			"a{sv}",
			this,
			SLOT(trackChanged(QVariantMap)));
}

void Amarok2Controller::statusChanged(PlayerStatus status)
{
	if (!active_)
	{
		active_ = true;
	}
	currentStatus_ = status;
};

void Amarok2Controller::trackChanged(QVariantMap map)
{
	QString title = map.value("title").toString();
	if (title != currentTrack_.title)
	{
		currentTrack_.started = QDateTime::currentDateTime().toTime_t();
		currentTrack_.title   = title;
		currentTrack_.album   = map.value("album").toString();
		currentTrack_.artist  = map.value("artist").toString();
		currentTrack_.track   = map.value("tracknumber").toString();
		currentTrack_.file    = map.value("location").toString();
		currentTrack_.time    = map.value("mtime").toUInt();
	}
};
