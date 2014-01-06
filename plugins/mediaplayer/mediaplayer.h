#pragma once

#include "gui/windows/main-configuration-window.h"

#include "mediaplayer_exports.h"

class QAction;
class QMenu;
class QTimer;

class ActionDescription;
class ChatWidget;
class ChatWidgetRepository;
class CustomInput;
class PlayerCommands;
class PlayerInfo;
class ToolBar;
class ToolButton;
class UserGroup;

class MediaPlayerStatusChanger;

class MEDIAPLAYERAPI MediaPlayer : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT

	static MediaPlayer *Instance;

	QPointer<ChatWidgetRepository> m_chatWidgetRepository;

	MediaPlayerStatusChanger *Changer;
	PlayerInfo *playerInfo;
	PlayerCommands *playerCommands;

	ActionDescription *enableMediaPlayerStatuses;
	ActionDescription *mediaPlayerMenu;
	ActionDescription *playAction, *stopAction, *prevAction, *nextAction, *volUpAction, *volDownAction;

	QAction *DockedMediaplayerStatus;

	QTimer *timer;
	int statusInterval;
	QString currentTitle;
	QMenu *menu;
	QAction *popups[6];
	bool winKeyPressed; // TODO: this is lame, make it good ;)
	QMap<ChatWidget *, QPushButton *> chatButtons;

	bool isPaused;

	MediaPlayer();
	virtual ~MediaPlayer();

	int pos();

	/**
		Returns Chat window, which currenly has a focus.
	*/
	ChatWidget *getCurrentChat();

	/**
		Returns true if playerInfo is supported by 3rd party
		player module which is currently loaded (if any).
	*/
	bool playerInfoSupported();

	/**
		Returns true if playerCommands is supported by 3rd party
		player module which is currently loaded (if any).
	*/
	bool playerCommandsSupported();

	void setControlsEnabled(bool enabled);

	void createDefaultConfiguration();

private slots:
	// Proxy methods for 3rd party modules
	void nextTrack();
	void prevTrack();
	void play();
	void stop();
	void pause();
	void setVolume(int vol);
	void incrVolume();
	void decrVolume();
	void playPause();
	QString getPlayerName();
	QString getPlayerVersion();
	QString getTitle();
	QString getAlbum();
	QString getArtist();
	/*
		Returns song's filename
	*/
	QString getFile();

	/*
	    Returns song's length in milliseconds
	 */
	int getLength();

	/*
	    Returns song's current position in milliseconds from the beginning
	*/
	int getCurrentPos();

	bool isPlaying();
	bool isActive();
	QStringList getPlayListTitles();
	QStringList getPlayListFiles();

	/*
		Applies all needed functions to newly created Chat window.
	*/
	void chatWidgetAdded(ChatWidget *);

	/*
		Removes all needed functions from Chat window being destroyed.
	*/
	void chatWidgetRemoved(ChatWidget *);

	void checkTitle();
	void chatKeyPressed(QKeyEvent *, CustomInput *, bool &);
	void chatKeyReleased(QKeyEvent *, CustomInput *, bool &);

	void mediaPlayerStatusChangerActivated(QAction *sender, bool toggled);
	void mediaPlayerMenuActivated(QAction *sender, bool toggled);
	void toggleStatuses(bool toggled);

protected:
	void configurationUpdated();

public:
	static void createInstance();
	static void destroyInstance();

	static MediaPlayer * instance() { return Instance; }

	void setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository);

	/*
		Looks for special tags in string 'str' and replaces it by
		assigned values. Then returns string in new form.

		Tag:		Replaces by:
		%t			Song title
		%a			Album title
		%r			Artist
		%f			File name
		%l			Length in format MM:SS
		%c			Current playing position in format MM:SS
		%p			Current playing position in percents
		%d			Current user description (if any)
		%n			Player name
		%v			Player version.
	*/
	QString parse(const QString &str);

	/*
		Returns formatted string of track length, which should be
		value returned by getLength() or getCurrentPosition() method.
	*/
	QString formatLength(int length);

	/*
		3rd party player module has to call this method in its constructor to register itself
		in MediaPlayer module so everything work.

		If one of PlayweInfo or PlayerCommands isn't supported by 3rd party module,
		just pass null (0) value for argument in this method.

		The method returns true if registering has successed, or false if there is already
		other module registered.
	*/
	bool registerMediaPlayer(PlayerInfo *info, PlayerCommands *cmds);

	/*
		3rd party player module has to call this method in its destructor to unregister itself
		from MediaPlayer module so other module can be registered.
	*/
	void unregisterMediaPlayer();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

	void titleChanged();
	void statusChanged();
	void setInterval(int seconds);

	/**
		Puts song title into current chat edit field.
	*/
	void putSongTitle(int);

	/**
		Puts whole playlist into current chat edit field.
	*/
	void putPlayList(int);

public slots:
	/**
	    Helper slots
	*/
	void insertFormattedSong();
	void insertSongTitle();
	void insertSongFilename();
	void insertPlaylistTitles();
	void insertPlaylistFilenames();

};
