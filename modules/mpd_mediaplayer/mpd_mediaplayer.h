#ifndef MPD_MEDIAPLAYER
#define MPD_MEDIAPLAYER

#include <QtCore/QObject>
#include <mpd/client.h>

#include "../mediaplayer/player_info.h"
#include "../mediaplayer/player_commands.h"
#include "mpd_config.h"

class MPDMediaPlayer : public PlayerInfo, public PlayerCommands
{
	public:
		MPDMediaPlayer();
		~MPDMediaPlayer();

		mpd_connection* mpdConnect();

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
	
	private:
		MPDConfig *config;

};

extern MPDMediaPlayer* mpd;
#endif
