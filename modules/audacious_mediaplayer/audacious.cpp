/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "config.h"

#ifndef AUDACIOUS_LEGACY
#include <audacious/audctrl.h>
#include <audacious/dbus.h>
#else
#include <audacious/beepctrl.h>
#define audacious_remote_is_running(x)			xmms_remote_is_running(x)
#define audacious_remote_is_playing(x)			xmms_remote_is_playing(x)
#define audacious_remote_get_playlist_pos(x)		xmms_remote_get_playlist_pos(x)
#define audacious_remote_get_playlist_title(x,y)	xmms_remote_get_playlist_title(x,y)
#define audacious_remote_get_playlist_file(x,y)		xmms_remote_get_playlist_file(x,y)
#define audacious_remote_get_playlist_time(x,y)		xmms_remote_get_playlist_time(x,y)
#define audacious_remote_get_output_time(x)		xmms_remote_get_output_time(x)
#define audacious_remote_get_playlist_length(x)		xmms_remote_get_playlist_length(x)
#define audacious_remote_get_version(x)			xmms_remote_get_version(x)
#define audacious_remote_playlist_next(x)		xmms_remote_playlist_next(x)
#define audacious_remote_playlist_prev(x)		xmms_remote_playlist_prev(x)
#define audacious_remote_play(x)			xmms_remote_play(x)
#define audacious_remote_stop(x)			xmms_remote_stop(x)
#define audacious_remote_pause(x)			xmms_remote_pause(x)
#define audacious_remote_set_main_volume(x,y)		xmms_remote_set_main_volume(x,y)
#define audacious_remote_get_main_volume(x)		xmms_remote_get_main_volume(x)
#endif

#include "debug.h"
#include "../mediaplayer/mediaplayer.h"
#include "audacious.h"

#define MODULE_AUDACIOUS_MEDIAPLAYER_VERSION 1.2

// For CP1250->ISO8859-2 converter TODO: make it kadu-global
const char CODE_CP1250[]	= {0xb9, 0x9c, 0x9f, 0xa5, 0x8c, 0x8f};
const char CODE_ISO8859_2[] = {0xb1, 0xb6, 0xbc, 0xa1, 0xa6, 0xac};

AudaciousMediaPlayer *audacious;

#ifndef AUDACIOUS_LEGACY
DBusGProxy *audacious_session = NULL;
DBusGConnection *connection = NULL;
#else
gint audacious_session = 0;
#endif

// Kadu initializing functions
extern "C" int audacious_mediaplayer_init()
{
#ifndef AUDACIOUS_LEGACY
	g_type_init();

	GError *error = NULL;
	connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
	if (connection == NULL)
	{
		kdebugm(KDEBUG_ERROR, "DBUS-Error: %s \n", error->message);
		return 1;
	}

	audacious_session = dbus_g_proxy_new_for_name(connection, AUDACIOUS_DBUS_SERVICE, AUDACIOUS_DBUS_PATH, AUDACIOUS_DBUS_INTERFACE);
	if (audacious_session == NULL)
	{
		kdebugm(KDEBUG_ERROR, "Unable to create dbus proxy\n");
		return 1;
	}
#endif

	audacious = new AudaciousMediaPlayer();
	if (!mediaplayer->registerMediaPlayer(audacious, audacious))
		return 1;

	return 0;
}

extern "C" void audacious_mediaplayer_close()
{
	mediaplayer->unregisterMediaPlayer();
#ifndef AUDACIOUS_LEGACY
	// Release reference to dbus proxy
	g_object_unref (audacious_session);
#endif
	delete audacious;
	audacious = 0;
}

AudaciousMediaPlayer::AudaciousMediaPlayer()
{
	kdebugf();
}

AudaciousMediaPlayer::~AudaciousMediaPlayer()
{
	kdebugf();
}

// PlayerInfo

QString AudaciousMediaPlayer::getPlayerName()
{
	return "Audacious";
}

QString AudaciousMediaPlayer::getPlayerVersion()
{
	kdebugf();
	if (!isActive())
		return "";

	return QString("%1").arg(audacious_remote_get_version(audacious_session));
}

QStringList AudaciousMediaPlayer::getPlayListTitles()
{
	kdebugf();
	QStringList list;

	// Firstly, how many items are in playlist?
	uint lgt = getPlayListLength();
	if (lgt == 0)
		return list;

	// Secondary, get all titles
	for ( uint cnt = 0; cnt < lgt; cnt++ )
		list.append(getTitle(cnt));

	return list;
}

QStringList AudaciousMediaPlayer::getPlayListFiles()
{
	kdebugf();
	QStringList list;

	// Firstly, how many items are in playlist?
	uint lgt = getPlayListLength();
	if (lgt == 0)
		return list;

	// Secondary, get all file names
	for ( uint cnt = 0; cnt < lgt; cnt++ )
		list.append(getFile(cnt));

	return list;
}

uint AudaciousMediaPlayer::getPlayListLength()
{
	kdebugf();
	return audacious_remote_get_playlist_length(audacious_session);
}

QString AudaciousMediaPlayer::getTitle(int position)
{
	kdebugf();
	if (!isActive())
		return "";

	// Do we want current track title, or given track title?
	if (position == -1)
		position = audacious_remote_get_playlist_pos(audacious_session);

	QString title(convert(audacious_remote_get_playlist_title(audacious_session, position)));
	return title;
}

QString AudaciousMediaPlayer::getAlbum(int position)
{
	return QString::null;
}

QString AudaciousMediaPlayer::getArtist(int position)
{
	return QString::null;
}

QString AudaciousMediaPlayer::getFile(int position)
{
	kdebugf();

	// Do we want current track file name, or given track file name?
	if (position == -1)
		position = audacious_remote_get_playlist_pos(audacious_session);

	return convert(audacious_remote_get_playlist_file(audacious_session, position));
}

int AudaciousMediaPlayer::getLength(int position)
{
	kdebugf();

	// Do we want current track file name, or given track file name?
	if (position == -1)
		position = audacious_remote_get_playlist_pos(audacious_session);

	return audacious_remote_get_playlist_time(audacious_session, position);
}

int AudaciousMediaPlayer::getCurrentPos()
{
	kdebugf();
	return audacious_remote_get_output_time(audacious_session);
}

// PlayerCommands

void AudaciousMediaPlayer::nextTrack()
{
	kdebugf();
	audacious_remote_playlist_next(audacious_session);
}

void AudaciousMediaPlayer::prevTrack()
{
	kdebugf();
	audacious_remote_playlist_prev(audacious_session);
}

void AudaciousMediaPlayer::play()
{
	kdebugf();
	audacious_remote_play(audacious_session);
}

void AudaciousMediaPlayer::stop()
{
	kdebugf();
	audacious_remote_stop(audacious_session);
}

void AudaciousMediaPlayer::pause()
{
	kdebugf();
	audacious_remote_pause(audacious_session);
}

void AudaciousMediaPlayer::setVolume(int vol)
{
	kdebugf();
	audacious_remote_set_main_volume(audacious_session, vol);
}

void AudaciousMediaPlayer::incrVolume()
{
	kdebugf();
	int vol = audacious_remote_get_main_volume(audacious_session);
	vol += 2;
	if (vol > 100)
		vol = 100;

	setVolume(vol);
}

void AudaciousMediaPlayer::decrVolume()
{
	kdebugf();
	int vol = audacious_remote_get_main_volume(audacious_session);
	vol -= 2;
	if (vol < 0)
		vol = 0;

	setVolume(vol);
}

bool AudaciousMediaPlayer::isPlaying()
{
	kdebugf();
	return audacious_remote_is_playing(audacious_session);
}

bool AudaciousMediaPlayer::isActive()
{
	kdebugf();
	return audacious_remote_is_running(audacious_session);
}

QString AudaciousMediaPlayer::convert(QString str)
{
	for (unsigned int i = 0; i < sizeof(CODE_CP1250); i++)
	{
		str.replace(CODE_CP1250[i],CODE_ISO8859_2[i]);
	}
	return QString::fromLocal8Bit(str,-1);
}
