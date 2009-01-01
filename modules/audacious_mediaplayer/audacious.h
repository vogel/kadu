#ifndef AUDACIOUS_H
#define AUDACIOUS_H

#include <QtCore/QObject>

#include "../mediaplayer/player_info.h"
#include "../mediaplayer/player_commands.h"

class AudaciousMediaPlayer : public PlayerInfo, public PlayerCommands
{
	static QString convert(QString str);

public:
	AudaciousMediaPlayer();
	~AudaciousMediaPlayer();

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
