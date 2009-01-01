/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QMenu>
#include <QtGui/QToolTip>

#include "action.h"
#include "chat_edit_box.h"
#include "chat_manager.h"
#include "chat_widget.h"
#include "config_file.h"
#include "custom_input.h"
#include "debug.h"
#include "gadu.h"
#include "html_document.h"
#include "icons_manager.h"
#include "kadu.h"
#include "message_box.h"
#include "status.h"
#include "userlist.h"

#include "../notify/notify.h"

#include "mp_status_changer.h"
#include "player_commands.h"
#include "player_info.h"

#include "mediaplayer.h"

#define MODULE_MEDIAPLAYER_VERSION 1.2
#define CHECK_STATUS_INTERVAL 10*1000 /* 10s */

const char *MediaPlayerSyntaxText = QT_TRANSLATE_NOOP
(
	"@default",
	"Syntax: %t - song title,\n%a - album, %r - artist, %f - file name,\n"
	"%l - song length (MM:SS), %c - current song position (MM:SS),\n"
	"%p - percents of played song, %n - player name, %v - player version\n"
);

const char *MediaPlayerChatShortCutsText = QT_TRANSLATE_NOOP
(
	"@default",
	"With this option enabled you'll be able to control\n"
	"your MediaPlayer in chat window by keyboard shortcuts:\n"
	"Win+ Enter/Backspace/Left/Right/Up/Down."
);

// TODO: remove For CP1250->ISO8859-2 converter
const char CODE_CP1250[]    = {0xb9, 0x9c, 0x9f, 0xa5, 0x8c, 0x8f};
const char CODE_ISO8859_2[] = {0xb1, 0xb6, 0xbc, 0xa1, 0xa6, 0xac};

// For ID3 tags signatures cutter
const char DEFAULT_SIGNATURES[] = "! WWW.POLSKIE-MP3.TK ! \n! www.polskie-mp3.tk ! ";

// The main mediaplayer module object
MediaPlayer *mediaplayer;

const char *mediaPlayerOsdHint = "MediaPlayerOsd";

// Kadu initializing functions
extern "C" int mediaplayer_init(bool firstLoad)
{
	mediaplayer = new MediaPlayer(firstLoad);
	notification_manager->registerEvent(mediaPlayerOsdHint, QT_TRANSLATE_NOOP("@default", "Pseudo-OSD for MediaPlayer"), CallbackNotRequired);

	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/mediaplayer.ui"), mediaplayer);

	return 0;
}

extern "C" void mediaplayer_close()
{
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/mediaplayer.ui"), mediaplayer);

	notification_manager->unregisterEvent(mediaPlayerOsdHint);
	delete mediaplayer;
}

// Implementation of MediaPlayer class

MediaPlayer::MediaPlayer(bool firstLoad)
{
	kdebugf();

	// Initialization
	playerInfo = 0;
	playerCommands = 0;

	// MediaPlayer menus in chats
	menu = new QMenu();
	popups[0] = menu->insertItem(tr("Put formated string"), this, SLOT(putSongTitle(int)));
	popups[1] = menu->insertItem(tr("Put song title"), this, SLOT(putSongTitle(int)));
	popups[2] = menu->insertItem(tr("Put song file name"), this, SLOT(putSongTitle(int)));
	popups[3] = menu->insertItem(tr("Send all playlist titles"), this, SLOT(putPlayList(int)));
	popups[4] = menu->insertItem(tr("Send all playlist files"), this, SLOT(putPlayList(int)));

	// Title checking timer
	timer = new QTimer();
	connect(timer, SIGNAL(timeout()), this, SLOT(checkTitle()));

	// Monitor of creating chats
	connect(chat_manager, SIGNAL(chatWidgetCreated(ChatWidget *)), this, SLOT(chatWidgetCreated(ChatWidget *)));
	connect(chat_manager, SIGNAL(chatWidgetDestroying(ChatWidget *)), this, SLOT(chatWidgetDestroying(ChatWidget *)));

	foreach (ChatWidget *it, chat_manager->chats())
		chatWidgetCreated(it);

	enableMediaPlayerStatuses = new ActionDescription(
		ActionDescription::TypeGlobal, "enableMediaPlayerStatusesAction",
		this, SLOT(mediaPlayerStatusChangerActivated(QAction *, bool)),
		"MediaPlayer", tr("Enable MediaPlayer statuses"), true
	);
	mediaPlayerMenu = new ActionDescription(
		ActionDescription::TypeChat, "mediaplayer_button",
		this, SLOT(mediaPlayerMenuActivated(QAction *, bool)),
		"MediaPlayerButton", tr("MediaPlayer"), false, ""
	);

	if (firstLoad)
		ChatEditBox::addAction("mediaplayer_button");

	// MediaPlayer statuses menu item
	bool menuPos = config_file.readBoolEntry("MediaPlayer", "dockMenu", false);
	if (menuPos)
	{
		mediaplayerStatus = new QAction(tr("Enable MediaPlayer statuses"), this);
		mediaplayerStatus->setCheckable(true);
		connect(mediaplayerStatus, SIGNAL(toggled(bool)), this, SLOT(toggleStatuses(bool)));
		dockMenu->addAction(mediaplayerStatus);
	}
	else
	{
		kadu->insertMenuActionDescription(0, enableMediaPlayerStatuses);
		mediaplayerStatus = NULL;
	}

	// Initial values of some object variables
	winKeyPressed = false;

	mediaPlayerStatusChanger = new MediaPlayerStatusChanger();
	status_changer_manager->registerStatusChanger(mediaPlayerStatusChanger);

	createDefaultConfiguration();

	mediaPlayerStatusChanger->changePositionInStatus((MediaPlayerStatusChanger::ChangeDescriptionTo)config_file.readNumEntry("MediaPlayer", "statusPosition"));

	setControlsEnabled(false);
}

MediaPlayer::~MediaPlayer()
{
	kdebugf();

	status_changer_manager->unregisterStatusChanger(mediaPlayerStatusChanger);
	delete mediaPlayerStatusChanger;
	mediaPlayerStatusChanger = 0;

	// Stop timer for checking titles
	timer->stop();

	// Disconnect all slots
	disconnect(timer, SIGNAL(timeout()), this, SLOT(checkTitle()));

	disconnect(chat_manager, SIGNAL(chatWidgetCreated(ChatWidget *)), this, SLOT(chatWidgetCreated(ChatWidget *)));
	disconnect(chat_manager, SIGNAL(chatWidgetDestroying(ChatWidget *)), this, SLOT(chatWidgetDestroying(ChatWidget *)));

	foreach (ChatWidget *it, chat_manager->chats())
		chatWidgetDestroying(it);

	// Delete own objects
	delete menu;
	delete timer;

	// Remove menu item (statuses)
	if (mediaplayerStatus == NULL)
		kadu->removeMenuActionDescription(enableMediaPlayerStatuses);
	else
		dockMenu->removeAction(mediaplayerStatus);
}

void MediaPlayer::setControlsEnabled(bool enabled)
{
	menu->setItemEnabled(popups[0], enabled);
	menu->setItemEnabled(popups[1], enabled);
	menu->setItemEnabled(popups[2], enabled);
	menu->setItemEnabled(popups[3], enabled);
	menu->setItemEnabled(popups[4], enabled);
}

void MediaPlayer::mediaPlayerMenuActivated(QAction *sender, bool toggled)
{
	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(sender->parent());
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
	connect(mainConfigurationWindow->widgetById("mediaplayer/signature"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("mediaplayer/signatures"), SLOT(setEnabled(bool)));
	QToolTip::add(mainConfigurationWindow->widgetById("mediaplayer/syntax"), qApp->translate("@default", MediaPlayerSyntaxText));
	QToolTip::add(mainConfigurationWindow->widgetById("mediaplayer/chatShortcuts"), qApp->translate("@default", MediaPlayerChatShortCutsText));
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
	kdebugf();

	if (!config_file.readBoolEntry("MediaPlayer", "chatShortcuts", true) || !isActive())
		return;

	if (e->key() == Qt::Key_Super_L)
		winKeyPressed = true; // We want to handle LeftWinKey pressed state
	else if (!winKeyPressed)
		return; // If LeftWinKey isn't pressed then break function.

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
	if (e->key() == Qt::Key_Super_L)
		winKeyPressed = false; // We want to handle LeftWinKey pressed state
}

void MediaPlayer::putSongTitle(int ident)
{
	kdebugf();

	if (!isActive())
	{
		// TODO: make it a notification
		MessageBox::msg(tr("%1 isn't running!").arg(getPlayerName()));
		return;
	}

	ChatWidget *chat = getCurrentChat();
	QString title;

	// This code tells us which item from MediaPlayer menu button was selected
	// TODO: sooooooo lame
	int id = 0;
	for ( int i = 0; i < 3; i++ )
	{
		if (popups[i] == ident)
		{
			id = i;
			break;
		}
	}

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

	int x, y;

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
	kdebugf();

	if (!isActive())
	{
		MessageBox::msg(tr("%1 isn't running!").arg(getPlayerName()));
		return;
	}

	ChatWidget* chat = getCurrentChat();
	int id = 0, x, y;
	QStringList list;

	// This code tells us which item from MediaPlayer menu button was selected
	for (int i = 3; i < 5; i++)
	{
		if (popups[i] == ident)
		{
			id = i;
			break;
		}
	}

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
		if (!MessageBox::ask(tr("More than 1/10 of titles you're trying to send are empty.<br>Perhaps %1 havn't read all titles yet, give its some more time.<br>Do you want to send playlist anyway?").arg(getPlayerName())))
			return;
	}

	if (chars >= 2000)
	{
		if (!MessageBox::ask(tr("You're trying to send %1 entries of %2 playlist.<br>It will be splitted and sent in few messages<br>Are you sure to do that?")
			.arg(QString::number(lgt)).arg(getPlayerName())) )
			return;
	}

	QString str;
	// TODO: make spliting in kadu-core
	chat->edit()->moveCursor(QTextEdit::MoveEnd, false);
	for (uint cnt = 0; cnt < lgt; cnt++)
	{
		str = QString::number((cnt + 1)) + ". " + list[cnt] + " (" + formatLength(getLength(cnt)) + ")\n";
		if ((chat->edit()->text().length() + str.length()) >= 2000)
			chat->sendMessage();

// 		chat->edit()->getCursorPosition(&y, &x);
		chat->edit()->insertPlainText(str); // Here we paste MediaPlayer playlist item
		chat->edit()->moveCursor(QTextEdit::MoveEnd, false);
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
		while (str[i] != '%' && i < sl)
		{
			r += str[i];
			i++;
		}

		if (str[i] == '%')
		{
			i++;
			switch(str[i].latin1())
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
					int perc = 100 * getCurrentPos() / getLength();
					tmp = QString::number(perc) + "%";
					r += tmp;
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
	int lgt = length / 1000, m, s;
	m = lgt / 60;
	s = lgt % 60;
	ms = QString::number(m) + ":";
	if (s < 10)
		ms += "0";

	ms += QString::number(s);

	return ms;
}

ChatWidget *MediaPlayer::getCurrentChat()
{
	kdebugf();

	// Getting all chat windows
	ChatList cs = chat_manager->chats();

	// Now for each chat window we check,
	// if it's an active one.
	uint i;
	for ( i = 0; i < cs.count(); i++ )
	{
		//if (cs[i]->isActiveWindow())
		if (cs[i]->edit() == QApplication::focusWidget() ||
			cs[i]->hasFocus())
			break;
	}

	if (i == cs.count())
		return 0;

	return cs[i];
}

void MediaPlayer::mediaPlayerStatusChangerActivated(QAction *sender, bool toggled)
{
	kdebugf();

	if (!isActive() && toggled)
	{
		foreach (KaduAction *action, enableMediaPlayerStatuses->actions())
			action->setChecked(false);

		MessageBox::msg(tr("%1 isn't running!").arg(getPlayerName()));
		return;
	}

	mediaPlayerStatusChanger->setDisable(!toggled);
	if (toggled)
		timer->start(CHECK_STATUS_INTERVAL);
	else
		timer->stop();
}

void MediaPlayer::toggleStatuses(bool toggled)
{
	if (!isActive() && toggled)
	{
		MessageBox::msg(tr("%1 isn't running!").arg(getPlayerName()));
		return;
	}
	
	mediaPlayerStatusChanger->setDisable(!toggled);
	if (toggled)
		timer->start(CHECK_STATUS_INTERVAL);
	else
		timer->stop();
}

void MediaPlayer::checkTitle()
{
	QString title = getTitle();
	int pos = getCurrentPos();

	// If OSD is enabled and current track position is betwean 0 and 1000 ms, then shows OSD
	if (config_file.readBoolEntry("MediaPlayer", "osd", true) && pos < 1000 && pos > 0)
		putTitleHint(title);

	bool checked;
	int idx = dockMenu->indexOf(popups[5]);
	if (idx != -1)
		checked = dockMenu->isItemChecked(popups[5]);
	else
		if (enableMediaPlayerStatuses->action(kadu))
			checked = enableMediaPlayerStatuses->action(kadu)->isChecked();
		else
			checked = false;

	if (!gadu->currentStatus().isOffline() && checked)
	{
		if (title != currentTitle || !gadu->currentStatus().hasDescription())
		{
			currentTitle = title;
			mediaPlayerStatusChanger->setTitle(parse(config_file.readEntry("MediaPlayer", "statusTagString")));
		}
	}
}

void MediaPlayer::putTitleHint(QString title)
{
	kdebugf();

	Notification *notification = new Notification(mediaPlayerOsdHint, "MediaPlayer", UserListElements());
	notification->setText(title);
	notification_manager->notify(notification);
}

void MediaPlayer::configurationUpdated()
{
	kdebugf();

	// Statuses switch
	bool enabled;

	int idx = dockMenu->indexOf(popups[5]);
	if (idx == -1)
	{
		if (enableMediaPlayerStatuses->action(kadu))
			enabled = enableMediaPlayerStatuses->action(kadu)->isChecked();
		kadu->removeMenuActionDescription(enableMediaPlayerStatuses);
	}
	else
	{
		enabled = dockMenu->isItemChecked(popups[5]);
		dockMenu->removeItem(popups[5]);
	}

	bool menuPos = config_file.readBoolEntry("MediaPlayer", "dockMenu", false);
	if (menuPos)
	{
		popups[5] = dockMenu->insertItem(tr("Enable MediaPlayer statuses"), this, SLOT(toggleStatuses(int)), 0, -1, 10);
		dockMenu->setItemChecked(popups[5], enabled);
	}
	else
	{
		kadu->addMenuActionDescription(enableMediaPlayerStatuses);
		if (enableMediaPlayerStatuses->action(kadu))
			enableMediaPlayerStatuses->action(kadu)->setChecked(enabled);
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

void MediaPlayer::play()
{
	if (playerCommandsSupported())
		playerCommands->play();
}

void MediaPlayer::stop()
{
	if (playerCommandsSupported())
		playerCommands->stop();
}

void MediaPlayer::pause()
{
	if (playerCommandsSupported())
		playerCommands->pause();
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

	return "";
}

QString MediaPlayer::getPlayerVersion()
{
	if (playerInfoSupported())
		return playerInfo->getPlayerVersion();

	return "";
}

QString MediaPlayer::getTitle(int position)
{
	if (playerInfoSupported())
	{
		QString title = playerInfo->getTitle(position);

		// Lets cut nasty signatures
		if (config_file.readBoolEntry("MediaPlayer", "signature", true))
		{
			QStringList sigList(QStringList::split('\n', config_file.readEntry("MediaPlayer", "signatures", DEFAULT_SIGNATURES)));
			for (unsigned int i = 0; i < sigList.count(); i++)
				title.remove(sigList[i]);
		}
		return title;
	}

	return "";
}

QString MediaPlayer::getAlbum(int position)
{
	if (playerInfoSupported())
		return playerInfo->getAlbum(position);

	return "";
}

QString MediaPlayer::getArtist(int position)
{
	if (playerInfoSupported())
		return playerInfo->getArtist(position);

	return "";
}

QString MediaPlayer::getFile(int position)
{
	if (playerInfoSupported())
		return playerInfo->getFile(position);

	return "";
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
