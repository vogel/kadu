#ifndef ITUNES_H
#define ITUNES_H

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QStringList>

#include "plugins/mediaplayer/player_commands.h"
#include "plugins/mediaplayer/player_info.h"

class ITunesController;

class ITunesMediaPlayer : public PlayerCommands, public PlayerInfo
{
	Q_OBJECT

	ITunesController *controller;
	int getVolume();
	QByteArray executeCommand(const QString &command);

public:
	ITunesMediaPlayer();
	~ITunesMediaPlayer();

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

	void nextTrack();
	void prevTrack();
	void play();
	void stop();
	void pause();
	void setVolume(int vol);
	void incrVolume();
	void decrVolume();

};

#endif
