#ifndef FALF_H
#define FALF_H

#include <QtCore/QFile>
#include <QtCore/QObject>

#include "plugins/mediaplayer/player_info.h"
#include "plugins/mediaplayer/player_commands.h"

class FalfMediaPlayer : public PlayerInfo
{
	enum DataType
	{
		TypeTitle,
		TypeAlbum,
		TypeArtist,
		TypeAny,
		TypeVersion
	};

	QFile InfoFile;
	QString getData(DataType type);

public:
	FalfMediaPlayer();
	virtual ~FalfMediaPlayer();

	virtual QString getTitle(int position = -1);
	virtual QString getAlbum(int position = -1);
	virtual QString getArtist(int position = -1);
	virtual QString getFile(int position = -1);
	virtual int getLength(int position = -1);
	virtual int getCurrentPos();
	virtual bool isPlaying();
	virtual bool isActive();
	virtual QStringList getPlayListTitles();
	virtual QStringList getPlayListFiles();
	virtual uint getPlayListLength();
	virtual QString getPlayerName();
	virtual QString getPlayerVersion();

};

#endif // FALF_H
