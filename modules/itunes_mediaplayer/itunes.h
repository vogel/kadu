#ifndef ITUNES_H
#define ITUNES_H

#include <QtCore/QObject>

#include "../mediaplayer/player_info.h"
#include "../mediaplayer/player_commands.h"

class ITunesController;

class ITunesMediaPlayer : public PlayerInfo, public PlayerCommands
{

	ITunesController *controller;

public:
	ITunesMediaPlayer();
	~ITunesMediaPlayer();

	QString getTitle(int position = -1);
	QString getAlbum(int position = -1);
	QString getArtist(int position = -1);
	QString getFile(int position = -1);
	int getLength(int position = -1);
	int getCurrentPos();
	bool isPlaying();
	bool isActive();
	QStringList getPlayListTitles();
	QStringList getPlayListFiles();
	uint getPlayListLength();
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
