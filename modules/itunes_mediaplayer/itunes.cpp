/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QString>

#include "debug.h"
#include "../mediaplayer/mediaplayer.h"

#include "itunes.h"
#include "itunescontroller.h"

ITunesMediaPlayer* iTunes;

extern "C" int itunes_mediaplayer_init()
{
	iTunes = new ITunesMediaPlayer();
	bool res = mediaplayer->registerMediaPlayer(iTunes, iTunes);
	return res ? 0 : 1;
}

extern "C" void itunes_mediaplayer_close()
{
	mediaplayer->unregisterMediaPlayer();
	delete iTunes;
	iTunes = NULL;
}

ITunesMediaPlayer::ITunesMediaPlayer()
{
	controller = new ITunesController();
	kdebugf();
}

ITunesMediaPlayer::~ITunesMediaPlayer()
{
	delete controller;
	controller = NULL;
	kdebugf();
}

// PlayerInfo

QString ITunesMediaPlayer::getPlayerName()
{
	return "iTunes";
}

QString ITunesMediaPlayer::getPlayerVersion()
{
	kdebugf();

	//TODO: obtain the iTunes version
	return "0.0";
}

QStringList ITunesMediaPlayer::getPlayListTitles()
{
	kdebugf();
	QStringList list;

	//TODO: obtain the playlist
	return list;
}

QStringList ITunesMediaPlayer::getPlayListFiles()
{
	kdebugf();
	QStringList list;

	//TODO: obtain the playlist
	return list;
}

uint ITunesMediaPlayer::getPlayListLength()
{
	kdebugf();
	uint num = 0;

	//TODO: obtain the playlist
	return num;
}

QString ITunesMediaPlayer::getTitle(int position)
{
	kdebugf();
	
	Tune t = controller->currentTune();
	return t.name();
}

QString ITunesMediaPlayer::getAlbum(int position)
{
	kdebugf();

	Tune t = controller->currentTune();
	return t.album();
}

QString ITunesMediaPlayer::getArtist(int position)
{
	kdebugf();

	Tune t = controller->currentTune();
	return t.artist();
}

QString ITunesMediaPlayer::getFile(int position)
{
	kdebugf();

	Tune t = controller->currentTune();
	return t.location();
}

int ITunesMediaPlayer::getLength(int position)
{
	kdebugf();
	
	Tune t = controller->currentTune();
	return t.time();
}

int ITunesMediaPlayer::getCurrentPos()
{
	kdebugf();

	Tune t = controller->currentTune();	
	return (QDateTime::currentDateTime().toTime_t() -  t.started()) * 1000;
}

// PlayerCommands

void ITunesMediaPlayer::nextTrack()
{
	kdebugf();

	//TODO: implement
}

void ITunesMediaPlayer::prevTrack()
{
	kdebugf();

	//TODO: implement
}

void ITunesMediaPlayer::play()
{
	kdebugf();

	//TODO: implement
}

void ITunesMediaPlayer::stop()
{
	kdebugf();

	//TODO: implement
}

void ITunesMediaPlayer::pause()
{
	kdebugf();

	//TODO: implement
}

void ITunesMediaPlayer::setVolume(int vol)
{
	kdebugf();

	//TODO: implement
}

void ITunesMediaPlayer::incrVolume()
{
	kdebugf();

	//TODO: implement
}

void ITunesMediaPlayer::decrVolume()
{
	kdebugf();

	//TODO: implement
}

bool ITunesMediaPlayer::isPlaying()
{
	kdebugf();

	Tune t = controller->currentTune();
	return (t.state() == Tune::playing);
}

bool ITunesMediaPlayer::isActive()
{
	kdebugf();
	
	Tune t = controller->currentTune();
	return (t.state() != Tune::unknown);
}
