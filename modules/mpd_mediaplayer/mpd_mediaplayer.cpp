/*
 * %kadu copyright begin%
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "../mediaplayer/mediaplayer.h"
#include "exports.h"

#include "mpd_mediaplayer.h"

MPDMediaPlayer* mpd = NULL;

extern "C" KADU_EXPORT int mpd_mediaplayer_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	mpd = new MPDMediaPlayer();
	bool res = mediaplayer->registerMediaPlayer(mpd, mpd);
	return res ? 0 : 1;
}

extern "C" KADU_EXPORT void mpd_mediaplayer_close()
{
	mediaplayer->unregisterMediaPlayer();
	delete mpd;
	mpd = NULL;
}

MPDMediaPlayer::MPDMediaPlayer()
{
	config = new MPDConfig();
}

MPDMediaPlayer::~MPDMediaPlayer()
{
	delete config;
}

mpd_connection* MPDMediaPlayer::mpdConnect()
{
	const char *host = config->host.toAscii().data();
	unsigned int port = config->port.toUInt();
	unsigned int timeout = config->timeout.toUInt();

	return mpd_connection_new(host, port, timeout * 1000);
}

QString MPDMediaPlayer::getTitle(int position)
{
	QString title;
	mpd_connection *con = mpdConnect();

	if (NULL != con)
	{
		mpd_status *status = mpd_run_status(con);
		if (NULL != status)
		{
			if (-1 == position)
		   		position = mpd_status_get_song_pos(status);

			mpd_song *song = mpd_run_get_queue_song_pos(con, position);
			if (NULL != song)
			{
				title = QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_TITLE, 0));
				mpd_song_free(song);
			}
			mpd_status_free(status);
		}
	    mpd_connection_free(con);
	}
	return title;
}

QString MPDMediaPlayer::getArtist(int position)
{
	QString artist;
	mpd_connection *con = mpdConnect();

	if (NULL != con)
	{
		mpd_status *status = mpd_run_status(con);
		if (NULL != status)
		{
			if (-1 == position)
				position = mpd_status_get_song_pos(status);

			mpd_song *song = mpd_run_get_queue_song_pos(con, position);
			if (NULL != song)
			{
				artist = QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_ARTIST, 0));
				mpd_song_free(song);
			}
			mpd_status_free(status);
		}
		mpd_connection_free(con);
	}
	return artist;
}

QString MPDMediaPlayer::getAlbum(int position)
{
	QString album;
	mpd_connection *con = mpdConnect();

	if (NULL != con)
	{
		mpd_status *status = mpd_run_status(con);
		if (NULL != status)
		{
			if (-1 == position)
				position = mpd_status_get_song_pos(status);

			mpd_song *song = mpd_run_get_queue_song_pos(con, position);
			if (NULL != song)
			{
				album = QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_ALBUM, 0));
				mpd_song_free(song);
			}
			mpd_status_free(status);
		}
		mpd_connection_free(con);
	}
	return album;
}

QString MPDMediaPlayer::getFile(int position)
{
	QString file;
	mpd_connection *con = mpdConnect();

	if (NULL != con)
	{
		mpd_status *status = mpd_run_status(con);
		if (NULL != status)
		{
			if (-1 == position)
				position = mpd_status_get_song_pos(status);

			mpd_song *song = mpd_run_get_queue_song_pos(con, position);
			if (NULL != song)
			{
				file = mpd_song_get_uri(song);
				file = file.right(file.length()-file.lastIndexOf("/")-1);
				mpd_song_free(song);
			}
			mpd_status_free(status);
		}
		mpd_connection_free(con);
	}
	return file;
}

int MPDMediaPlayer::getLength(int position)
{
	int length = 0;
	mpd_connection *con = mpdConnect();

	if (NULL != con)
	{
		mpd_status *status = mpd_run_status(con);
		if (NULL != status)
		{
			if (-1 == position)
				position = mpd_status_get_song_pos(status);

			mpd_song *song = mpd_run_get_queue_song_pos(con, position);
			if (NULL != song)
			{
				length = mpd_song_get_duration(song);
				mpd_song_free(song);
			}
			mpd_status_free(status);
		}
		mpd_connection_free(con);
	}
	return length * 1000;
}

int MPDMediaPlayer::getCurrentPos()
{
	int pos = 0;
	mpd_connection *con = mpdConnect();

	if (NULL != con)
	{
		mpd_status *status = mpd_run_status(con);
		if (NULL != status)
		{
			pos = mpd_status_get_elapsed_ms(status);
			mpd_status_free(status);
		}
		mpd_connection_free(con);
	}
	return pos;
}

bool MPDMediaPlayer::isPlaying()
{
	mpd_connection *con = mpdConnect();

	if (NULL == con)
		return false;
	else
	{
		mpd_status *status = mpd_run_status(con);
		if (NULL == status)
			return false;

		if (mpd_status_get_state(status) != MPD_STATE_PLAY)
		{
			mpd_status_free(status);
			mpd_connection_free(con);
			return false;
		}
		else
		{
			mpd_status_free(status);
			mpd_connection_free(con);
			return true;
		}
	}
}

bool MPDMediaPlayer::isActive()
{
	mpd_connection *con = mpdConnect();

	if (NULL == con)
		return false;
	else
	{
		if (mpd_connection_get_error(con) == MPD_ERROR_SUCCESS)
		{
			mpd_connection_free(con);
			return true;
		}
		else
		{
			mpd_connection_free(con);
			return false;
		}
	}
}

QStringList MPDMediaPlayer::getPlayListTitles()
{
	QStringList playlist;
	mpd_connection *con = mpdConnect();

	if (NULL != con)
	{
		mpd_song *song;
		mpd_send_list_queue_meta(con);
		while ((song = mpd_recv_song(con)) != NULL)
		{
			playlist << QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_TITLE, 0));
			mpd_song_free(song);
		}
		mpd_connection_free(con);
	}
	return playlist;
}

QStringList MPDMediaPlayer::getPlayListFiles()
{
	QStringList playlistFiles;
	QString file;
	mpd_connection *con = mpdConnect();

	if (NULL != con)
	{
		mpd_song *song;
		mpd_send_list_queue_meta(con);
		while ((song = mpd_recv_song(con)) != NULL)
		{
			file = QString::fromUtf8(mpd_song_get_uri(song));
			// get only filename without directories
			file = file.right(file.length()-file.lastIndexOf("/")-1);
			playlistFiles << file;
			mpd_song_free(song);
		}
		mpd_connection_free(con);
	}
	return playlistFiles;
}

uint MPDMediaPlayer::getPlayListLength()
{
	uint playlistLength = 0;
	mpd_connection *con = mpdConnect();

	if (NULL != con)
	{
		mpd_status *status = mpd_run_status(con);
		if (status != NULL)
		{
			playlistLength = mpd_status_get_queue_length(status);
			mpd_status_free(status);
		}
		mpd_connection_free(con);
	}
	return playlistLength;
}

QString MPDMediaPlayer::getPlayerName()
{
	return QString("mpd");
}

QString MPDMediaPlayer::getPlayerVersion()
{
	QString version, major, minor, patch;
	mpd_connection *con = mpdConnect();

	if (NULL != con)
	{
		const unsigned *ver = mpd_connection_get_server_version(con);
		if (NULL != ver)
		{
			major.setNum(ver[0]);
			minor.setNum(ver[1]);
			patch.setNum(ver[2]);
			version = major + '.' + minor + '.' + patch;
		}
		mpd_connection_free(con);
	}
	else
		version = "unknown";

	return version;
}

void MPDMediaPlayer::nextTrack()
{
	mpd_connection *con = mpdConnect();

	if (NULL != con)
	{
		mpd_run_next(con);
		mpd_connection_free(con);
	}
}

void MPDMediaPlayer::prevTrack()
{
	mpd_connection *con = mpdConnect();

	if (NULL != con)
	{
		mpd_run_previous(con);
		mpd_connection_free(con);
	}
}

void MPDMediaPlayer::play()
{
	mpd_connection *con = mpdConnect();

	if (NULL != con)
	{
		mpd_status *status = mpd_run_status(con);
		if (NULL != status)
		{
			if (mpd_status_get_state(status) == MPD_STATE_PAUSE ||
				mpd_status_get_state(status) == MPD_STATE_STOP)
			{
				mpd_run_play(con);
				mpd_status_free(status);
			}
		}
		mpd_connection_free(con);
	}
}

void MPDMediaPlayer::stop()
{
	mpd_connection *con = mpdConnect();

	if (NULL != con)
	{
		mpd_status *status = mpd_run_status(con);
		if (NULL != status)
		{
			if (mpd_status_get_state(status) == MPD_STATE_PLAY ||
				mpd_status_get_state(status) == MPD_STATE_PAUSE)
			{
				mpd_run_stop(con);
				mpd_status_free(status);
			}
		}
		mpd_connection_free(con);
	}
}

void MPDMediaPlayer::pause()
{
	mpd_connection *con = mpdConnect();

	if (NULL != con)
	{
		mpd_status *status = mpd_run_status(con);
		if (NULL != status)
		{
			if (mpd_status_get_state(status) == MPD_STATE_PLAY)
			{
				mpd_run_pause(con, true);
				mpd_status_free(status);
			}
		}
		mpd_connection_free(con);
	}
}

void MPDMediaPlayer::setVolume(int vol)
{
	mpd_connection *con = mpdConnect();

	if (NULL != con)
	{
		mpd_run_set_volume(con, vol);
		mpd_connection_free(con);
	}
}

void MPDMediaPlayer::incrVolume()
{
	mpd_connection *con = mpdConnect();

	if (NULL != con)
	{
		mpd_status *status = mpd_run_status(con);
		if (NULL != status)
		{
			int oldVolume = mpd_status_get_volume(status);
			int newVolume = oldVolume + 10;
			mpd_status_free(status);

			if (newVolume > 100)
				newVolume = 100;
			mpd_run_set_volume(con, newVolume);
		}
		mpd_connection_free(con);
	}
}

void MPDMediaPlayer::decrVolume()
{
	mpd_connection *con = mpdConnect();

	if (NULL != con)
	{
		mpd_status *status = mpd_run_status(con);
		if (NULL != status)
		{
			int oldVolume = mpd_status_get_volume(status);
			int newVolume = oldVolume - 10;
			mpd_status_free(status);

			if (newVolume < 0)
				newVolume = 0;
			mpd_send_set_volume(con, newVolume);
		}
		mpd_connection_free(con);
	}
}
