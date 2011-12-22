#ifndef MEDIA_PLAYER_INFO_H
#define MEDIA_PLAYER_INFO_H

#include <QtCore/QStringList>

#include "mediaplayer_exports.h"

class MEDIAPLAYERAPI PlayerInfo
{

public:
	PlayerInfo() {}
	virtual ~PlayerInfo() {}

	/**
		Returns name of player that this class is implemented for. For example:
		if PlayerInfo class is implemented for XMMS player, the module should
		return "XMMS" string, etc. This string is will be used on user interface.
	*/
	virtual QString getPlayerName() = 0;

 	/**
		Returns version of player that this class is implemented for.
	*/
	virtual QString getPlayerVersion() = 0;

	/**
		Returns song title (author and title) as string.
		Optional argument 'position' defines media player playlist
		entry (if supported), which title should be returned of.
		Value -1 of 'position' means current playing track.
	*/
	virtual QString getTitle() = 0;

	/**
		Returns album title as string.
	*/
	virtual QString getAlbum() = 0;

	/**
		Returns artist as string.
	*/
	virtual QString getArtist() = 0;

	/**
		Returns song file name as string.
		Optional argument 'position' similar as in
		getTitle() method.
	*/
	virtual QString getFile() = 0;

	/**
		Returns track length in miliseconds.
		Optional argument 'position' similar as in
		getTitle() method.
	*/
	virtual int getLength() = 0;

	/**
		Returns current position of played track in miliseconds.
	*/
	virtual int getCurrentPos() = 0;

	/**
		Returns true if media player is currently playing, or false otherwise.
	*/
	virtual bool isPlaying() = 0;

	/**
		Returns true if media player is active (run), or false otherwise.
	*/
	virtual bool isActive() = 0;

	/**
		Returns list of titles in playlist.
	*/
	virtual QStringList getPlayListTitles() = 0;

	/**
		Returns list of file names of entries in playlist.
	*/
	virtual QStringList getPlayListFiles() = 0;

};

#endif

