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

#include "next-action.h"

#include "mediaplayer.h"

NextAction::NextAction(QObject *parent)
        :   // using C++ initializers breaks Qt's lupdate
          ActionDescription(parent)
{
    setIcon(KaduIcon{"external_modules/mediaplayer-media-skip-forward"});
    setName(QStringLiteral("mediaplayer_next"));
    setText(tr("Next Track"));
    setType(ActionDescription::TypeChat);
}

NextAction::~NextAction()
{
}

void NextAction::setMediaPlayer(MediaPlayer *mediaPlayer)
{
    m_mediaPlayer = mediaPlayer;
}

void NextAction::actionTriggered(QAction *, bool)
{
    m_mediaPlayer->nextTrack();
}

#include "moc_next-action.cpp"
