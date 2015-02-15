/*
 * %kadu copyright begin%
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

#ifndef MPRIS_MEDIAPLAYER_H
#define MPRIS_MEDIAPLAYER_H

#include "plugins/mediaplayer/player_commands.h"
#include "plugins/mediaplayer/player_info.h"

class MPRISController;

class MPRISMediaPlayer : public PlayerCommands, public PlayerInfo
{
	Q_OBJECT

	MPRISController *controller;
	QString service;
	QString name;

public:
	explicit MPRISMediaPlayer(QObject *parent = 0);
	virtual ~MPRISMediaPlayer();

	QString getTitle();
	QString getAlbum();
	QString getArtist();
	QString getFile();
	int getLength();
	int getCurrentPos();
	bool isPlaying();
	bool isActive();
	QStringList getPlayListTitles();
	QStringList getPlayListFiles();
	QString getPlayerName();
	QString getPlayerVersion();
	void setService(const QString &service);
	void setName(const QString &name);

	void nextTrack();
	void prevTrack();
	void play();
	void stop();
	void pause();
	void setVolume(int vol);
	void incrVolume();
	void decrVolume();

};

#endif // MPRIS_MEDIAPLAYER_H
