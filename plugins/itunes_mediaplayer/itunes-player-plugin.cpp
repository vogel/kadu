/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "plugin/mediaplayer/mediaplayer.h"

#include "itunes-player-plugin.h"
#include "itunes.h"

ItunesMediaplayerPlugin::~ItunesMediaplayerPlugin()
{

}

bool ItunesMediaplayerPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	iTunes = new ITunesMediaPlayer();
	return MediaPlayer::instance()->registerMediaPlayer(iTunes, iTunes);
}

void ItunesMediaplayerPlugin::done()
{
	MediaPlayer::instance()->unregisterMediaPlayer();
	delete iTunes;
}

Q_EXPORT_PLUGIN2(itunes_mediaplayer, ItunesMediaplayerPlugin)

#include "moc_itunes-player-plugin.cpp"
