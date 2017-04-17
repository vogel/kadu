/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "play-action.h"
#include "play-action.moc"

#include "mediaplayer.h"

#include "actions/actions.h"

PlayAction::PlayAction(QObject *parent)
        :   // using C++ initializers breaks Qt's lupdate
          ActionDescription(parent)
{
    setIcon(KaduIcon{"external_modules/mediaplayer-media-playback-play"});
    setName(QStringLiteral("mediaplayer_play"));
    setText(tr("Play"));
    setType(ActionDescription::TypeChat);
}

PlayAction::~PlayAction()
{
}

void PlayAction::setMediaPlayer(MediaPlayer *mediaPlayer)
{
    m_mediaPlayer = mediaPlayer;
}

void PlayAction::actionTriggered(QAction *, bool)
{
    m_mediaPlayer->playPause();
}
