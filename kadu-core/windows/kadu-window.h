/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Przemysław Rudy (prudy1@o2.pl)
 * Copyright 2009, 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010, 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include <QtCore/QMap>
#include <QtCore/QPair>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

#include "buddies/buddy-list.h"
#include "chat/chat.h"
#include "exports.h"
#include "os/generic/compositing-aware-object.h"
#include "talkable/talkable.h"
#include "windows/main-window.h"

class QSplitter;
class QMenu;
class QMenuBar;
class QVBoxLayout;

class ActionDescription;
class AddConferenceAction;
class AddGroupAction;
class AddRoomChatAction;
class AddUserAction;
class Application;
class BuddyInfoPanel;
class ChatWidgetManager;
class Configuration;
class ExitAction;
class FileTransferManager;
class InjectedFactory;
class MainWindowRepository;
class MenuInventory;
class Myself;
class OpenChatWithAction;
class OpenForumAction;
class OpenRedmineAction;
class OpenGetInvolvedAction;
class OpenSearchAction;
class OpenTranslateAction;
class ProxyActionContext;
class RecentChatsAction;
class RosterWidget;
class ShowAboutWindowAction;
class ShowBlockedBuddiesAction;
class ShowConfigurationWindowAction;
class ShowInfoPanelAction;
class ShowMultilogonsAction;
class ShowMyselfAction;
class ShowOfflineBuddiesAction;
class ShowYourAccountsAction;
class StatusButtons;
class StatusContainerManager;
class TalkableConverter;
class TalkableTreeView;
class UrlHandlerManager;
class UrlOpener;

class KADUAPI KaduWindow : public MainWindow, private ConfigurationAwareObject, CompositingAwareObject
{
    Q_OBJECT

public:
    enum MenuType
    {
        MenuKadu,
        MenuBuddies,
        MenuTools,
        MenuHelp
    };

private:
    QPointer<AddConferenceAction> m_addConferenceAction;
    QPointer<AddGroupAction> m_addGroupAction;
    QPointer<AddRoomChatAction> m_addRoomChatAction;
    QPointer<AddUserAction> m_addUserAction;
    QPointer<Application> m_application;
    QPointer<ChatWidgetManager> m_chatWidgetManager;
    QPointer<ExitAction> m_exitAction;
    QPointer<FileTransferManager> m_fileTransferManager;
    QPointer<MainWindowRepository> m_mainWindowRepository;
    QPointer<MenuInventory> m_menuInventory;
    QPointer<Myself> m_myself;
    QPointer<OpenChatWithAction> m_openChatWithAction;
    QPointer<OpenForumAction> m_openForumAction;
    QPointer<OpenGetInvolvedAction> m_openGetInvolvedAction;
    QPointer<OpenRedmineAction> m_openRedmineAction;
    QPointer<OpenSearchAction> m_openSearchAction;
    QPointer<OpenTranslateAction> m_openTranslateAction;
    QPointer<RecentChatsAction> m_recentChatsAction;
    QPointer<ShowAboutWindowAction> m_showAboutWindowAction;
    QPointer<ShowBlockedBuddiesAction> m_showBlockedBuddiesAction;
    QPointer<ShowInfoPanelAction> m_showInfoPanelAction;
    QPointer<ShowMultilogonsAction> m_showMultilogonsAction;
    QPointer<ShowMyselfAction> m_showMyselfAction;
    QPointer<ShowOfflineBuddiesAction> m_showOfflineBuddiesAction;
    QPointer<ShowConfigurationWindowAction> m_showConfigurationWindowAction;
    QPointer<ShowYourAccountsAction> m_showYourAccountsAction;
    QPointer<StatusContainerManager> m_statusContainerManager;
    QPointer<TalkableConverter> m_talkableConverter;
    QPointer<UrlHandlerManager> m_urlHandlerManager;
    QPointer<UrlOpener> m_urlOpener;

    bool Docked;   // TODO: 0.11.0 it is a hack
    QSplitter *Split;

    BuddyInfoPanel *InfoPanel;
    QMenu *KaduMenu;
    QMenu *ContactsMenu;
    QMenu *ToolsMenu;
    QMenu *HelpMenu;

    QWidget *MainWidget;
    QVBoxLayout *MainLayout;

    RosterWidget *Roster;
    StatusButtons *ChangeStatusButtons;
    QPoint LastPositionBeforeStatusMenuHide;

#ifdef Q_OS_WIN
    QWidget *HiddenParent;
#endif
    QWidget *WindowParent;

    bool CompositingEnabled;

    ProxyActionContext *Context;

    void createGui();

    void createMenu();
    void createKaduMenu();
    void createContactsMenu();
    void createToolsMenu();
    void createHelpMenu();

#ifdef Q_OS_WIN
    void hideWindowFromTaskbar();
#endif

    virtual void compositingEnabled();
    virtual void compositingDisabled();

private slots:
    INJEQT_SET void setAddConferenceAction(AddConferenceAction *addConferenceAction);
    INJEQT_SET void setAddGroupAction(AddGroupAction *addGroupAction);
    INJEQT_SET void setAddRoomChatAction(AddRoomChatAction *addRoomChatAction);
    INJEQT_SET void setAddUserAction(AddUserAction *addUserAction);
    INJEQT_SET void setApplication(Application *application);
    INJEQT_SET void setChatWidgetManager(ChatWidgetManager *chatWidgetManager);
    INJEQT_SET void setExitAction(ExitAction *exitAction);
    INJEQT_SET void setFileTransferManager(FileTransferManager *fileTransferManager);
    INJEQT_SET void setMainWindowRepository(MainWindowRepository *mainWindowRepository);
    INJEQT_SET void setMenuInventory(MenuInventory *menuInventory);
    INJEQT_SET void setMyself(Myself *myself);
    INJEQT_SET void setOpenChatWithAction(OpenChatWithAction *openChatWithAction);
    INJEQT_SET void setOpenForumAction(OpenForumAction *openForumAction);
    INJEQT_SET void setOpenRedmineAction(OpenRedmineAction *openRedmineAction);
    INJEQT_SET void setOpenGetInvolvedAction(OpenGetInvolvedAction *openGetInvolvedAction);
    INJEQT_SET void setOpenSearchAction(OpenSearchAction *openSearchAction);
    INJEQT_SET void setOpenTranslateAction(OpenTranslateAction *openTranslateAction);
    INJEQT_SET void setRecentChatsAction(RecentChatsAction *recentChatsAction);
    INJEQT_SET void setShowAboutWindowAction(ShowAboutWindowAction *showAboutWindowAction);
    INJEQT_SET void setShowBlockedBuddiesAction(ShowBlockedBuddiesAction *showBlockedBuddiesAction);
    INJEQT_SET void setShowConfigurationWindowAction(ShowConfigurationWindowAction *showConfigurationWindowAction);
    INJEQT_SET void setShowInfoPanelAction(ShowInfoPanelAction *showInfoPanelAction);
    INJEQT_SET void setShowMultilogonsAction(ShowMultilogonsAction *showMultilogonsAction);
    INJEQT_SET void setShowMyselfAction(ShowMyselfAction *showMyselfAction);
    INJEQT_SET void setShowOfflineBuddiesAction(ShowOfflineBuddiesAction *showOfflineBuddiesAction);
    INJEQT_SET void setShowYourAccountsAction(ShowYourAccountsAction *showYourAccountsAction);
    INJEQT_SET void setStatusContainerManager(StatusContainerManager *statusContainerManager);
    INJEQT_SET void setTalkableConverter(TalkableConverter *talkableConverter);
    INJEQT_SET void setUrlHandlerManager(UrlHandlerManager *urlHandlerManager);
    INJEQT_SET void setUrlOpener(UrlOpener *urlOpener);
    INJEQT_INIT void init();
    INJEQT_DONE void done();

#ifdef Q_OS_WIN
    void setHiddenParent();
#endif

protected:
    virtual void closeEvent(QCloseEvent *);
    virtual void keyPressEvent(QKeyEvent *);
    virtual void changeEvent(QEvent *event);

    virtual bool supportsActionType(ActionDescription::ActionType type);

    virtual void configurationUpdated();

public:
    static void createDefaultToolbars(Configuration *configuration, QDomElement parentConfig);

    KaduWindow();
    virtual ~KaduWindow();

    void storeConfiguration();

    virtual TalkableTreeView *talkableTreeView();
    virtual TalkableProxyModel *talkableProxyModel();

    void setDocked(bool);
    bool docked()
    {
        return Docked;
    }

    BuddyInfoPanel *infoPanel()
    {
        return InfoPanel;
    }

public slots:
    void talkableActivatedSlot(const Talkable &talkable);

signals:
    void keyPressed(QKeyEvent *e);
    void parentChanged(QWidget *oldParent);

    void talkableActivated(const Talkable &talkable);
};
