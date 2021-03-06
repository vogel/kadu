/*
 * %kadu copyright begin%
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#pragma once

#include <injeqt/injeqt.h>
#include <mpd/client.h>

#include "plugins/mediaplayer/player-commands.h"
#include "plugins/mediaplayer/player-info.h"

#include "mpd-config.h"

class Configuration;

class MPDMediaPlayer : public QObject, public PlayerCommands, public PlayerInfo
{
    Q_OBJECT

    QPointer<Configuration> m_configuration;

    MPDConfig Config;

private slots:
    INJEQT_SET void setConfiguration(Configuration *configuration);
    INJEQT_INIT void init();

public:
    Q_INVOKABLE explicit MPDMediaPlayer(QObject *parent = nullptr);
    virtual ~MPDMediaPlayer();

    mpd_connection *mpdConnect();

    // PlayerInfo implementation
    virtual QString getTitle();
    virtual QString getAlbum();
    virtual QString getArtist();
    virtual QString getFile();
    virtual int getLength();
    virtual int getCurrentPos();
    virtual bool isPlaying();
    virtual bool isActive();
    virtual QStringList getPlayListTitles();
    virtual QStringList getPlayListFiles();
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
