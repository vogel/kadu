#pragma once

#include "configuration/configuration-aware-object.h"
#include "configuration/gui/configuration-ui-handler.h"

#include "mediaplayer-exports.h"

#include <QtCore/QMap>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class QAction;
class QKeyEvent;
class QMenu;
class QPushButton;
class QTimer;

class Actions;
class ChatWidget;
class ChatWidgetRepository;
class Configuration;
class CustomInput;
class DockingMenuActionRepository;
class IconsManager;
class MediaplayerNotificationService;
class MenuInventory;
class NextAction;
class PluginInjectedFactory;
class PrevAction;
class PlayAction;
class PlayerCommands;
class PlayerInfo;
class StatusChangerManager;
class StopAction;
class ToggleMediaplayerStatusesAction;
class ToolBar;
class ToolButton;
class UserGroup;
class VolumeDownAction;
class VolumeUpAction;

class MediaplayerMenuAction;
class MediaPlayerStatusChanger;

class MEDIAPLAYERAPI MediaPlayer : public QObject, ConfigurationAwareObject
{
	Q_OBJECT

	QPointer<Actions> m_actions;
	QPointer<ChatWidgetRepository> m_chatWidgetRepository;
	QPointer<Configuration> m_configuration;
	QPointer<DockingMenuActionRepository> m_dockingMenuActionRepository;
	QPointer<IconsManager> m_iconsManager;
	QPointer<MediaplayerMenuAction> m_mediaplayerMenuAction;
	QPointer<MediaplayerNotificationService> m_mediaplayerNotificationService;
	QPointer<MenuInventory> m_menuInventory;
	QPointer<NextAction> m_nextAction;
	QPointer<PlayAction> m_playAction;
	QPointer<PluginInjectedFactory> m_pluginInjectedFactory;
	QPointer<PrevAction> m_prevAction;
	QPointer<StatusChangerManager> m_statusChangerManager;
	QPointer<StopAction> m_stopAction;
	QPointer<ToggleMediaplayerStatusesAction> m_toggleMediaplayerStatusesAction;
	QPointer<VolumeDownAction> m_volumeDownAction;
	QPointer<VolumeUpAction> m_volumeUpAction;

	MediaPlayerStatusChanger *Changer;
	PlayerInfo *playerInfo;
	PlayerCommands *playerCommands;

	QAction *DockedMediaplayerStatus;

	QTimer *timer;
	int statusInterval;
	QString currentTitle;
	QMenu *m_menu;
	QAction *popups[6];
	bool winKeyPressed; // TODO: this is lame, make it good ;)
	QMap<ChatWidget *, QPushButton *> chatButtons;

	bool isPaused;

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
	INJEQT_SET void setActions(Actions *actions);
	INJEQT_SET void setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setDockingMenuActionRepository(DockingMenuActionRepository *dockingMenuActionRepository);
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_SET void setMediaplayerMenuAction(MediaplayerMenuAction *mediaplayerMenuAction);
	INJEQT_SET void setMediaplayerNotificationService(MediaplayerNotificationService *mediaplayerNotificationService);
	INJEQT_SET void setMenuInventory(MenuInventory *menuInventory);
	INJEQT_SET void setNextAction(NextAction *nextAction);
	INJEQT_SET void setPlayAction(PlayAction *playAction);
	INJEQT_SET void setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory);
	INJEQT_SET void setPrevAction(PrevAction *prevAction);
	INJEQT_SET void setStatusChangerManager(StatusChangerManager *statusChangerManager);
	INJEQT_SET void setStopAction(StopAction *stopAction);
	INJEQT_SET void setToggleMediaplayerStatusesAction(ToggleMediaplayerStatusesAction *toggleMediaplayerStatusesAction);
	INJEQT_SET void setVolumeDownAction(VolumeDownAction *volumeDownAction);
	INJEQT_SET void setVolumeUpAction(VolumeUpAction *volumeUpAction);
	INJEQT_INIT void init();
	INJEQT_DONE void done();

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

	void statusAboutToBeChanged();

protected:
	void configurationUpdated();

public:
	Q_INVOKABLE explicit MediaPlayer(QObject *parent = nullptr);
	virtual ~MediaPlayer();

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

	// Proxy methods for 3rd party plugins
	void nextTrack();
	void prevTrack();
	void play();
	void stop();
	void pause();
	void setVolume(int vol);
	void incrVolume();
	void decrVolume();
	void playPause();

	void toggleStatuses(bool toggled);
	QMenu * menu() const { return m_menu; }

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
