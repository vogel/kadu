/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Tomasz Rostański (rozteck@interia.pl)
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

#include "plugins/mediaplayer/mediaplayer.h"

#include "amarok.h"

AmarokMediaPlayer::AmarokMediaPlayer(QObject *parent) :
		PlayerCommands(parent)
{
	kdebugf();
}

AmarokMediaPlayer::~AmarokMediaPlayer()
{
	kdebugf();
}

int AmarokMediaPlayer::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	bool res = MediaPlayer::instance()->registerMediaPlayer(this, this);
	if (!res)
		return 1;

	MediaPlayer::instance()->setInterval(5);
	return 0;
}

void AmarokMediaPlayer::done()
{
	MediaPlayer::instance()->unregisterMediaPlayer();
}

QByteArray AmarokMediaPlayer::executeCommand(const QString &obj, const QString &func)
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
			qPrintable(obj), qPrintable(func), result.constData());

	return result;
}

QString AmarokMediaPlayer::getString(const QString &obj, const QString &func)
{
	if (!isActive())
		return QString();

	return QString(executeCommand(obj, func)).simplified();
}

QStringList AmarokMediaPlayer::getStringList(const QString &obj, const QString &func)
{
	if (!isActive())
		return QStringList();

	return QString(executeCommand(obj, func)).split('\n');
}

int AmarokMediaPlayer::getInt(const QString &obj, const QString &func)
{
	if (!isActive())
		return 0;

	return getString(obj, func).toInt();
}

uint AmarokMediaPlayer::getUint(const QString &obj, const QString &func)
{
	if (!isActive())
		return 0;

	return getString(obj, func).toUInt();
}

bool AmarokMediaPlayer::getBool(const QString &obj, const QString &func)
{
	if (!isActive())
		return 0;

	return (getString(obj, func) == QLatin1String("true"));
}


void AmarokMediaPlayer::send(const QString &obj, const QString &func, int arg)
{
	if (!isActive())
		return;

	QString func2(func);
	if (arg != -1)
	{
		func2.append(' ');
		func2.append(QString::number(arg));
	}

	executeCommand(obj, func2);
}

// PlayerInfo

QString AmarokMediaPlayer::getPlayerName()
{
	return QLatin1String("amaroK");
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
	QString ret(QString(executeCommand("player", "isPlaying")).simplified());
	kdebugf2();

	return (ret == QLatin1String("true")) || (ret == QLatin1String("false"));
}

Q_EXPORT_PLUGIN2(amarok1_mediaplayer, AmarokMediaPlayer)
