#ifndef ITUNES_H
#define ITUNES_H

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QByteArray>

#include "plugins/mediaplayer/player_info.h"
#include "plugins/mediaplayer/player_commands.h"

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
