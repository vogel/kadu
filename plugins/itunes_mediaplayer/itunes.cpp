/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtCore/QProcess>
#include <QtCore/QDateTime>

#include "debug.h"
#include "plugins/mediaplayer/mediaplayer.h"

#include "itunes.h"
#include "itunescontroller.h"

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
	return executeCommand("tell application \"iTunes\" to version");
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
	QByteArray reply = executeCommand("tell application \"iTunes\" to get duration of current playlist");
	return reply.toInt();
}

QString ITunesMediaPlayer::getTitle(int position)
{
	Q_UNUSED(position)
	kdebugf();
	Tune t = controller->currentTune();
	return t.name();
}

QString ITunesMediaPlayer::getAlbum(int position)
{
	Q_UNUSED(position)
	kdebugf();
	Tune t = controller->currentTune();
	return t.album();
}

QString ITunesMediaPlayer::getArtist(int position)
{
	Q_UNUSED(position)
	kdebugf();
	Tune t = controller->currentTune();
	return t.artist();
}

QString ITunesMediaPlayer::getFile(int position)
{
	Q_UNUSED(position)
	kdebugf();
	Tune t = controller->currentTune();
	return t.location();
}

int ITunesMediaPlayer::getLength(int position)
{
	Q_UNUSED(position)
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
	executeCommand("tell application \"iTunes\" to next track");
}

void ITunesMediaPlayer::prevTrack()
{
	kdebugf();
	executeCommand("tell application \"iTunes\" to previous track");
}

void ITunesMediaPlayer::play()
{
	kdebugf();
	executeCommand("tell application \"iTunes\" to play");
}

void ITunesMediaPlayer::stop()
{
	kdebugf();
	executeCommand("tell application \"iTunes\" to stop");
}

void ITunesMediaPlayer::pause()
{
	kdebugf();
	executeCommand("tell application \"iTunes\" to pause");
}

void ITunesMediaPlayer::setVolume(int vol)
{
	kdebugf();
	executeCommand(QString("tell application \"iTunes\" to set sound volume to %1").arg(vol));
}

int ITunesMediaPlayer::getVolume()
{
	kdebugf();
	QByteArray reply = executeCommand("tell application \"iTunes\" to get sound volume");
	return reply.toInt();
}

void ITunesMediaPlayer::incrVolume()
{
	kdebugf();
	int vol = getVolume();
	if (vol < 98)
		vol += 2;
	setVolume(vol);
}

void ITunesMediaPlayer::decrVolume()
{
	kdebugf();
	int vol = getVolume();
	if (vol > 2)
		vol -= 2;
	setVolume(vol);
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

QByteArray ITunesMediaPlayer::executeCommand(const QString &command)
{
	QByteArray result;
	QStringList params;
	QProcess process;

	params << "-e" << '\'' + command + '\'';
	process.start("osascript", params);

	if (!process.waitForStarted(500))
		return result;

	if (!process.waitForFinished())
		return result;

	result = process.readAll();

	kdebugmf(KDEBUG_INFO,"command: osascript -e %s - result: [%s]\n",
		qPrintable(command), result.constData());

	return result;
}
