#ifndef AMAROK_H
#define AMAROK_H

#include "plugins/generic-plugin.h"

#include "plugins/mediaplayer/player_commands.h"
#include "plugins/mediaplayer/player_info.h"

class AmarokMediaPlayer : public PlayerCommands, PlayerInfo, GenericPlugin
{
	Q_OBJECT
	Q_INTERFACES(GenericPlugin)

	QString getString(const QString &obj, const QString &func);
	QStringList getStringList(const QString &obj, const QString &func);
	int getInt(const QString &obj, const QString &func);
	uint getUint(const QString &obj, const QString &func);
	bool getBool(const QString &obj, const QString &func);
	void send(const QString &obj, const QString &func, int arg = -1);
	QByteArray executeCommand(const QString &obj, const QString &func);

public:
	explicit AmarokMediaPlayer(QObject *parent = 0);
	virtual ~AmarokMediaPlayer();

	virtual int init(bool firstLoad);
	virtual void done();

	// PlayerInfo implementation
	virtual QString getTitle();
	virtual QString getAlbum();
	virtual QString getArtist();
	virtual QString getFile();
	virtual int getLength();
	virtual int getCurrentPos();
	virtual bool isPlaying();
	virtual bool isActive();
	virtual QStringList getPlayListTitles();
	virtual QStringList getPlayListFiles();
	virtual QString getPlayerName();
	virtual QString getPlayerVersion();

	// PlayerCommands implementation
	virtual void nextTrack();
	virtual void prevTrack();
	virtual void play();
	virtual void stop();
	virtual void pause();
	virtual void setVolume(int vol);
	virtual void incrVolume();
	virtual void decrVolume();

};

#endif // AMAROK_H
