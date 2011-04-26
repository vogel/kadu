#ifndef MPRIS_MEDIAPLAYER_H
#define MPRIS_MEDIAPLAYER_H

#include <QtCore/QObject>

#include "player_info.h"
#include "player_commands.h"
#include "mpris_controller.h"

class MPRISMediaPlayer : public PlayerCommands, public PlayerInfo
{
	Q_OBJECT

	QString getString(QString obj, QString func);
	int getInt(QString obj, QString func);
	QString getStringMapValue(QString obj, QString func, int param, QString field);
	int getIntMapValue(QString obj, QString func, int param, QString field);
	void send(QString obj, QString func, int arg = -1);
	MPRISController *controller;
	QString service;
	QString name;

public:
	MPRISMediaPlayer(QString n, QString s);
	~MPRISMediaPlayer();

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

#endif
