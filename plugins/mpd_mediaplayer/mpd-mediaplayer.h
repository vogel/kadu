/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 badboy (badboy@gen2.org)
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

#ifndef MPD_MEDIAPLAYER
#define MPD_MEDIAPLAYER

#include <mpd/client.h>

#include "plugins/generic-plugin.h"

#include "plugins/mediaplayer/player_info.h"
#include "plugins/mediaplayer/player_commands.h"

#include "mpd-config.h"

class MPDMediaPlayer : public PlayerCommands, PlayerInfo, GenericPlugin
{
	Q_OBJECT
	Q_INTERFACES(GenericPlugin)

	MPDConfig Config;

public:
	explicit MPDMediaPlayer(QObject *parent = 0);
	virtual ~MPDMediaPlayer();

	virtual int init(bool firstLoad);
	virtual void done();

	mpd_connection * mpdConnect();

	// PlayerInfo implementation
	virtual QString getTitle(int position = -1);
	virtual QString getAlbum(int position = -1);
	virtual QString getArtist(int position = -1);
	virtual QString getFile(int position = -1);
	virtual int getLength(int position = -1);
	virtual int getCurrentPos();
	virtual bool isPlaying();
	virtual bool isActive();
	virtual QStringList getPlayListTitles();
	virtual QStringList getPlayListFiles();
	virtual uint getPlayListLength();
	virtual QString getPlayerName();
	virtual QString getPlayerVersion();

	// PlayerCommands implementation
	virtual void nextTrack();
	virtual void prevTrack();
	virtual void play();
	virtual void stop();
	virtual void pause();
	virtual void setVolume(int vol);
	virtual void incrVolume();
	virtual void decrVolume();

};

#endif // MPD_MEDIAPLAYER
