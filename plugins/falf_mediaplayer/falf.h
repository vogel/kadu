#ifndef FALF_H
#define FALF_H

#include <QtCore/QFile>
#include <QtCore/QObject>

#include "plugins/generic-plugin.h"

#include "plugins/mediaplayer/player_info.h"

class FalfMediaPlayer : public QObject, PlayerInfo, GenericPlugin
{
	Q_OBJECT
	Q_INTERFACES(GenericPlugin)

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
	explicit FalfMediaPlayer(QObject *parent = 0);
	virtual ~FalfMediaPlayer();

	virtual int init(bool firstLoad);
	virtual void done();

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

};

#endif // FALF_H
