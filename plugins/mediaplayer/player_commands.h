#ifndef MEDIA_PLAYER_CMDS_H
#define MEDIA_PLAYER_CMDS_H

#include <QtCore/QObject>

#include "mediaplayer_exports.h"

class MEDIAPLAYERAPI PlayerCommands : public QObject
{
	Q_OBJECT

public:
	explicit PlayerCommands(QObject *parent = 0) : QObject(parent) {}
	virtual ~PlayerCommands() {}

public slots:
	/**
		Orders media player to play the next one track.
	*/
	virtual void nextTrack() = 0;

	/**
		Orders media player to play the previous one track.
	*/
	virtual void prevTrack() = 0;

	/**
		Orders media player to start playing.
	*/
	virtual void play() = 0;

	/**
		Orders media player to stop playing.
	*/
	virtual void stop() = 0;

	/**
		Orders media player to pause playing.
	*/
	virtual void pause() = 0;

	/**
		Orders media player to sset volume to 'vol' (in percents).
	*/
	virtual void setVolume(int vol) = 0;

	/**
		Orders media player to increment volume.
	*/
	virtual void incrVolume() = 0;

	/**
		Orders media player to decrement volume.
	*/
	virtual void decrVolume() = 0;

};

#endif

