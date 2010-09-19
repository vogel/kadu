#ifndef AMAROK_H
#define AMAROK_H

#include "../mediaplayer/player_info.h"
#include "../mediaplayer/player_commands.h"

class AmarokMediaPlayer : public PlayerCommands, public PlayerInfo
{
	Q_OBJECT

	QString getString(QString obj, QString func);
	QStringList getStringList(QString obj, QString func);
	int getInt(QString obj, QString func);
	uint getUint(QString obj, QString func);
	bool getBool(QString obj, QString func);
	void send(QString obj, QString func, int arg = -1);
	QByteArray executeCommand(QString obj, QString func);

public:
	AmarokMediaPlayer();
	~AmarokMediaPlayer();

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
