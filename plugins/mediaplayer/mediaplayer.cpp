/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtGui/QKeyEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMenu>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolTip>

#include "actions/action-description.h"
#include "actions/action.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "icons/kadu-icon.h"
#include "menu/menu-inventory.h"
#include "notification/notification-event-repository.h"
#include "notification/notification-event.h"
#include "notification/notification.h"
#include "plugin/plugin-injected-factory.h"
#include "status/status-changer-manager.h"
#include "widgets/chat-edit-box.h"
#include "widgets/chat-widget/chat-widget-repository.h"
#include "widgets/chat-widget/chat-widget.h"
#include "widgets/configuration/configuration-widget.h"
#include "widgets/custom-input.h"
#include "windows/main-configuration-window.h"
#include "windows/message-dialog.h"

#include "plugins/docking/docking-menu-action-repository.h"
#include "plugins/docking/docking.h"

#include "actions/mediaplayer-menu-action.h"
#include "actions/next-action.h"
#include "actions/play-action.h"
#include "actions/prev-action.h"
#include "actions/stop-action.h"
#include "actions/toggle-mediaplayer-statuses-action.h"
#include "actions/volume-down-action.h"
#include "actions/volume-up-action.h"
#include "media-player-status-changer.h"
#include "mediaplayer-notification-service.h"
#include "player-commands.h"
#include "player-info.h"

#include "mediaplayer.h"

#define MODULE_MEDIAPLAYER_VERSION 1.3
#define CHECK_STATUS_INTERVAL 1 * 1000 /* 1s */

#define SHORTCUT_KEY Qt::Key_Meta

// For ID3 tags signatures cutter
const char DEFAULT_SIGNATURES[] = "! WWW.POLSKIE-MP3.TK ! \n! www.polskie-mp3.tk ! ";

// Implementation of MediaPlayer class

MediaPlayer::MediaPlayer(QObject *parent) : QObject{parent}
{
}

MediaPlayer::~MediaPlayer()
{
}

void MediaPlayer::setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository)
{
    m_chatWidgetRepository = chatWidgetRepository;

    if (m_chatWidgetRepository)
    {
        // Monitor of creating chats
        connect(
            m_chatWidgetRepository.data(), SIGNAL(chatWidgetAdded(ChatWidget *)), this,
            SLOT(chatWidgetAdded(ChatWidget *)));
        connect(
            m_chatWidgetRepository.data(), SIGNAL(chatWidgetRemoved(ChatWidget *)), this,
            SLOT(chatWidgetRemoved(ChatWidget *)));

        for (ChatWidget *chatWidget : m_chatWidgetRepository.data())
            chatWidgetAdded(chatWidget);
    }
}

void MediaPlayer::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void MediaPlayer::setDockingMenuActionRepository(DockingMenuActionRepository *dockingMenuActionRepository)
{
    m_dockingMenuActionRepository = dockingMenuActionRepository;
}

void MediaPlayer::setIconsManager(IconsManager *iconsManager)
{
    m_iconsManager = iconsManager;
}

void MediaPlayer::setMediaplayerMenuAction(MediaplayerMenuAction *mediaplayerMenuAction)
{
    m_mediaplayerMenuAction = mediaplayerMenuAction;
}

void MediaPlayer::setMediaplayerNotificationService(MediaplayerNotificationService *mediaplayerNotificationService)
{
    m_mediaplayerNotificationService = mediaplayerNotificationService;
}

void MediaPlayer::setMenuInventory(MenuInventory *menuInventory)
{
    m_menuInventory = menuInventory;
}

void MediaPlayer::setNextAction(NextAction *nextAction)
{
    m_nextAction = nextAction;
}

void MediaPlayer::setPlayAction(PlayAction *playAction)
{
    m_playAction = playAction;
}

void MediaPlayer::setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory)
{
    m_pluginInjectedFactory = pluginInjectedFactory;
}

void MediaPlayer::setPrevAction(PrevAction *prevAction)
{
    m_prevAction = prevAction;
}

void MediaPlayer::setStatusChangerManager(StatusChangerManager *statusChangerManager)
{
    m_statusChangerManager = statusChangerManager;
}

void MediaPlayer::setStopAction(StopAction *stopAction)
{
    m_stopAction = stopAction;
}

void MediaPlayer::setVolumeDownAction(VolumeDownAction *volumeDownAction)
{
    m_volumeDownAction = volumeDownAction;
}

void MediaPlayer::setVolumeUpAction(VolumeUpAction *volumeUpAction)
{
    m_volumeUpAction = volumeUpAction;
}

void MediaPlayer::setToggleMediaplayerStatusesAction(ToggleMediaplayerStatusesAction *toggleMediaplayerStatusesAction)
{
    m_toggleMediaplayerStatusesAction = toggleMediaplayerStatusesAction;
}

void MediaPlayer::init()
{
    // Initialization
    playerInfo = 0;
    playerCommands = 0;
    statusInterval = CHECK_STATUS_INTERVAL;

    // MediaPlayer menus in chats
    m_menu = new QMenu();
    popups[0] = m_menu->addAction(tr("Put formated string"), this, SLOT(insertFormattedSong()));
    popups[1] = m_menu->addAction(tr("Put song title"), this, SLOT(insertSongTitle()));
    popups[2] = m_menu->addAction(tr("Put song file name"), this, SLOT(insertSongFilename()));
    popups[3] = m_menu->addAction(tr("Send all playlist titles"), this, SLOT(insertPlaylistTitles()));
    popups[4] = m_menu->addAction(tr("Send all playlist files"), this, SLOT(insertPlaylistFilenames()));

    // Title checking timer
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkTitle()));

    DockedMediaplayerStatus = 0;

    // Initial values of some object variables
    winKeyPressed = false;

    Changer = new MediaPlayerStatusChanger(this);
    m_statusChangerManager->registerStatusChanger(Changer);
    connect(
        m_statusChangerManager, SIGNAL(manualStatusAboutToBeChanged(StatusContainer *, Status)), this,
        SLOT(statusAboutToBeChanged()));

    setControlsEnabled(false);
    isPaused = true;

    Changer->changePositionInStatus(
        (MediaPlayerStatusChanger::ChangeDescriptionTo)m_configuration->deprecatedApi()->readNumEntry(
            "MediaPlayer", "statusPosition"));
    createDefaultConfiguration();
    configurationUpdated();
}

void MediaPlayer::done()
{
    if (DockedMediaplayerStatus)
        m_dockingMenuActionRepository->removeAction(DockedMediaplayerStatus);

    m_statusChangerManager->unregisterStatusChanger(Changer);

    timer->stop();

    if (m_chatWidgetRepository)
    {
        disconnect(m_chatWidgetRepository.data(), 0, this, 0);

        for (ChatWidget *chatWidget : m_chatWidgetRepository.data())
            chatWidgetRemoved(chatWidget);
    }

    delete m_menu;

    m_menuInventory->menu("main")->removeAction(m_toggleMediaplayerStatusesAction)->update();
}

void MediaPlayer::setControlsEnabled(bool enabled)
{
    popups[0]->setEnabled(enabled);
    popups[1]->setEnabled(enabled);
    popups[2]->setEnabled(enabled);
    popups[3]->setEnabled(enabled);
    popups[4]->setEnabled(enabled);
}

void MediaPlayer::chatWidgetAdded(ChatWidget *chat)
{
    connect(
        chat->edit(), SIGNAL(keyPressed(QKeyEvent *, CustomInput *, bool &)), this,
        SLOT(chatKeyPressed(QKeyEvent *, CustomInput *, bool &)));
    connect(
        chat->edit(), SIGNAL(keyReleased(QKeyEvent *, CustomInput *, bool &)), this,
        SLOT(chatKeyReleased(QKeyEvent *, CustomInput *, bool &)));
}

void MediaPlayer::chatWidgetRemoved(ChatWidget *chat)
{
    disconnect(chat->edit(), 0, this, 0);
}

void MediaPlayer::chatKeyPressed(QKeyEvent *e, CustomInput *k, bool &handled)
{
    Q_UNUSED(k)

    if (handled)
        return;

    if (!m_configuration->deprecatedApi()->readBoolEntry("MediaPlayer", "chatShortcuts", true))
        return;

    if (e->key() == SHORTCUT_KEY)
        winKeyPressed = true;   // We want to handle LeftWinKey pressed state
    else if (!winKeyPressed)
        return;   // If LeftWinKey isn't pressed then break function.

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
        winKeyPressed = false;   // We want to handle LeftWinKey pressed state
}

void MediaPlayer::putSongTitle(int ident)
{
    Q_UNUSED(ident)

    if (!isActive())
    {
        // TODO: make it a notification
        if (!getPlayerName().isEmpty())
            MessageDialog::show(
                m_iconsManager->iconByPath(KaduIcon("dialog-warning")), tr("Kadu"),
                tr("%1 isn't running!").arg(getPlayerName()));
        else
            MessageDialog::show(
                m_iconsManager->iconByPath(KaduIcon("dialog-warning")), tr("Kadu"), tr("Player isn't running!"));

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
        title = parse(m_configuration->deprecatedApi()->readEntry("MediaPlayer", "chatString"));
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

    if (!isActive())
    {
        if (!getPlayerName().isEmpty())
            MessageDialog::show(
                m_iconsManager->iconByPath(KaduIcon("dialog-warning")), tr("Kadu"),
                tr("%1 isn't running!").arg(getPlayerName()));
        else
            MessageDialog::show(
                m_iconsManager->iconByPath(KaduIcon("dialog-warning")), tr("Kadu"), tr("Player isn't running!"));

        return;
    }

    ChatWidget *chat = getCurrentChat();
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

    if (list.isEmpty())
        return;

    uint lgt = list.size();

    // Calculating playlist length as characters
    uint chars = 0, emptyEntries = 0;
    for (uint cnt = 0; cnt < lgt; cnt++)
    {
        if (list[cnt].length() == 0)
            emptyEntries++;

        chars += list[cnt].length();
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
    chars += 135;   // Additional chars added by Kadu chat input.

    if (emptyEntries > (lgt / 10))
    {
        QString question = tr("More than 1/10 of titles you're trying to send are empty.<br>Perhaps %1 hasn't read all "
                              "titles yet, give its some more time.<br>Do you want to send playlist anyway?")
                               .arg(getPlayerName());

        MessageDialog *dialog =
            MessageDialog::create(m_iconsManager->iconByPath(KaduIcon("dialog-question")), tr("Kadu"), question);
        dialog->addButton(QMessageBox::Yes, tr("Send anyway"));
        dialog->addButton(QMessageBox::No, tr("Cancel"));

        if (!dialog->ask())
            return;
    }

    if (chars >= 2000)
    {
        QString question = tr("You're trying to send %1 entries of %2 playlist.<br>It will be split and sent in few "
                              "messages<br>Are you sure to do that?")
                               .arg(QString::number(lgt))
                               .arg(getPlayerName());
        MessageDialog *dialog =
            MessageDialog::create(m_iconsManager->iconByPath(KaduIcon("dialog-question")), tr("Kadu"), question);
        dialog->addButton(QMessageBox::Yes, tr("Send"));
        dialog->addButton(QMessageBox::No, tr("Cancel"));

        if (!dialog->ask())
            return;
    }

    QString str;
    // TODO: make spliting in kadu-core
    chat->edit()->moveCursor(QTextCursor::End);
    for (uint cnt = 0; cnt < lgt; cnt++)
    {
        str = QString::number((cnt + 1)) + ". " + list[cnt] + "\n";
        // TODO to remove? - spliting in protocol!!
        if ((chat->edit()->document()->toPlainText().length() + str.length()) >= 2000)
            chat->sendMessage();

        // 		chat->edit()->getCursorPosition(&y, &x);
        chat->edit()->insertPlainText(str);   // Here we paste MediaPlayer playlist item
        chat->edit()->moveCursor(QTextCursor::End);
    }
    chat->sendMessage();
}

// TODO: it needs an update to new Kadu-parser
QString MediaPlayer::parse(const QString &str)
{
    if (isActive())
    {
        if (!isPlaying())
            return tr("Playback stopped.");
    }
    else
        return tr("Player turned off.");

    uint sl = str.length();
    QString r;

    for (uint i = 0; i < sl; i++)
    {
        while ((i < sl) && (str[i] != '%'))
        {
            r += str[i];
            ++i;
        }

        if (i >= sl)
            i = sl - 1;

        if (str[i] == '%')
        {
            i++;
            switch (str[i].toLatin1())
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
    if (!m_chatWidgetRepository)
        return 0;

    // Now for each chat window we check,
    // if it's an active one.
    for (ChatWidget *chatWidget : m_chatWidgetRepository.data())
    {
        // if (chat->isActiveWindow())
        if (chatWidget->edit() == QApplication::focusWidget() || chatWidget->hasFocus())
            return chatWidget;
    }

    return 0;
}

void MediaPlayer::statusAboutToBeChanged()
{
    if (Changer->changeDescriptionTo() == MediaPlayerStatusChanger::DescriptionReplace)
        toggleStatuses(false);
}

void MediaPlayer::toggleStatuses(bool toggled)
{
    if (!isActive() && toggled)
    {
        for (auto &&action : m_toggleMediaplayerStatusesAction->actions())
            action->setChecked(false);

        if (!getPlayerName().isEmpty())
            MessageDialog::show(
                m_iconsManager->iconByPath(KaduIcon("dialog-warning")), tr("Kadu"),
                tr("%1 isn't running!").arg(getPlayerName()));
        else
            MessageDialog::show(
                m_iconsManager->iconByPath(KaduIcon("dialog-warning")), tr("Kadu"), tr("Player isn't running!"));

        return;
    }

    for (auto &&action : m_toggleMediaplayerStatusesAction->actions())
        action->setChecked(toggled);

    Changer->setDisable(!toggled);
    if (toggled)
    {
        checkTitle();
        if (statusInterval > 0)
            timer->start(statusInterval);
    }
    else
        timer->stop();
}

void MediaPlayer::titleChanged()
{
    if (!Changer->isDisabled())
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
    if (Changer->isDisabled())
        return;

    int pos = getCurrentPos();

    // If OSD is enabled and current track position is betwean 0 and 1000 ms, then shows OSD
    if (m_configuration->deprecatedApi()->readBoolEntry("MediaPlayer", "osd", true) && pos < 1000 && pos > 0)
        m_mediaplayerNotificationService->notifyPlayingTitle(getTitle());

    Changer->setTitle(parse(m_configuration->deprecatedApi()->readEntry("MediaPlayer", "statusTagString")));
}

void MediaPlayer::configurationUpdated()
{
    // Statuses switch
    bool enabled = !Changer->isDisabled();

    if (m_configuration->deprecatedApi()->readBoolEntry("MediaPlayer", "dockMenu", false))
    {
        m_menuInventory->menu("main")->removeAction(m_toggleMediaplayerStatusesAction)->update();

        if (!DockedMediaplayerStatus)
        {
            DockedMediaplayerStatus = new QAction(tr("Enable MediaPlayer statuses"), this);
            DockedMediaplayerStatus->setCheckable(true);
            DockedMediaplayerStatus->setChecked(enabled);
            connect(DockedMediaplayerStatus, SIGNAL(toggled(bool)), this, SLOT(toggleStatuses(bool)));

            m_dockingMenuActionRepository->addAction(DockedMediaplayerStatus);
        }
    }
    else
    {
        m_menuInventory->menu("main")
            ->addAction(m_toggleMediaplayerStatusesAction, KaduMenu::SectionMiscTools, 7)
            ->update();

        if (DockedMediaplayerStatus)
        {
            m_dockingMenuActionRepository->removeAction(DockedMediaplayerStatus);
            delete DockedMediaplayerStatus;
            DockedMediaplayerStatus = 0;
        }
    }

    Changer->changePositionInStatus(
        (MediaPlayerStatusChanger::ChangeDescriptionTo)m_configuration->deprecatedApi()->readNumEntry(
            "MediaPlayer", "statusPosition"));
}

bool MediaPlayer::playerInfoSupported()
{
    return playerInfo != 0;
}

bool MediaPlayer::playerCommandsSupported()
{
    return playerCommands != 0;
}

bool MediaPlayer::registerMediaPlayer(PlayerInfo *info, PlayerCommands *cmds)
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
        for (auto action : m_playAction->actions())
        {
            action->setIcon(KaduIcon("external_modules/mediaplayer-media-playback-pause"));
            action->setText(tr("Pause"));
        }
    }
    else
    {
        pause();
        isPaused = true;
        for (auto action : m_playAction->actions())
        {
            action->setIcon(KaduIcon("external_modules/mediaplayer-media-playback-play"));
            action->setText(tr("Play"));
        }
    }
}

void MediaPlayer::play()
{
    if (playerCommandsSupported())
        playerCommands->play();

    isPaused = false;
    for (auto action : m_playAction->actions())
        action->setIcon(KaduIcon("external_modules/mediaplayer-media-playback-play"));
}

void MediaPlayer::stop()
{
    if (playerCommandsSupported())
        playerCommands->stop();

    isPaused = true;
    for (auto action : m_playAction->actions())
        action->setIcon(KaduIcon("external_modules/mediaplayer-media-playback-play"));
}

void MediaPlayer::pause()
{
    if (playerCommandsSupported())
        playerCommands->pause();

    isPaused = true;
    for (auto action : m_playAction->actions())
        action->setIcon(KaduIcon("external_modules/mediaplayer-media-playback-play"));
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

QString MediaPlayer::getTitle()
{
    if (playerInfoSupported())
    {
        QString title = playerInfo->getTitle();

        // Lets cut nasty signatures
        if (m_configuration->deprecatedApi()->readBoolEntry("MediaPlayer", "signature", true))
        {
            QStringList sigList(
                m_configuration->deprecatedApi()
                    ->readEntry("MediaPlayer", "signatures", DEFAULT_SIGNATURES)
                    .split('\n'));
            for (int i = 0; i < sigList.count(); i++)
                title.remove(sigList[i]);
        }
        return title;
    }

    return QString();
}

QString MediaPlayer::getAlbum()
{
    if (playerInfoSupported())
        return playerInfo->getAlbum();

    return QString();
}

QString MediaPlayer::getArtist()
{
    if (playerInfoSupported())
        return playerInfo->getArtist();

    return QString();
}

QString MediaPlayer::getFile()
{
    if (playerInfoSupported())
        return playerInfo->getFile();

    return QString();
}

int MediaPlayer::getLength()
{
    if (playerInfoSupported())
        return playerInfo->getLength();

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

void MediaPlayer::createDefaultConfiguration()
{
    m_configuration->deprecatedApi()->addVariable("MediaPlayer", "chatString", "MediaPlayer: %t [%c / %l]");
    m_configuration->deprecatedApi()->addVariable("MediaPlayer", "statusTagString", "%r - %t");
    m_configuration->deprecatedApi()->addVariable("MediaPlayer", "osd", true);
    m_configuration->deprecatedApi()->addVariable("MediaPlayer", "signature", true);
    m_configuration->deprecatedApi()->addVariable("MediaPlayer", "signatures", DEFAULT_SIGNATURES);
    m_configuration->deprecatedApi()->addVariable("MediaPlayer", "chatShortcuts", true);
    m_configuration->deprecatedApi()->addVariable("MediaPlayer", "dockMenu", false);
    m_configuration->deprecatedApi()->addVariable("MediaPlayer", "statusPosition", 0);
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

#include "moc_mediaplayer.cpp"
