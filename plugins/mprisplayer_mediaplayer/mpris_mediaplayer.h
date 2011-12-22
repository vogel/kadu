#ifndef MPRIS_MEDIAPLAYER_H
#define MPRIS_MEDIAPLAYER_H

#include <QtCore/QObject>

#include "plugins/mediaplayer/player_commands.h"
#include "plugins/mediaplayer/player_info.h"

#include "mpris_controller.h"

class MPRISMediaPlayer : public PlayerCommands, public PlayerInfo
{
	Q_OBJECT

	MPRISController *controller;
	QString service;
	QString name;

	QString getString(QString obj, QString func);
	int getInt(QString obj, QString func);
	int getIntMapValue(QString obj, QString func, int param, QString field);
	void send(QString obj, QString func, int arg = -1);

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

#endif
