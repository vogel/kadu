/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2008, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QKeyEvent>
#include <QtGui/QMenu>
#include <QtGui/QTextEdit>
#include <QtGui/QToolTip>

#include "core/core.h"
#include "configuration/configuration-file.h"

#include "gui/actions/action.h"
#include "gui/actions/action-description.h"

#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/custom-input.h"
#include "gui/widgets/configuration/configuration-widget.h"

#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"

#include "misc/path-conversion.h"

#include "notify/notification-manager.h"
#include "notify/notification.h"
#include "notify/notify-event.h"

#include "status/status-changer-manager.h"

#include "debug.h"
#include "icons-manager.h"

#include "mp_status_changer.h"
#include "player_commands.h"
#include "player_info.h"

#include "mediaplayer.h"

#define MODULE_MEDIAPLAYER_VERSION 1.3
#define CHECK_STATUS_INTERVAL 1*1000 /* 1s */

#define SHORTCUT_KEY Qt::Key_Meta

const char *MediaPlayerSyntaxText = QT_TRANSLATE_NOOP
(
	"@default",
	"Syntax: %t - song title,\n%a - album, %r - artist, %f - file name,\n"
	"%l - song length (MM:SS), %c - current song position (MM:SS),\n"
	"%p - percents of played song, %n - player name, %v - player version\n"
);

#ifdef Q_OS_MAC
const char *MediaPlayerChatShortCutsText = QT_TRANSLATE_NOOP
(
	"@default",
	"With this option enabled you'll be able to control\n"
	"your MediaPlayer in chat window by keyboard shortcuts:\n"
	"Control+ Enter/Backspace/Left/Right/Up/Down."
);
#else
const char *MediaPlayerChatShortCutsText = QT_TRANSLATE_NOOP
(
	"@default",
	"With this option enabled you'll be able to control\n"
	"your MediaPlayer in chat window by keyboard shortcuts:\n"
	"Win+ Enter/Backspace/Left/Right/Up/Down."
);
#endif

// TODO: remove For CP1250->ISO8859-2 converter
const char CODE_CP1250[]    = {0xb9, 0x9c, 0x9f, 0xa5, 0x8c, 0x8f};
const char CODE_ISO8859_2[] = {0xb1, 0xb6, 0xbc, 0xa1, 0xa6, 0xac};

// For ID3 tags signatures cutter
const char DEFAULT_SIGNATURES[] = "! WWW.POLSKIE-MP3.TK ! \n! www.polskie-mp3.tk ! ";

// The main mediaplayer module object
MediaPlayer *mediaplayer;

const char *mediaPlayerOsdHint = "MediaPlayerOsd";

extern "C" KADU_EXPORT int mediaplayer_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	mediaplayer = new MediaPlayer();

	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/mediaplayer.ui"));
	MainConfigurationWindow::registerUiHandler(mediaplayer);

	return 0;
}

extern "C" KADU_EXPORT void mediaplayer_close()
{
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/mediaplayer.ui"));
	MainConfigurationWindow::unregisterUiHandler(mediaplayer);

	delete mediaplayer;
	mediaplayer = 0;
}

// Implementation of MediaPlayer class

MediaPlayer::MediaPlayer()
{
	kdebugf();

	// Initialization
	playerInfo = 0;
	playerCommands = 0;
	statusInterval = CHECK_STATUS_INTERVAL;

	// MediaPlayer menus in chats
	menu = new QMenu();
	popups[0] = menu->addAction(tr("Put formated string"), this, SLOT(insertFormattedSong()));
	popups[1] = menu->addAction(tr("Put song title"), this, SLOT(insertSongTitle()));
	popups[2] = menu->addAction(tr("Put song file name"), this, SLOT(insertSongFilename()));
	popups[3] = menu->addAction(tr("Send all playlist titles"), this, SLOT(insertPlaylistTitles()));
	popups[4] = menu->addAction(tr("Send all playlist files"), this, SLOT(insertPlaylistFilenames()));

	// Title checking timer
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(checkTitle()));

	// Monitor of creating chats
	connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetCreated(ChatWidget *)), this, SLOT(chatWidgetCreated(ChatWidget *)));
	connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetDestroying(ChatWidget *)), this, SLOT(chatWidgetDestroying(ChatWidget *)));

	foreach (ChatWidget *it, ChatWidgetManager::instance()->chats())
		chatWidgetCreated(it);

	enableMediaPlayerStatuses = new ActionDescription(
		0, ActionDescription::TypeGlobal, "enableMediaPlayerStatusesAction",
		this, SLOT(mediaPlayerStatusChangerActivated(QAction *, bool)),
		"external_modules/mediaplayer-media-playback-play", tr("Enable MediaPlayer Statuses"), true
	);
	mediaPlayerMenu = new ActionDescription(
		0, ActionDescription::TypeChat, "mediaplayer_button",
		this, SLOT(mediaPlayerMenuActivated(QAction *, bool)),
		"external_modules/mediaplayer", tr("MediaPlayer"), false
	);
	playAction = new ActionDescription(
		0, ActionDescription::TypeChat, "mediaplayer_play",
		this, SLOT(playPause()),
		"external_modules/mediaplayer-media-playback-play", tr("Play"), false
	);
	stopAction = new ActionDescription(
		0, ActionDescription::TypeChat, "mediaplayer_stop",
		this, SLOT(stop()),
		"external_modules/mediaplayer-media-playback-stop", tr("Stop"), false
	);
	prevAction = new ActionDescription(
		0, ActionDescription::TypeChat, "mediaplayer_prev",
		this, SLOT(prevTrack()),
		"external_modules/mediaplayer-media-skip-backward", tr("Previous Track"), false
	);
	nextAction = new ActionDescription(
		0, ActionDescription::TypeChat, "mediaplayer_next",
		this, SLOT(nextTrack()),
		"external_modules/mediaplayer-media-skip-forward", tr("Next Track"), false
	);
	volUpAction = new ActionDescription(
		0, ActionDescription::TypeChat, "mediaplayer_vol_up",
		this, SLOT(incrVolume()),
		"audio-volume-high", tr("Volume Up"), false
	);
	volDownAction = new ActionDescription(
		0, ActionDescription::TypeChat, "mediaplayer_vol_down",
		this, SLOT(decrVolume()),
		"audio-volume-low", tr("Volume Down"), false
	);

	Core::instance()->kaduWindow()->insertMenuActionDescription(enableMediaPlayerStatuses, KaduWindow::MenuKadu, 7);
	mediaplayerStatus = NULL;

	// Initial values of some object variables
	winKeyPressed = false;

	mediaPlayerStatusChanger = new MediaPlayerStatusChanger(this);
	StatusChangerManager::instance()->registerStatusChanger(mediaPlayerStatusChanger);

	createDefaultConfiguration();

	mediaPlayerStatusChanger->changePositionInStatus((MediaPlayerStatusChanger::ChangeDescriptionTo)config_file.readNumEntry("MediaPlayer", "statusPosition"));

	setControlsEnabled(false);
	isPaused = true;

	mediaPlayerEvent = new NotifyEvent(QString(mediaPlayerOsdHint), NotifyEvent::CallbackNotRequired, QT_TRANSLATE_NOOP("@default", "Pseudo-OSD for MediaPlayer"));
	NotificationManager::instance()->registerNotifyEvent(mediaPlayerEvent);

}

MediaPlayer::~MediaPlayer()
{
	kdebugf();

	NotificationManager::instance()->unregisterNotifyEvent(mediaPlayerEvent);
	delete mediaPlayerEvent;
	mediaPlayerEvent = 0;

	StatusChangerManager::instance()->unregisterStatusChanger(mediaPlayerStatusChanger);

	// Stop timer for checking titles
	timer->stop();

	// Disconnect all slots
	disconnect(timer, SIGNAL(timeout()), this, SLOT(checkTitle()));

	disconnect(ChatWidgetManager::instance(), SIGNAL(chatWidgetCreated(ChatWidget *)), this, SLOT(chatWidgetCreated(ChatWidget *)));
	disconnect(ChatWidgetManager::instance(), SIGNAL(chatWidgetDestroying(ChatWidget *)), this, SLOT(chatWidgetDestroying(ChatWidget *)));

	foreach (ChatWidget *it, ChatWidgetManager::instance()->chats())
		chatWidgetDestroying(it);

	delete menu;

	// Remove menu item (statuses)
//	if (mediaplayerStatus == NULL)
	Core::instance()->kaduWindow()->removeMenuActionDescription(enableMediaPlayerStatuses);
//	else
//		dockMenu->removeAction(mediaplayerStatus);
}

void MediaPlayer::setControlsEnabled(bool enabled)
{
	popups[0]->setEnabled(enabled);
	popups[1]->setEnabled(enabled);
	popups[2]->setEnabled(enabled);
	popups[3]->setEnabled(enabled);
	popups[4]->setEnabled(enabled);
}

void MediaPlayer::mediaPlayerMenuActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	ChatWidget *chatWidget = chatEditBox->chatWidget();
	if (chatWidget)
	{
		QList<QWidget *> widgets = sender->associatedWidgets();
		if (widgets.size() == 0)
			return;

		QWidget *widget = widgets[widgets.size() - 1];
		menu->popup(widget->mapToGlobal(QPoint(0, widget->height())));
	}
}

void MediaPlayer::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow->widget()->widgetById("mediaplayer/signature"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widget()->widgetById("mediaplayer/signatures"), SLOT(setEnabled(bool)));
	mainConfigurationWindow->widget()->widgetById("mediaplayer/syntax")->setToolTip(qApp->translate("@default", MediaPlayerSyntaxText));
	mainConfigurationWindow->widget()->widgetById("mediaplayer/chatShortcuts")->setToolTip(qApp->translate("@default", MediaPlayerChatShortCutsText));
}

void MediaPlayer::chatWidgetCreated(ChatWidget *chat)
{
	kdebugf();
	connect(chat->edit(), SIGNAL(keyPressed(QKeyEvent *, CustomInput *, bool &)), this, SLOT(chatKeyPressed(QKeyEvent *, CustomInput *, bool &)));
	connect(chat->edit(), SIGNAL(keyReleased(QKeyEvent *, CustomInput *, bool &)), this, SLOT(chatKeyReleased(QKeyEvent *, CustomInput *, bool &)));
}

void MediaPlayer::chatWidgetDestroying(ChatWidget *chat)
{
	kdebugf();
	disconnect(chat->edit(), SIGNAL(keyPressed(QKeyEvent *, CustomInput *, bool &)), this, SLOT(chatKeyPressed(QKeyEvent *, CustomInput *, bool &)));
	disconnect(chat->edit(), SIGNAL(keyReleased(QKeyEvent *, CustomInput *, bool &)), this, SLOT(chatKeyReleased(QKeyEvent *, CustomInput *, bool &)));
}

void MediaPlayer::chatKeyPressed(QKeyEvent *e, CustomInput *k, bool &handled)
{
	Q_UNUSED(k)

	kdebugf();

	if (handled)
		return;

	if (!config_file.readBoolEntry("MediaPlayer", "chatShortcuts", true))
		return;

	if (e->key() == SHORTCUT_KEY)
		winKeyPressed = true; // We want to handle LeftWinKey pressed state
	else if (!winKeyPressed)
		return; // If LeftWinKey isn't pressed then break function.

	if (!isActive())
		return;

	handled = true;

	switch (e->key())
	{
		case Qt::Key_Left:
			prevTrack();
			break;

		case Qt::Key_Right:
			nextTrack();
			break;

		case Qt::Key_Up:
			incrVolume();
			break;

		case Qt::Key_Down:
			decrVolume();
			break;

		case Qt::Key_Return:
		case Qt::Key_Enter:
			if (isPlaying())
				pause();
			else
				play();
			break;

		case Qt::Key_Backspace:
			stop();
			break;

		default:
			handled = false;
	}
}

void MediaPlayer::chatKeyReleased(QKeyEvent *e, CustomInput *k, bool &handled)
{
	Q_UNUSED(k)
	Q_UNUSED(handled)

	if (e->key() == SHORTCUT_KEY)
		winKeyPressed = false; // We want to handle LeftWinKey pressed state
}

void MediaPlayer::putSongTitle(int ident)
{
	Q_UNUSED(ident)

	kdebugf();

	if (!isActive())
	{
		// TODO: make it a notification
		MessageDialog::show("dialog-warning", tr("Kadu"), tr("%1 isn't running!").arg(getPlayerName()));
		return;
	}

	ChatWidget *chat = getCurrentChat();
	QString title;

	// This code tells us which item from MediaPlayer menu button was selected
	// TODO: sooooooo lame
	int id = ident;
// 	for ( int i = 0; i < 3; i++ )
// 	{
// 		if (popups[i] == ident)
// 		{
// 			id = i;
// 			break;
// 		}
// 	}

	// Sets title variable to proper value
	switch (id)
	{
		case 0:
			title = parse(config_file.readEntry("MediaPlayer", "chatString"));
			break;
		case 1:
			title = getTitle();
			break;
		case 2:
			title = getFile();
			break;
	}

// 	int x, y;

	chat->edit()->insertPlainText(title);
	// TODO: it is lame in general, we need a better API to do these things
// 	HtmlDocument doc, doc2, doc3;
// 	chat->edit()->getCursorPosition(&y, &x);
// 	chat->edit()->insertAt(title, y, x); // Here we paste the title
// 	doc.parseHtml(chat->edit()->text());
//
// 	// Parsing inserted element as HTML
// 	for (int i = 0; i < doc.countElements(); i++)
// 	{
// 		if (i == 7)
// 		{
// 			doc2.parseHtml(doc.elementText(i));
// 			for (int j = 0; j < doc2.countElements(); j++)
// 			{
// 				if (doc2.isTagElement(j))
// 					doc3.addTag(doc2.elementText(j));
// 				else
// 					doc3.addText(doc2.elementText(j));
// 			}
// 		}
// 		else
// 		{
// 			if (doc.isTagElement(i))
// 				doc3.addTag(doc.elementText(i));
// 			else
// 				doc3.addText(doc.elementText(i));
// 		}
// 	}
//
// 	chat->edit()->setText(doc3.generateHtml());
// 	chat->edit()->moveCursor(QTextEdit::MoveEnd, false);
}

void MediaPlayer::putPlayList(int ident)
{
	Q_UNUSED(ident)

	kdebugf();

	if (!isActive())
	{
		MessageDialog::show("dialog-warning", tr("Kadu"), tr("%1 isn't running!").arg(getPlayerName()));
		return;
	}

	ChatWidget* chat = getCurrentChat();
 	int id = ident;
//	int x, y;
	QStringList list;

	// This code tells us which item from MediaPlayer menu button was selected
// 	for (int i = 3; i < 5; i++)
// 	{
// 		if (popups[i] == ident)
// 		{
// 			id = i;
// 			break;
// 		}
// 	}

	uint lgt = getPlayListLength();
	if (lgt == 0)
		return;

	// Creating list of elements to paste/send
	switch (id)
	{
		case 3:
			list = getPlayListTitles();
			break;
		case 4:
			list = getPlayListFiles();
			break;
	}

	// Calculating playlist length as characters
	uint chars = 0, emptyEntries = 0;
	for (uint cnt = 0; cnt < lgt; cnt++)
	{
		if (list[cnt].length() == 0)
			emptyEntries++;

		chars += list[cnt].length();
		chars += formatLength(getLength(cnt)).length();
		chars += QString::number(cnt).length();
		chars += 12; /*
						 The white-space betwean entry and length (1),
						 dot and whitespace after number of track on the begin of line (2)
						 left and right brace for track length (2),
						 and a </p><p> tags as a new line (7).
					 */

		if (chars >= 2000)
			break;
	}
	chars += 135; // Additional chars added by Kadu chat input.

	if (emptyEntries > (lgt / 10))
	{
		if (!MessageDialog::ask("dialog-question", tr("Kadu"), tr("More than 1/10 of titles you're trying to send are empty.<br>Perhaps %1 hasn't read all titles yet, give its some more time.<br>Do you want to send playlist anyway?").arg(getPlayerName())))
			return;
	}

	if (chars >= 2000)
	{
		if (!MessageDialog::ask("dialog-question", tr("Kadu"), tr("You're trying to send %1 entries of %2 playlist.<br>It will be split and sent in few messages<br>Are you sure to do that?")
			.arg(QString::number(lgt)).arg(getPlayerName())) )
			return;
	}

	QString str;
	// TODO: make spliting in kadu-core
	chat->edit()->moveCursor(QTextCursor::End);
	for (uint cnt = 0; cnt < lgt; cnt++)
	{
		str = QString::number((cnt + 1)) + ". " + list[cnt] + " (" + formatLength(getLength(cnt)) + ")\n";
		// TODO to remove? - spliting in protocol!!
		if ((chat->edit()->document()->toPlainText().length() + str.length()) >= 2000)
			chat->sendMessage();

// 		chat->edit()->getCursorPosition(&y, &x);
		chat->edit()->insertPlainText(str); // Here we paste MediaPlayer playlist item
		chat->edit()->moveCursor(QTextCursor::End);
	}
	chat->sendMessage();
}

// TODO: it needs an update to new Kadu-parser
QString MediaPlayer::parse(const QString &str)
{
	/*
		Code of this function is partialy
		borrown from Kadu. Thanks to Kadu Team! :)
	*/
	kdebugf();

	if (isActive())
	{
		if (!isPlaying())
			return tr("Playback stopped.");
	}
	else
		return tr("Player turned off.");


	uint sl = str.length();
	QString r;

	for ( uint i = 0; i < sl; i++ )
	{
		while ((i < sl) && (str[i] != '%'))
		{
			r += str[i];
			++i;
		}

		if (i >= sl) i = sl - 1;

		if (str[i] == '%')
		{
			i++;
			switch(str[i].toLatin1())
			{
				case 't':
					r += getTitle();
					break;

				case 'a':
					r += getAlbum();
					break;

				case 'r':
					r += getArtist();
					break;

				case 'f':
					r += getFile();
					break;

				case 'l':
					r += formatLength(getLength());
					break;

				case 'c':
					r += formatLength(getCurrentPos());
					break;

				case 'p':
				{
					QString tmp;
					int len = getLength();
					if (len != 0)
					{
						int perc = 100 * getCurrentPos() / len;
						tmp = QString::number(perc) + '%';
						r += tmp;
					}
					break;
				}

				case 'n':
					r += getPlayerName();
					break;

				case 'v':
					r += getPlayerVersion();
					break;

				default:
					r += str[i];
			}
		}
	}
	return r;
}

QString MediaPlayer::formatLength(int length)
{
	kdebugf();

	QString ms;
	if (length < 1000)
		length = 1000;

	int lgt = length / 1000, m, s;
	m = lgt / 60;
	s = lgt % 60;
	ms = QString::number(m) + ':';
	if (s < 10)
		ms += '0';

	ms += QString::number(s);

	return ms;
}

ChatWidget *MediaPlayer::getCurrentChat()
{
	kdebugf();

	// Now for each chat window we check,
	// if it's an active one.
	foreach (ChatWidget *chat, ChatWidgetManager::instance()->chats())
	{
		//if (chat->isActiveWindow())
		if (chat->edit() == QApplication::focusWidget() || chat->hasFocus())
			return chat;
	}

	return 0;
}

void MediaPlayer::mediaPlayerStatusChangerActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)

	kdebugf();

	if (!isActive() && toggled)
	{
		foreach (Action *action, enableMediaPlayerStatuses->actions())
			action->setChecked(false);

		MessageDialog::show("dialog-warning", tr("Kadu"), tr("%1 isn't running!").arg(getPlayerName()));
		return;
	}

	mediaPlayerStatusChanger->setDisable(!toggled);
	if (toggled)
	{
		checkTitle();
		if (statusInterval > 0)
			timer->start(statusInterval);
	}
	else
		timer->stop();
}

void MediaPlayer::toggleStatuses(bool toggled)
{
	if (!isActive() && toggled)
	{
		MessageDialog::show("dialog-warning", tr("Kadu"), tr("%1 isn't running!").arg(getPlayerName()));
		return;
	}

	mediaPlayerStatusChanger->setDisable(!toggled);
	if (toggled && statusInterval > 0)
		timer->start(statusInterval);
	else
		timer->stop();
}

void MediaPlayer::titleChanged()
{
	if (!mediaPlayerStatusChanger->isDisabled())
		checkTitle();
}

void MediaPlayer::statusChanged()
{
	checkTitle();
}

void MediaPlayer::setInterval(int seconds)
{
	statusInterval = seconds * 1000;
}

void MediaPlayer::checkTitle()
{
	if (mediaPlayerStatusChanger->isDisabled())
		return;

	QString title = getTitle();
	int pos = getCurrentPos();

	// If OSD is enabled and current track position is betwean 0 and 1000 ms, then shows OSD
	if (config_file.readBoolEntry("MediaPlayer", "osd", true) && pos < 1000 && pos > 0)
		putTitleHint(title);

	bool checked;
	if (mediaplayerStatus != NULL)
		checked = mediaplayerStatus->isChecked();
	else if (enableMediaPlayerStatuses->action(Core::instance()->kaduWindow()->actionSource()))
		checked = enableMediaPlayerStatuses->action(Core::instance()->kaduWindow()->actionSource())->isChecked();
	else
		checked = false;

	mediaPlayerStatusChanger->setTitle(parse(config_file.readEntry("MediaPlayer", "statusTagString")));
}

void MediaPlayer::putTitleHint(QString title)
{
	kdebugf();

	Notification *notification = new Notification(QString(mediaPlayerOsdHint), "external_modules/mediaplayer-media-playback-play");
	notification->setText(title);
	NotificationManager::instance()->notify(notification);
}

void MediaPlayer::configurationUpdated()
{
	kdebugf();

	// Statuses switch
	bool enabled = false;

	if (mediaplayerStatus == NULL)
	{
		if (enableMediaPlayerStatuses->action(Core::instance()->kaduWindow()->actionSource()))
			enabled = enableMediaPlayerStatuses->action(Core::instance()->kaduWindow()->actionSource())->isChecked();
		Core::instance()->kaduWindow()->removeMenuActionDescription(enableMediaPlayerStatuses);
	}
	else
	{
		enabled = mediaplayerStatus->isChecked();
		//dockMenu->removeAction(mediaplayerStatus);
	}

	if (config_file.readBoolEntry("MediaPlayer", "dockMenu", false))
	{
		mediaplayerStatus = new QAction(tr("Enable MediaPlayer statuses"), this);
		mediaplayerStatus->setCheckable(true);
		connect(mediaplayerStatus, SIGNAL(toggled(bool)), this, SLOT(toggleStatuses(bool)));
		//dockMenu->addAction(mediaplayerStatus);
	}
	else
	{
		Core::instance()->kaduWindow()->insertMenuActionDescription(enableMediaPlayerStatuses, KaduWindow::MenuKadu);
		if (enableMediaPlayerStatuses->action(Core::instance()->kaduWindow()->actionSource()))
			enableMediaPlayerStatuses->action(Core::instance()->kaduWindow()->actionSource())->setChecked(enabled);
	}

	mediaPlayerStatusChanger->changePositionInStatus((MediaPlayerStatusChanger::ChangeDescriptionTo)config_file.readNumEntry("MediaPlayer", "statusPosition"));
}

bool MediaPlayer::playerInfoSupported()
{
	return playerInfo != 0;
}

bool MediaPlayer::playerCommandsSupported()
{
	return playerCommands != 0;
}

bool MediaPlayer::registerMediaPlayer(PlayerInfo* info, PlayerCommands* cmds)
{
	if (playerInfo != 0 || playerCommands != 0)
		return false;

	playerInfo = info;
	playerCommands = cmds;

	setControlsEnabled(true);

	return true;
}

void MediaPlayer::unregisterMediaPlayer()
{
	setControlsEnabled(false);

	playerInfo = 0;
	playerCommands = 0;
}

//
// 3rd party modules proxy methods
//

void MediaPlayer::nextTrack()
{
	if (playerCommandsSupported())
		playerCommands->nextTrack();
}

void MediaPlayer::prevTrack()
{
	if (playerCommandsSupported())
		playerCommands->prevTrack();
}

void MediaPlayer::playPause()
{
	if (!playerCommandsSupported())
		return;

	if (isPaused)
	{
		play();
		isPaused = false;
		foreach(Action *action, playAction->actions())
		{
			action->setIcon(IconsManager::instance()->iconByPath("external_modules/mediaplayer-media-playback-pause"));
			action->setText(tr("Pause"));

		}
	}
	else
	{
		pause();
		isPaused = true;
		foreach(Action *action, playAction->actions())
		{
			action->setIcon(IconsManager::instance()->iconByPath("external_modules/mediaplayer-media-playback-play"));
			action->setText(tr("Play"));
		}
	}
}

void MediaPlayer::play()
{
	if (playerCommandsSupported())
		playerCommands->play();

	isPaused = false;
	foreach(Action *action, playAction->actions())
		action->setIcon(IconsManager::instance()->iconByPath("external_modules/mediaplayer-media-playback-play"));
}

void MediaPlayer::stop()
{
	if (playerCommandsSupported())
		playerCommands->stop();

	isPaused = true;
	foreach(Action *action, playAction->actions())
		action->setIcon(IconsManager::instance()->iconByPath("external_modules/mediaplayer-media-playback-play"));
}

void MediaPlayer::pause()
{
	if (playerCommandsSupported())
		playerCommands->pause();

	isPaused = true;
	foreach(Action *action, playAction->actions())
		action->setIcon(IconsManager::instance()->iconByPath("external_modules/mediaplayer-media-playback-play"));
}

void MediaPlayer::setVolume(int vol)
{
	if (playerCommandsSupported())
		playerCommands->setVolume(vol);
}

void MediaPlayer::incrVolume()
{
	if (playerCommandsSupported())
		playerCommands->incrVolume();
}

void MediaPlayer::decrVolume()
{
	if (playerCommandsSupported())
		playerCommands->decrVolume();
}

QString MediaPlayer::getPlayerName()
{
	if (playerInfoSupported())
		return playerInfo->getPlayerName();

	return QString();
}

QString MediaPlayer::getPlayerVersion()
{
	if (playerInfoSupported())
		return playerInfo->getPlayerVersion();

	return QString();
}

QString MediaPlayer::getTitle(int position)
{
	if (playerInfoSupported())
	{
		QString title = playerInfo->getTitle(position);

		// Lets cut nasty signatures
		if (config_file.readBoolEntry("MediaPlayer", "signature", true))
		{
			QStringList sigList(config_file.readEntry("MediaPlayer", "signatures", DEFAULT_SIGNATURES).split('\n'));
			for (int i = 0; i < sigList.count(); i++)
				title.remove(sigList[i]);
		}
		return title;
	}

	return QString();
}

QString MediaPlayer::getAlbum(int position)
{
	if (playerInfoSupported())
		return playerInfo->getAlbum(position);

	return QString();
}

QString MediaPlayer::getArtist(int position)
{
	if (playerInfoSupported())
		return playerInfo->getArtist(position);

	return QString();
}

QString MediaPlayer::getFile(int position)
{
	if (playerInfoSupported())
		return playerInfo->getFile(position);

	return QString();
}

int MediaPlayer::getLength(int position)
{
	if (playerInfoSupported())
		return playerInfo->getLength(position);

	return 0;
}

int MediaPlayer::getCurrentPos()
{
	if (playerInfoSupported())
		return playerInfo->getCurrentPos();

	return 0;
}

bool MediaPlayer::isPlaying()
{
	if (playerInfoSupported())
		return playerInfo->isPlaying();

	return false;
}

bool MediaPlayer::isActive()
{
	if (playerInfoSupported())
		return playerInfo->isActive();

	return false;
}

QStringList MediaPlayer::getPlayListTitles()
{
	if (playerInfoSupported())
		return playerInfo->getPlayListTitles();

	return QStringList();
}

QStringList MediaPlayer::getPlayListFiles()
{
	if (playerInfoSupported())
		return playerInfo->getPlayListFiles();

	return QStringList();
}

uint MediaPlayer::getPlayListLength()
{
	if (playerInfoSupported())
		return playerInfo->getPlayListLength();

	return 0;
}

void MediaPlayer::createDefaultConfiguration()
{
	config_file.addVariable("MediaPlayer", "chatString", "MediaPlayer: %t [%c / %l]");
	config_file.addVariable("MediaPlayer", "statusTagString", "%r - %t");
	config_file.addVariable("MediaPlayer", "osd", true);
	config_file.addVariable("MediaPlayer", "signature", true);
	config_file.addVariable("MediaPlayer", "signatures", DEFAULT_SIGNATURES);
	config_file.addVariable("MediaPlayer", "chatShortcuts", true);
	config_file.addVariable("MediaPlayer", "dockMenu", false);
	config_file.addVariable("MediaPlayer", "statusPosition", 0);
}

void MediaPlayer::insertFormattedSong()
{
	putSongTitle(0);
}

void MediaPlayer::insertSongTitle()
{
	putSongTitle(1);
}

void MediaPlayer::insertSongFilename()
{
	putSongTitle(2);
}

void MediaPlayer::insertPlaylistTitles()
{
	putPlayList(3);
}

void MediaPlayer::insertPlaylistFilenames()
{
	putPlayList(4);
}
