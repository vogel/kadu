#ifndef AMAROK2_H
#define AMAROK2_H

#include <QtCore/QObject>

#include "../mediaplayer/player_info.h"
#include "../mediaplayer/player_commands.h"
#include "amarok2controller.h"

#define SERVICE "org.kde.amarok"

class Amarok2MediaPlayer : public PlayerInfo, public PlayerCommands
{
	QString getString(QString obj, QString func);
	int getInt(QString obj, QString func);
	void send(QString obj, QString func, int arg = -1);
	Amarok2Controller *controller;

public:
	Amarok2MediaPlayer();
	~Amarok2MediaPlayer();

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
