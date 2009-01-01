#ifndef FALF_H
#define FALF_H

#include <QtCore/QFile>
#include <QtCore/QObject>

#include "../mediaplayer/player_info.h"
#include "../mediaplayer/player_commands.h"

class FalfMediaPlayer : public PlayerInfo
{
	public:
		FalfMediaPlayer();
		~FalfMediaPlayer();

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

	private:
		QFile infoFile;
		enum dataType {TITLE, ALBUM, ARTIST, ANY, VER};
		QString getData(dataType);
};

#endif
