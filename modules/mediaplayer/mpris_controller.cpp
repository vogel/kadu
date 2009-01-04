/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* 
 * MPRIS standard implementation (used in Amarok2, Audacious, BMPx, 
 * Dragon Player, VLC, XMMS2
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
#include "mediaplayer.h"
//#include "mpris_mediaplayer.h"
#include "mpris_controller.h"

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

MPRISController::MPRISController(QString s) : service(s)
{
	active_ = false;

	QDBusConnection bus = QDBusConnection::sessionBus();

	qDBusRegisterMetaType<PlayerStatus>();
	bus.connect(
			service,
			"/Player",
			"org.freedesktop.MediaPlayer",
			"StatusChange",
			"(iiii)",
			this,
			SLOT(statusChanged(PlayerStatus)));

	bus.connect(
			service,
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

MPRISController::~MPRISController()
{
	QDBusConnection bus = QDBusConnection::sessionBus();

//	qDBusUnregisterMetaType<PlayerStatus>();
	bus.disconnect(
			service,
			"/Player",
			"org.freedesktop.MediaPlayer",
			"StatusChange",
			"(iiii)",
			this,
			SLOT(statusChanged(PlayerStatus)));

	bus.disconnect(
			service,
			"/Player",
			"org.freedesktop.MediaPlayer",
			"TrackChange",
			"a{sv}",
			this,
			SLOT(trackChanged(QVariantMap)));
}

void MPRISController::statusChanged(PlayerStatus status)
{
	if (!active_)
	{
		active_ = true;
	}
	currentStatus_ = status;
};

void MPRISController::trackChanged(QVariantMap map)
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
