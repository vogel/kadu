/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "plugins/mediaplayer/mediaplayer.h"

#include "mpd-mediaplayer.h"

MPDMediaPlayer::MPDMediaPlayer(QObject *parent) :
		PlayerCommands(parent)
{
}

MPDMediaPlayer::~MPDMediaPlayer()
{
}

bool MPDMediaPlayer::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	return MediaPlayer::instance()->registerMediaPlayer(this, this);
}

void MPDMediaPlayer::done()
{
	MediaPlayer::instance()->unregisterMediaPlayer();
}

mpd_connection * MPDMediaPlayer::mpdConnect()
{
	const char *host = Config.host().toUtf8().constData();
	unsigned int port = Config.port().toUInt();
	unsigned int timeout = Config.timeout().toUInt();

	return mpd_connection_new(host, port, timeout * 1000);
}

QString MPDMediaPlayer::getTitle()
{
	mpd_connection *con = mpdConnect();
	if (!con)
		return QString();

	QString title;
	mpd_status *status = mpd_run_status(con);
	if (status)
	{
		int position = mpd_status_get_song_pos(status);

		mpd_song *song = mpd_run_get_queue_song_pos(con, position);
		if (song)
		{
			title = QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_TITLE, 0));
			mpd_song_free(song);
		}
		mpd_status_free(status);
	}
	mpd_connection_free(con);

	return title;
}

QString MPDMediaPlayer::getArtist()
{
	mpd_connection *con = mpdConnect();
	if (!con)
		return QString();

	QString artist;
	mpd_status *status = mpd_run_status(con);
	if (status)
	{
		int position = mpd_status_get_song_pos(status);

		mpd_song *song = mpd_run_get_queue_song_pos(con, position);
		if (song)
		{
			artist = QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_ARTIST, 0));
			mpd_song_free(song);
		}
		mpd_status_free(status);
	}
	mpd_connection_free(con);

	return artist;
}

QString MPDMediaPlayer::getAlbum()
{
	mpd_connection *con = mpdConnect();
	if (!con)
		return QString();

	QString album;
	mpd_status *status = mpd_run_status(con);
	if (status)
	{
		int position = mpd_status_get_song_pos(status);

		mpd_song *song = mpd_run_get_queue_song_pos(con, position);
		if (song)
		{
			album = QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_ALBUM, 0));
			mpd_song_free(song);
		}
		mpd_status_free(status);
	}
	mpd_connection_free(con);

	return album;
}

QString MPDMediaPlayer::getFile()
{
	mpd_connection *con = mpdConnect();
	if (!con)
		return QString();

	QString file;
	mpd_status *status = mpd_run_status(con);
	if (NULL != status)
	{
		int position = mpd_status_get_song_pos(status);

		mpd_song *song = mpd_run_get_queue_song_pos(con, position);
		if (NULL != song)
		{
			file = mpd_song_get_uri(song);
			file = file.right(file.length() - file.lastIndexOf('/') - 1);
			mpd_song_free(song);
		}
		mpd_status_free(status);
	}
	mpd_connection_free(con);

	return file;
}

int MPDMediaPlayer::getLength()
{
	mpd_connection *con = mpdConnect();
	if (!con)
		return 0;

	int length = 0;
	mpd_status *status = mpd_run_status(con);
	if (status)
	{
		int position = mpd_status_get_song_pos(status);

		mpd_song *song = mpd_run_get_queue_song_pos(con, position);
		if (song)
		{
			length = mpd_song_get_duration(song);
			mpd_song_free(song);
		}
		mpd_status_free(status);
	}
	mpd_connection_free(con);

	return length * 1000;
}

int MPDMediaPlayer::getCurrentPos()
{
	mpd_connection *con = mpdConnect();
	if (!con)
		return 0;

	int pos = 0;
	mpd_status *status = mpd_run_status(con);
	if (status)
	{
		pos = mpd_status_get_elapsed_ms(status);
		mpd_status_free(status);
	}
	mpd_connection_free(con);

	return pos;
}

bool MPDMediaPlayer::isPlaying()
{
	mpd_connection *con = mpdConnect();
	if (!con)
		return false;

	mpd_status *status = mpd_run_status(con);
	if (!status)
	{
		mpd_connection_free(con);
		return false;
	}

	bool ret = (mpd_status_get_state(status) == MPD_STATE_PLAY);
	mpd_status_free(status);
	mpd_connection_free(con);

	return ret;
}

bool MPDMediaPlayer::isActive()
{
	mpd_connection *con = mpdConnect();
	if (!con)
		return false;

	bool ret = (mpd_connection_get_error(con) == MPD_ERROR_SUCCESS);
	mpd_connection_free(con);

	return ret;
}

QStringList MPDMediaPlayer::getPlayListTitles()
{
	mpd_connection *con = mpdConnect();
	if (!con)
		return QStringList();

	QStringList playlist;
	mpd_song *song;
	mpd_send_list_queue_meta(con);
	while ((song = mpd_recv_song(con)) != 0)
	{
		playlist << QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_TITLE, 0));
		mpd_song_free(song);
	}
	mpd_connection_free(con);

	return playlist;
}

QStringList MPDMediaPlayer::getPlayListFiles()
{
	mpd_connection *con = mpdConnect();
	if (!con)
		return QStringList();

	QStringList playlistFiles;
	mpd_song *song;
	mpd_send_list_queue_meta(con);
	while ((song = mpd_recv_song(con)) != NULL)
	{
		QString file = QString::fromUtf8(mpd_song_get_uri(song));
		// get only filename without directories
		file = file.right(file.length() - file.lastIndexOf('/') - 1);
		playlistFiles << file;
		mpd_song_free(song);
	}
	mpd_connection_free(con);

	return playlistFiles;
}

QString MPDMediaPlayer::getPlayerName()
{
	return QLatin1String("mpd");
}

QString MPDMediaPlayer::getPlayerVersion()
{
	QString version(tr("Unknown"));
	mpd_connection *con = mpdConnect();
	if (!con)
		return version;

	const unsigned *ver = mpd_connection_get_server_version(con);
	if (ver)
		version = QString::number(ver[0]) + '.' + QString::number(ver[1]) + '.' + QString::number(ver[2]);
	mpd_connection_free(con);

	return version;
}

void MPDMediaPlayer::nextTrack()
{
	mpd_connection *con = mpdConnect();
	if (!con)
		return;

	mpd_run_next(con);
	mpd_connection_free(con);
}

void MPDMediaPlayer::prevTrack()
{
	mpd_connection *con = mpdConnect();
	if (!con)
		return;

	mpd_run_previous(con);
	mpd_connection_free(con);
}

void MPDMediaPlayer::play()
{
	mpd_connection *con = mpdConnect();
	if (!con)
		return;

	mpd_status *status = mpd_run_status(con);
	if (status)
	{
		mpd_state state = mpd_status_get_state(status);
		if (state == MPD_STATE_PAUSE || state == MPD_STATE_STOP)
			mpd_run_play(con);

		mpd_status_free(status);
	}
	mpd_connection_free(con);
}

void MPDMediaPlayer::stop()
{
	mpd_connection *con = mpdConnect();
	if (!con)
		return;

	mpd_status *status = mpd_run_status(con);
	if (status)
	{
		mpd_state state = mpd_status_get_state(status);
		if (state == MPD_STATE_PLAY || state == MPD_STATE_PAUSE)
			mpd_run_stop(con);

		mpd_status_free(status);
	}
	mpd_connection_free(con);
}

void MPDMediaPlayer::pause()
{
	mpd_connection *con = mpdConnect();
	if (!con)
		return;

	mpd_status *status = mpd_run_status(con);
	if (status)
	{
		if (mpd_status_get_state(status) == MPD_STATE_PLAY)
			mpd_run_pause(con, true);

		mpd_status_free(status);
	}
	mpd_connection_free(con);
}

void MPDMediaPlayer::setVolume(int vol)
{
	mpd_connection *con = mpdConnect();
	if (!con)
		return;

	mpd_run_set_volume(con, vol);
	mpd_connection_free(con);
}

void MPDMediaPlayer::incrVolume()
{
	mpd_connection *con = mpdConnect();
	if (!con)
		return;

	mpd_status *status = mpd_run_status(con);
	if (status)
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

void MPDMediaPlayer::decrVolume()
{
	mpd_connection *con = mpdConnect();
	if (!con)
		return;

	mpd_status *status = mpd_run_status(con);
	if (status)
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

Q_EXPORT_PLUGIN2(mpd_mediaplayer, MPDMediaPlayer)

#include "moc_mpd-mediaplayer.cpp"
