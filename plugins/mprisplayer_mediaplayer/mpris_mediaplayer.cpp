/*
 * %kadu copyright begin%
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

/* Dorr: Amarok2 implements the MPRIS standard so this module should work
 * also with Audacious, BMPx, DragonPLayer, VLC, XMMS2 and maybe others.
 *
 * See http://mpris.org/ for more details about the standard
 */

#include <QtCore/QStringList>

#include "plugins/mediaplayer/mediaplayer.h"

#include "mpris_controller.h"

#include "mpris_mediaplayer.h"
#include "mpris_mediaplayer.moc"

MPRISMediaPlayer::MPRISMediaPlayer(QObject *parent) : QObject(parent)
{
}

MPRISMediaPlayer::~MPRISMediaPlayer()
{
}

void MPRISMediaPlayer::setMediaPlayer(MediaPlayer *mediaPlayer)
{
    m_mediaPlayer = mediaPlayer;
}

void MPRISMediaPlayer::setService(const QString &service)
{
    this->service = service;

    if (m_controller)
        delete m_controller;

    m_controller = new MPRISController(m_mediaPlayer, this->service, this);
}

void MPRISMediaPlayer::setName(const QString &name)
{
    this->name = name;
}

// PlayerInfo

QString MPRISMediaPlayer::getPlayerName()
{
    return name;
}

QString MPRISMediaPlayer::getPlayerVersion()
{
    if (m_controller)
        return m_controller->identity();
    else
        return QString();
}

// TrackList

QStringList MPRISMediaPlayer::getPlayListTitles()
{
    QStringList result;
    if (!isPlaying())
        return result;

    QList<TrackInfo> tracks = m_controller ? m_controller->getTrackList() : QList<TrackInfo>();

    for (auto const &track : tracks)
        result << track.title();

    return result;
}

QStringList MPRISMediaPlayer::getPlayListFiles()
{
    QStringList result;
    if (!isPlaying())
        return result;

    QList<TrackInfo> tracks = m_controller ? m_controller->getTrackList() : QList<TrackInfo>();

    for (auto const &track : tracks)
        result << track.file();

    return result;
}

QString MPRISMediaPlayer::getTitle()
{
    if (!isPlaying())
        return QString();

    return m_controller->track().title();
}

QString MPRISMediaPlayer::getAlbum()
{
    if (!isPlaying())
        return QString();

    return m_controller->track().album();
}

QString MPRISMediaPlayer::getArtist()
{
    if (!isPlaying())
        return QString();

    return m_controller->track().artist();
}

QString MPRISMediaPlayer::getFile()
{
    if (!isPlaying())
        return QString();

    return m_controller->track().file();
}

int MPRISMediaPlayer::getLength()
{
    if (!isPlaying())
        return 0;

    return m_controller->track().length();
}

// Player

int MPRISMediaPlayer::getCurrentPos()
{
    if (!isPlaying())
        return 0;

    return m_controller->getCurrentPosition();
}

void MPRISMediaPlayer::nextTrack()
{
    if (m_controller)
        m_controller->call("Next");
}

void MPRISMediaPlayer::prevTrack()
{
    if (m_controller)
        m_controller->call("Previous");
}

void MPRISMediaPlayer::play()
{
    if (m_controller)
        m_controller->call("Play");
}

void MPRISMediaPlayer::stop()
{
    if (m_controller)
        m_controller->call("Stop");
}

void MPRISMediaPlayer::pause()
{
    if (m_controller)
        m_controller->call("Pause");
}

void MPRISMediaPlayer::setVolume(int vol)
{
    if (m_controller)
        m_controller->setVolume(vol);
}

void MPRISMediaPlayer::incrVolume()
{
    if (!m_controller)
        return;

    int vol = m_controller->getVolume();
    if (vol < 100)
        vol += 2;

    if (vol > 100)
        vol = 100;

    m_controller->setVolume(vol);
}

void MPRISMediaPlayer::decrVolume()
{
    if (!m_controller)
        return;

    int vol = m_controller->getVolume();
    if (vol > 0)
        vol -= 2;

    if (vol < 0)
        vol = 0;

    m_controller->setVolume(vol);
}

bool MPRISMediaPlayer::isPlaying()
{
    if (!m_controller)
        return false;

    return MPRISController::StatusPlaying == m_controller->status();
}

bool MPRISMediaPlayer::isActive()
{
    if (!m_controller)
        return false;

    return m_controller->active();
}
