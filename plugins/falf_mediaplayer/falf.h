#ifndef FALF_H
#define FALF_H

#include <QtCore/QFile>
#include <QtCore/QObject>

#include "plugin/plugin-root-component.h"

#include "plugins/mediaplayer/player_info.h"

class FalfMediaPlayer : public QObject, PlayerInfo, PluginRootComponent
{
	Q_OBJECT
	Q_INTERFACES(PluginRootComponent)
	Q_PLUGIN_METADATA(IID "im.kadu.PluginRootComponent")

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

	virtual bool init(bool firstLoad);
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
