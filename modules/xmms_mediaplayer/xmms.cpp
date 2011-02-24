/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
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

#include <xmms/xmmsctrl.h>

#include "debug.h"
#include "exports.h"
#include "../mediaplayer/mediaplayer.h"
#include "xmms.h"

#define MODULE_XMMS_MEDIAPLAYER_VERSION 1.0

// For CP1250->ISO8859-2 converter TODO: make it kadu-globall
const char CODE_CP1250[]	= {0xb9, 0x9c, 0x9f, 0xa5, 0x8c, 0x8f};
const char CODE_ISO8859_2[] = {0xb1, 0xb6, 0xbc, 0xa1, 0xa6, 0xac};

XmmsMediaPlayer *xmms_mediaplayer_session;

extern "C" KADU_EXPORT int xmms_mediaplayer_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	xmms_mediaplayer_session = new XmmsMediaPlayer();
	bool res = mediaplayer->registerMediaPlayer(xmms_mediaplayer_session, xmms_mediaplayer_session);
	return res ? 0 : 1;
}

extern "C" KADU_EXPORT void xmms_mediaplayer_close()
{
	mediaplayer->unregisterMediaPlayer();
	delete xmms_mediaplayer_session;
}

XmmsMediaPlayer::XmmsMediaPlayer()
{
	kdebugf();
}

XmmsMediaPlayer::~XmmsMediaPlayer()
{
	kdebugf();
}

// PlayerInfo

QString XmmsMediaPlayer::getPlayerName()
{
	return "XMMS";
}

QString XmmsMediaPlayer::getPlayerVersion()
{
	return QString();
}

QStringList XmmsMediaPlayer::getPlayListTitles()
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

QStringList XmmsMediaPlayer::getPlayListFiles()
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

uint XmmsMediaPlayer::getPlayListLength()
{
	kdebugf();
	return xmms_remote_get_playlist_length(0);
}

QString XmmsMediaPlayer::getTitle(int position)
{
	kdebugf();
	if (!xmms_remote_is_running(0))
		return "";

	// Do we want current track title, or given track title?
	if (position == -1)
		position = xmms_remote_get_playlist_pos(0);

	// Gets the title and converts CP1250 to ISO8859-2 on the fly
	QString title(convert(xmms_remote_get_playlist_title(0, position)));
	return title;
}

QString XmmsMediaPlayer::getAlbum(int position)
{
	return QString();
}

QString XmmsMediaPlayer::getArtist(int position)
{
	return QString();
}

QString XmmsMediaPlayer::getFile(int position)
{
	kdebugf();

	// Do we want current track file name, or given track file name?
	if (position == -1)
		position = xmms_remote_get_playlist_pos(0);

	// Converts CP1250 to ISO8859-2 on the fly while returning file name
	return convert(xmms_remote_get_playlist_file(0, position));
}

int XmmsMediaPlayer::getLength(int position)
{
	kdebugf();

	// Do we want current track file name, or given track file name?
	if (position == -1)
		position = xmms_remote_get_playlist_pos(0);

	return xmms_remote_get_playlist_time(0, position);
}

int XmmsMediaPlayer::getCurrentPos()
{
	kdebugf();
	return xmms_remote_get_output_time(0);
}

// PlayerCommands

void XmmsMediaPlayer::nextTrack()
{
	kdebugf();
	xmms_remote_playlist_next(0);
}

void XmmsMediaPlayer::prevTrack()
{
	kdebugf();
	xmms_remote_playlist_prev(0);
}

void XmmsMediaPlayer::play()
{
	kdebugf();
	xmms_remote_play(0);
}

void XmmsMediaPlayer::stop()
{
	kdebugf();
	xmms_remote_stop(0);
}

void XmmsMediaPlayer::pause()
{
	kdebugf();
	xmms_remote_pause(0);
}

void XmmsMediaPlayer::setVolume(int vol)
{
	kdebugf();
	xmms_remote_set_main_volume(0, vol);
}

void XmmsMediaPlayer::incrVolume()
{
	kdebugf();
	int vol = xmms_remote_get_main_volume(0);
	vol += 2;
	if (vol > 100)
		vol = 100;

	setVolume(vol);
}

void XmmsMediaPlayer::decrVolume()
{
	kdebugf();
	int vol = xmms_remote_get_main_volume(0);
	vol -= 2;
	if (vol < 0)
		vol = 0;

	setVolume(vol);
}

bool XmmsMediaPlayer::isPlaying()
{
	kdebugf();
	return xmms_remote_is_playing(0);
}

bool XmmsMediaPlayer::isActive()
{
	kdebugf();
	return xmms_remote_get_version(0) > 0;
}

QString XmmsMediaPlayer::convert(QString str)
{
	for (unsigned int i = 0; i < sizeof(CODE_CP1250); i++)
	{
		str.replace(CODE_CP1250[i],CODE_ISO8859_2[i]);
	}
	return QString::fromLocal8Bit(str,-1);
}
