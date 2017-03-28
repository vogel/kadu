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

#include "prev-action.h"

#include "mediaplayer.h"

#include "actions/actions.h"

PrevAction::PrevAction(QObject *parent)
        :   // using C++ initializers breaks Qt's lupdate
          ActionDescription(parent)
{
    setIcon(KaduIcon{"external_modules/mediaplayer-media-skip-backward"});
    setName(QStringLiteral("mediaplayer_prev"));
    setText(tr("Previous Track"));
    setType(ActionDescription::TypeChat);
}

PrevAction::~PrevAction()
{
}

void PrevAction::setMediaPlayer(MediaPlayer *mediaPlayer)
{
    m_mediaPlayer = mediaPlayer;
}

void PrevAction::actionTriggered(QAction *, bool)
{
    m_mediaPlayer->prevTrack();
}

#include "moc_prev-action.cpp"
