/*
 * %kadu copyright begin%
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
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

#include "itunes.h"
#include "itunes-player-plugin.h"

ItunesMediaplayerPlugin::~ItunesMediaplayerPlugin()
{

}

int ItunesMediaplayerPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	iTunes = new ITunesMediaPlayer();
	bool res = MediaPlayer::instance()->registerMediaPlayer(iTunes, iTunes);
	return res ? 0 : 1;
}

void ItunesMediaplayerPlugin::done()
{
	MediaPlayer::instance()->unregisterMediaPlayer();
	delete iTunes;
}

Q_EXPORT_PLUGIN2(itunes_mediaplayer, ItunesMediaplayerPlugin)
