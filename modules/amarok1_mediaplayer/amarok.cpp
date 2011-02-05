/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

/* Dorr: As we have not dcopclient implementation for Qt4 now we're calling
 * each dcop command using QProcess. I know it's not ideal solution but
 * works ;)
 */

#include <QtCore/QProcess>
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QStringList>

#include "debug.h"
#include "exports.h"
#include "../mediaplayer/mediaplayer.h"
#include "amarok.h"

#define MODULE_AMAROK_MEDIAPLAYER_VERSION 2.0

AmarokMediaPlayer* amarok;

extern "C" KADU_EXPORT int amarok1_mediaplayer_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	amarok = new AmarokMediaPlayer();
	bool res = mediaplayer->registerMediaPlayer(amarok, amarok);
	mediaplayer->setInterval(5);
	return res ? 0 : 1;
}

extern "C" KADU_EXPORT void amarok1_mediaplayer_close()
{
	mediaplayer->unregisterMediaPlayer();
	delete amarok;
	amarok = NULL;
}

QByteArray AmarokMediaPlayer::executeCommand(QString obj, QString func)
{
	QByteArray result;
	QStringList params;
	QProcess process;

	params << "amarok" << obj << func;
	process.start("dcop", params);

	if (!process.waitForStarted(500))
		return result;

	if (!process.waitForFinished())
		return result;

	result = process.readAll();

	kdebugmf(KDEBUG_INFO, "command: dcop amarok %s %s - result: [%s]\n",
		qPrintable(obj), qPrintable(func), qPrintable(QString(result)));

	return result;
}

AmarokMediaPlayer::AmarokMediaPlayer()
{
	kdebugf();
}

AmarokMediaPlayer::~AmarokMediaPlayer()
{
	kdebugf();
}

QString AmarokMediaPlayer::getString(QString obj, QString func)
{
	if (!isActive())
		return "";

	QByteArray reply = executeCommand(obj, func);
	QString str(reply);
	return str.simplified();
}

QStringList AmarokMediaPlayer::getStringList(QString obj, QString func)
{
	if (!isActive())
		return QStringList();

	QByteArray reply = executeCommand(obj, func);
	return QString(reply).split('\n');
}

int AmarokMediaPlayer::getInt(QString obj, QString func)
{
	if (!isActive())
		return 0;

	QString reply = getString(obj, func);
	return reply.toInt();
}

uint AmarokMediaPlayer::getUint(QString obj, QString func)
{
	if (!isActive())
		return 0;

	QString reply = getString(obj, func);
	return reply.toUInt();
}

bool AmarokMediaPlayer::getBool(QString obj, QString func)
{
	if (!isActive())
		return 0;

	QString ret = getString(obj, func);
	return (ret == "true") ? true : false;
}


void AmarokMediaPlayer::send(QString obj, QString func, int arg)
{
	if (!isActive())
		return;

	if (arg != -1)
	{
		func.append(" ");
		func.append(QString::number(arg));
	}
	QByteArray reply = executeCommand(obj, func);
}

// PlayerInfo

QString AmarokMediaPlayer::getPlayerName()
{
	return "amaroK";
}

QString AmarokMediaPlayer::getPlayerVersion()
{
	kdebugf();
	return getString("player", "version");
	kdebugf2();
}

QStringList AmarokMediaPlayer::getPlayListTitles()
{
	kdebugf();
	// No API for titles list, only file names.
	return getStringList("playlist", "filenames");
	kdebugf2();
}

QStringList AmarokMediaPlayer::getPlayListFiles()
{
	kdebugf();
	return getStringList("playlist", "filenames");
	kdebugf2();
}

uint AmarokMediaPlayer::getPlayListLength()
{
	kdebugf();
	return getUint("playlist", "getTotalTrackCount");
	kdebugf2();
}

QString AmarokMediaPlayer::getTitle(int position)
{
	Q_UNUSED(position)

	kdebugf();
	return getString("player", "title");
	kdebugf2();
}

QString AmarokMediaPlayer::getAlbum(int position)
{
	Q_UNUSED(position)

	kdebugf();
	return getString("player", "album");
	kdebugf2();
}

QString AmarokMediaPlayer::getArtist(int position)
{
	Q_UNUSED(position)

	kdebugf();
	return getString("player", "artist");
	kdebugf2();
}

QString AmarokMediaPlayer::getFile(int position)
{
	Q_UNUSED(position)

	kdebugf();
	return getString("player", "path");
	kdebugf2();
}

int AmarokMediaPlayer::getLength(int position)
{
	Q_UNUSED(position)

	kdebugf();
	return getInt("player", "trackTotalTime") * 1000;
	kdebugf2();
}

int AmarokMediaPlayer::getCurrentPos()
{
	kdebugf();
	return getInt("player", "trackCurrentTime") * 1000;
	kdebugf2();
}

// PlayerCommands

void AmarokMediaPlayer::nextTrack()
{
	kdebugf();
	send("player", "next");
	kdebugf2();
}

void AmarokMediaPlayer::prevTrack()
{
	kdebugf();
	send("player", "prev");
	kdebugf2();
}

void AmarokMediaPlayer::play()
{
	kdebugf();
	send("player", "play");
	kdebugf2();
}

void AmarokMediaPlayer::stop()
{
	kdebugf();
	send("player", "stop");
	kdebugf2();
}

void AmarokMediaPlayer::pause()
{
	kdebugf();
	send("player", "pause");
	kdebugf2();
}

void AmarokMediaPlayer::setVolume(int vol)
{
	kdebugf();
	send("player", "setVolume", vol);
	kdebugf2();
}

void AmarokMediaPlayer::incrVolume()
{
	kdebugf();
	int vol = getInt("player", "getVolume");
	if (vol < 100)
		vol += 2;

	if (vol > 100)
		vol = 100;

	send("player", "setVolume", vol);
	kdebugf2();
}

void AmarokMediaPlayer::decrVolume()
{
	kdebugf();
	int vol = getInt("player", "getVolume");
	if (vol > 0)
		vol -= 2;

	if (vol < 0)
		vol = 0;

	send("player", "setVolume", vol);
	kdebugf2();
}

bool AmarokMediaPlayer::isPlaying()
{
	kdebugf();
	return getBool("player", "isPlaying");
	kdebugf2();
}

bool AmarokMediaPlayer::isActive()
{
	kdebugf();

	QByteArray reply = executeCommand("player", "isPlaying");
	QString ret(reply);
	ret = ret.simplified();

	return ((ret == "true") || (ret == "false")) ? true : false;
	kdebugf2();
}
