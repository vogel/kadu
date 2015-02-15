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

MPRISMediaPlayer::MPRISMediaPlayer(QObject *parent) :
		PlayerCommands(parent)
{
	controller = new MPRISController(service, this);

	if (name == "Audacious")
		MediaPlayer::instance()->setInterval(5);
	else
		MediaPlayer::instance()->setInterval(0);
}

MPRISMediaPlayer::~MPRISMediaPlayer()
{
}

void MPRISMediaPlayer::setService(const QString &service)
{
	this->service = service;

	if (controller)
		delete controller;

	controller = new MPRISController(this->service, this);
}

void MPRISMediaPlayer::setName(const QString &name)
{
	this->name = name;

	if (name == "Audacious")
		MediaPlayer::instance()->setInterval(5);
	else
		MediaPlayer::instance()->setInterval(0);
}

// PlayerInfo

QString MPRISMediaPlayer::getPlayerName()
{
	return name;
}

QString MPRISMediaPlayer::getPlayerVersion()
{
	if (controller)
		return controller->identity();
	else
		return QString();
}

// TrackList

QStringList MPRISMediaPlayer::getPlayListTitles()
{
	QStringList result;
	if (!isPlaying())
		return result;

	QList<TrackInfo> tracks = controller->getTrackList();

	foreach (const TrackInfo &track, tracks)
		result << track.title();

	return result;
}

QStringList MPRISMediaPlayer::getPlayListFiles()
{
	QStringList result;
	if (!isPlaying())
		return result;

	QList<TrackInfo> tracks = controller->getTrackList();

	foreach (const TrackInfo &track, tracks)
		result << track.file();

	return result;
}

QString MPRISMediaPlayer::getTitle()
{
	if (!isPlaying())
		return QString();

	return controller->track().title();
}

QString MPRISMediaPlayer::getAlbum()
{
	if (!isPlaying())
		return QString();

	return controller->track().album();
}

QString MPRISMediaPlayer::getArtist()
{
	if (!isPlaying())
		return QString();

	return controller->track().artist();
}

QString MPRISMediaPlayer::getFile()
{
	if (!isPlaying())
		return QString();

	return controller->track().file();
}

int MPRISMediaPlayer::getLength()
{
	if (!isPlaying())
		return 0;

	return controller->track().length();
}

// Player

int MPRISMediaPlayer::getCurrentPos()
{
	if (!isPlaying())
		return 0;

	return controller->getCurrentPosition();
}

void MPRISMediaPlayer::nextTrack()
{
	if (controller)
		controller->call("Next");
}

void MPRISMediaPlayer::prevTrack()
{
	if (controller)
		controller->call("Previous");
}

void MPRISMediaPlayer::play()
{
	if (controller)
		controller->call("Play");
}

void MPRISMediaPlayer::stop()
{
	if (controller)
		controller->call("Stop");
}

void MPRISMediaPlayer::pause()
{
	if (controller)
		controller->call("Pause");
}

void MPRISMediaPlayer::setVolume(int vol)
{
	if (controller)
		controller->setVolume(vol);
}

void MPRISMediaPlayer::incrVolume()
{
	if (!controller)
		return;

	int vol = controller->getVolume();
	if (vol < 100)
		vol += 2;

	if (vol > 100)
		vol = 100;

	controller->setVolume(vol);
}

void MPRISMediaPlayer::decrVolume()
{
	if (!controller)
		return;

	int vol = controller->getVolume();
	if (vol > 0)
		vol -= 2;

	if (vol < 0)
		vol = 0;

	controller->setVolume(vol);
}

bool MPRISMediaPlayer::isPlaying()
{
	if (!controller)
		return false;

	return MPRISController::StatusPlaying == controller->status();
}

bool MPRISMediaPlayer::isActive()
{
	if (!controller)
		return false;

	return controller->active();
}

#include "moc_mpris_mediaplayer.cpp"
