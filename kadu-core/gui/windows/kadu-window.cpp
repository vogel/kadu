/*
 * %kadu copyright begin%
 * Copyright 2010 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Przemysław Rudy (prudy1@o2.pl)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QCloseEvent>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QVBoxLayout>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#include "accounts/account-manager.h"
#include "actions/add-group-action.h"
#include "actions/add-user-action.h"
#include "actions/exit-action.h"
#include "actions/open-forum-action.h"
#include "actions/open-get-involved-action.h"
#include "actions/open-redmine-action.h"
#include "actions/open-search-action.h"
#include "actions/open-translate-action.h"
#include "actions/show-about-window-action.h"
#include "actions/show-blocked-buddies-action.h"
#include "actions/show-configuration-window-action.h"
#include "actions/show-info-panel-action.h"
#include "actions/show-multilogons-action.h"
#include "actions/show-myself-action.h"
#include "actions/show-offline-buddies-action.h"
#include "actions/show-your-accounts-action.h"
#include "buddies/buddy-set.h"
#include "chat/model/chat-data-extractor.h"
#include "chat/type/chat-type-manager.h"
#include "configuration/config-file-variant-wrapper.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-set.h"
#include "contacts/contact.h"
#include "core/application.h"
#include "core/injected-factory.h"
#include "core/myself.h"
#include "file-transfer/file-transfer-manager.h"
#include "gui/actions/action.h"
#include "gui/actions/chat/add-conference-action.h"
#include "gui/actions/chat/add-room-chat-action.h"
#include "gui/actions/recent-chats-action.h"
#include "gui/hot-key.h"
#include "gui/menu/menu-inventory.h"
#include "gui/menu/menu-item.h"
#include "gui/widgets/buddy-info-panel.h"
#include "gui/widgets/chat-widget/actions/open-chat-with-action.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/widgets/recent-chats-menu.h"
#include "gui/widgets/roster-widget.h"
#include "gui/widgets/status-buttons.h"
#include "gui/widgets/talkable-tree-view.h"
#include "gui/windows/kadu-window-actions.h"
#include "gui/windows/main-window-repository.h"
#include "gui/windows/proxy-action-context.h"
#include "os/generic/url-opener.h"
#include "os/generic/window-geometry-manager.h"
#include "status/status-container-manager.h"
#include "talkable/talkable-converter.h"
#include "url-handlers/url-handler-manager.h"
#include "activate.h"

#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "debug.h"

#include "kadu-window.h"

KaduWindow::KaduWindow() :
		MainWindow(nullptr, QString(), 0), Docked(false),
		WindowParent(0), CompositingEnabled(false)
{
}

KaduWindow::~KaduWindow()
{
}

void KaduWindow::setAddConferenceAction(AddConferenceAction *addConferenceAction)
{
	m_addConferenceAction = addConferenceAction;
}

void KaduWindow::setAddGroupAction(AddGroupAction *addGroupAction)
{
	m_addGroupAction = addGroupAction;
}

void KaduWindow::setAddRoomChatAction(AddRoomChatAction *addRoomChatAction)
{
	m_addRoomChatAction = addRoomChatAction;
}

void KaduWindow::setAddUserAction(AddUserAction *addUserAction)
{
	m_addUserAction = addUserAction;
}

void KaduWindow::setApplication(Application *application)
{
	m_application = application;
}

void KaduWindow::setChatWidgetManager(ChatWidgetManager *chatWidgetManager)
{
	m_chatWidgetManager = chatWidgetManager;
}

void KaduWindow::setExitAction(ExitAction *exitAction)
{
	m_exitAction = exitAction;
}

void KaduWindow::setFileTransferManager(FileTransferManager *fileTransferManager)
{
	m_fileTransferManager = fileTransferManager;
}

void KaduWindow::setMainWindowRepository(MainWindowRepository *mainWindowRepository)
{
	m_mainWindowRepository = mainWindowRepository;
}

void KaduWindow::setMenuInventory(MenuInventory *menuInventory)
{
	m_menuInventory = menuInventory;
}

void KaduWindow::setMyself(Myself *myself)
{
	m_myself = myself;
}

void KaduWindow::setOpenChatWithAction(OpenChatWithAction *openChatWithAction)
{
	m_openChatWithAction = openChatWithAction;
}

void KaduWindow::setOpenForumAction(OpenForumAction *openForumAction)
{
	m_openForumAction = openForumAction;
}

void KaduWindow::setOpenGetInvolvedAction(OpenGetInvolvedAction *openGetInvolvedAction)
{
	m_openGetInvolvedAction = openGetInvolvedAction;
}

void KaduWindow::setOpenRedmineAction(OpenRedmineAction *openRedmineAction)
{
	m_openRedmineAction = openRedmineAction;
}

void KaduWindow::setOpenSearchAction(OpenSearchAction *openSearchAction)
{
	m_openSearchAction = openSearchAction;
}

void KaduWindow::setOpenTranslateAction(OpenTranslateAction *openTranslateAction)
{
	m_openTranslateAction = openTranslateAction;
}

void KaduWindow::setRecentChatsAction(RecentChatsAction *recentChatsAction)
{
	m_recentChatsAction = recentChatsAction;
}

void KaduWindow::setShowAboutWindowAction(ShowAboutWindowAction *showAboutWindowAction)
{
	m_showAboutWindowAction = showAboutWindowAction;
}

void KaduWindow::setShowBlockedBuddiesAction(ShowBlockedBuddiesAction *showBlockedBuddiesAction)
{
	m_showBlockedBuddiesAction = showBlockedBuddiesAction;
}

void KaduWindow::setShowConfigurationWindowAction(ShowConfigurationWindowAction *showConfigurationWindowAction)
{
	m_showConfigurationWindowAction = showConfigurationWindowAction;
}

void KaduWindow::setShowInfoPanelAction(ShowInfoPanelAction *showInfoPanelAction)
{
	m_showInfoPanelAction = showInfoPanelAction;
}

void KaduWindow::setShowMultilogonsAction(ShowMultilogonsAction *showMultilogonsAction)
{
	m_showMultilogonsAction = showMultilogonsAction;
}

void KaduWindow::setShowMyselfAction(ShowMyselfAction *showMyselfAction)
{
	m_showMyselfAction = showMyselfAction;
}

void KaduWindow::setShowOfflineBuddiesAction(ShowOfflineBuddiesAction *showOfflineBuddiesAction)
{
	m_showOfflineBuddiesAction = showOfflineBuddiesAction;
}

void KaduWindow::setShowYourAccountsAction(ShowYourAccountsAction *showYourAccountsAction)
{
	m_showYourAccountsAction = showYourAccountsAction;
}

void KaduWindow::setStatusContainerManager(StatusContainerManager *statusContainerManager)
{
	m_statusContainerManager = statusContainerManager;
}

void KaduWindow::setTalkableConverter(TalkableConverter *talkableConverter)
{
	m_talkableConverter = talkableConverter;
}

void KaduWindow::setUrlHandlerManager(UrlHandlerManager *urlHandlerManager)
{
	m_urlHandlerManager = urlHandlerManager;
}

void KaduWindow::setUrlOpener(UrlOpener *urlOpener)
{
	m_urlOpener = urlOpener;
}

void KaduWindow::init()
{
	setWindowRole("kadu-main");

	setActionContext(new ProxyActionContext(m_statusContainerManager));

#ifdef Q_OS_WIN
	HiddenParent = new QWidget();
	setHiddenParent();
#endif

	setWindowTitle(QStringLiteral("Kadu"));

	// we need to create gui first, then actions, then menus
	// TODO: fix it in 0.10 or whenever
	createGui();

	Context = static_cast<ProxyActionContext *>(actionContext());
	Context->setForwardActionContext(Roster->actionContext());

	loadToolBarsFromConfig();
	createMenu();

	configurationUpdated();

	new WindowGeometryManager(new ConfigFileVariantWrapper(configuration(), "General", "Geometry"), QRect(0, 50, 350, 650), this);
	m_mainWindowRepository->addMainWindow(this);
}

void KaduWindow::done()
{
	m_mainWindowRepository->removeMainWindow(this);
}

void KaduWindow::createGui()
{
	MainWidget = new QWidget(this);
	MainLayout = new QVBoxLayout(MainWidget);
	MainLayout->setMargin(0);
	MainLayout->setSpacing(0);

	Split = new QSplitter(Qt::Vertical, MainWidget);

	Roster = injectedFactory()->makeInjected<RosterWidget>(Split);
	InfoPanel = injectedFactory()->makeInjected<BuddyInfoPanel>(Split);

	connect(Roster, SIGNAL(currentChanged(Talkable)), InfoPanel, SLOT(displayItem(Talkable)));
	connect(Roster, SIGNAL(talkableActivated(Talkable)), this, SLOT(talkableActivatedSlot(Talkable)));

	ChangeStatusButtons = injectedFactory()->makeInjected<StatusButtons>(MainWidget);

	if (!configuration()->deprecatedApi()->readBoolEntry("Look", "ShowInfoPanel"))
		InfoPanel->setVisible(false);
	if (!configuration()->deprecatedApi()->readBoolEntry("Look", "ShowStatusButton"))
		ChangeStatusButtons->hide();

	QList<int> splitSizes;

	splitSizes.append(configuration()->deprecatedApi()->readNumEntry("General", "UserBoxHeight"));
	splitSizes.append(configuration()->deprecatedApi()->readNumEntry("General", "DescriptionHeight"));

	Split->setSizes(splitSizes);

	MainLayout->addWidget(Split);
	MainLayout->addWidget(ChangeStatusButtons);

	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	setCentralWidget(MainWidget);
	setFocusProxy(Roster);
	Roster->setFocus();
}

void KaduWindow::createMenu()
{
	createKaduMenu();
	createContactsMenu();
	createToolsMenu();
	createHelpMenu();
}

void KaduWindow::createKaduMenu()
{
	KaduMenu = new QMenu(this);
	m_menuInventory->menu("main")->attachToMenu(KaduMenu);
	m_menuInventory->menu("main")
		->addAction(m_showConfigurationWindowAction, KaduMenu::SectionConfig, 30)
		->addAction(m_showYourAccountsAction, KaduMenu::SectionConfig, 29)
		->addAction(m_recentChatsAction, KaduMenu::SectionRecentChats, 28)
		->addAction(m_exitAction, KaduMenu::SectionQuit)
		->update();

	KaduMenu->setTitle("&Kadu");

	menuBar()->addMenu(KaduMenu);
}

void KaduWindow::createContactsMenu()
{
	ContactsMenu = new QMenu(this);
	ContactsMenu->setTitle(tr("&Buddies"));

	m_menuInventory->menu("buddy")->attachToMenu(ContactsMenu);
	m_menuInventory->menu("buddy")
		->addAction(m_addUserAction, KaduMenu::SectionBuddies, 50)
		->addAction(m_addConferenceAction, KaduMenu::SectionBuddies, 40)
		->addAction(m_addRoomChatAction, KaduMenu::SectionBuddies, 30)
		->addAction(m_addGroupAction, KaduMenu::SectionBuddies, 20)
		->addAction(m_openSearchAction, KaduMenu::SectionBuddies, 10)
		->addAction(m_openChatWithAction, KaduMenu::SectionOpenChat)
		->addAction(m_showOfflineBuddiesAction, KaduMenu::SectionBuddyListFilters, 4)
		->addAction(m_showBlockedBuddiesAction, KaduMenu::SectionBuddyListFilters, 3)
		->addAction(m_showMyselfAction, KaduMenu::SectionBuddyListFilters, 2)
		->addAction(m_showInfoPanelAction, KaduMenu::SectionBuddyListFilters, 1)
		->update();

	menuBar()->addMenu(ContactsMenu);
}

void KaduWindow::createToolsMenu()
{
	ToolsMenu = new QMenu(this);
	ToolsMenu->setTitle(tr("&Tools"));

	m_menuInventory->menu("tools")->attachToMenu(ToolsMenu);
	m_menuInventory->menu("tools")
		->addAction(m_showMultilogonsAction, KaduMenu::SectionTools, 1)
		->update();

	menuBar()->addMenu(ToolsMenu);
}

void KaduWindow::createHelpMenu()
{
	HelpMenu = new QMenu(this);
	HelpMenu->setTitle(tr("&Help"));

	m_menuInventory->menu("help")->attachToMenu(HelpMenu);
	m_menuInventory->menu("help")
		->addAction(m_openForumAction, KaduMenu::SectionHelp, 2)
		->addAction(m_openRedmineAction, KaduMenu::SectionHelp, 1)
		->addAction(m_openGetInvolvedAction, KaduMenu::SectionGetInvolved, 2)
		->addAction(m_openTranslateAction, KaduMenu::SectionGetInvolved, 1)
		->addAction(m_showAboutWindowAction, KaduMenu::SectionAbout, 1)
		->update();

	menuBar()->addMenu(HelpMenu);
}

void KaduWindow::compositingEnabled()
{
	if (!configuration()->deprecatedApi()->readBoolEntry("Look", "UserboxTransparency"))
	{
		compositingDisabled();
		return;
	}

	if (CompositingEnabled)
		return;

	CompositingEnabled = true;
	setTransparency(true);
	menuBar()->setAutoFillBackground(true);
	InfoPanel->setAutoFillBackground(true);
	ChangeStatusButtons->setAutoFillBackground(true);
	for (int i = 1; i < Split->count(); ++i)
	{
		QSplitterHandle *splitterHandle = Split->handle(i);
		splitterHandle->setAutoFillBackground(true);
	}

	configurationUpdated();
}

void KaduWindow::compositingDisabled()
{
	if (!CompositingEnabled)
		return;

	CompositingEnabled = false;
	menuBar()->setAutoFillBackground(false);
	InfoPanel->setAutoFillBackground(false);
	ChangeStatusButtons->setAutoFillBackground(false);
	for (int i = 1; i < Split->count(); ++i)
	{
		QSplitterHandle *splitterHandle = Split->handle(i);
		splitterHandle->setAutoFillBackground(false);
	}
	setTransparency(false);

	configurationUpdated();
}

void KaduWindow::talkableActivatedSlot(const Talkable &talkable)
{
	auto buddy = m_talkableConverter->toBuddy(talkable);
	if (buddy.isTemporary())
		return;

	auto chat = m_talkableConverter->toChat(talkable);
	if (chat && !chat.contacts().toBuddySet().contains(m_myself->buddy()))
	{
		m_chatWidgetManager->openChat(chat, OpenChatActivation::Activate);
		return;
	}

	if (buddy.contacts().isEmpty() && buddy.mobile().isEmpty() && !buddy.email().isEmpty())
		if (buddy.email().indexOf(m_urlHandlerManager->mailRegExp()) == 0)
			m_urlOpener->openEmail(buddy.email().toUtf8());

	emit talkableActivated(talkable);
}

void KaduWindow::storeConfiguration()
{
#ifndef Q_OS_WIN
	// see bug 1948 - this is a hack to get real values of info panel height
	if (!isVisible())
	{
		show();
		hide();
	}
#endif

	if (configuration()->deprecatedApi()->readBoolEntry("Look", "ShowInfoPanel"))
	{
		configuration()->deprecatedApi()->writeEntry("General", "UserBoxHeight", Roster->size().height());
		configuration()->deprecatedApi()->writeEntry("General", "DescriptionHeight", InfoPanel->size().height());
	}
	if (configuration()->deprecatedApi()->readBoolEntry("Look", "ShowStatusButton"))
		configuration()->deprecatedApi()->writeEntry("General", "UserBoxHeight", Roster->size().height());
}

void KaduWindow::closeEvent(QCloseEvent *e)
{
	if (!isWindow())
	{
		e->ignore();
		return;
	}

	// do not block window closing when session is about to close
	if (m_application->isSavingSession())
	{
		MainWindow::closeEvent(e);
		return;
	}

	if (Docked)
	{
		e->ignore();
		hide();
	}
	else
	{
		MainWindow::closeEvent(e);
		m_application->quit();
	}
}

void KaduWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		if (Docked && isWindow())
		{
			kdebugm(KDEBUG_INFO, "Kadu::keyPressEvent(Key_Escape): Kadu hide\n");
			hide();
			return;
		}
	}
	else if (e->matches(QKeySequence::Copy) && !InfoPanel->selectedText().isEmpty())
		// Do not use triggerPageAction(), see bug #2345.
		InfoPanel->pageAction(QWebPage::Copy)->trigger();

	emit keyPressed(e);

	MainWindow::keyPressEvent(e);
}

#ifdef Q_OS_WIN
/* On Windows the only way to not show a window in the taskbar without making it a toolwindow
 * is to turn off the WS_EX_APPWINDOW style and provide it with a parent (which will be hidden
 * in our case).
 */
void KaduWindow::setHiddenParent()
{
	QWidget *futureChild = window();
	bool wasVisible = futureChild->isVisible();
	Qt::WindowFlags previousFlags = futureChild->windowFlags();
	futureChild->setParent(HiddenParent);
	futureChild->setWindowFlags(previousFlags);
	futureChild->setVisible(wasVisible);

	hideWindowFromTaskbar();
}

void KaduWindow::hideWindowFromTaskbar()
{
	auto *w = window();
	auto newWindowLongPtr = GetWindowLongPtr(reinterpret_cast<HWND>(w->winId()), GWL_EXSTYLE);
	auto hideFromTaskbar = configuration()->deprecatedApi()->readBoolEntry("General", "HideMainWindowFromTaskbar");
	if (hideFromTaskbar == !(newWindowLongPtr & WS_EX_APPWINDOW))
		return;

	if (hideFromTaskbar)
		newWindowLongPtr &= ~WS_EX_APPWINDOW;
	else
		newWindowLongPtr |= WS_EX_APPWINDOW;

	auto wasVisible = w->isVisible();
	w->setVisible(false);
	SetWindowLongPtr(reinterpret_cast<HWND>(w->winId()), GWL_EXSTYLE, newWindowLongPtr);
	w->setVisible(wasVisible);
}
#endif

void KaduWindow::changeEvent(QEvent *event)
{
	MainWindow::changeEvent(event);
	if (event->type() == QEvent::ActivationChange)
	{
		if (!_isActiveWindow(this))
			Roster->clearFilter();
	}
#ifdef Q_OS_WIN
	else if (event->type() == QEvent::WindowStateChange)
	{
		if (Docked && isMinimized() && configuration()->deprecatedApi()->readBoolEntry("General", "HideMainWindowFromTaskbar"))
			QMetaObject::invokeMethod(this, "hide", Qt::QueuedConnection);
	}
#endif
	else if (event->type() == QEvent::ParentChange)
	{
		QWidget *previousWindowParent = WindowParent;
		WindowParent = (window() != this) ? window() : 0;
		if (previousWindowParent != WindowParent)
		{
			// On Windows we reparent WindowParent, so we want it to be parentless now.
			// BTW, if WindowParent would be really needed in future, it's quite easy to support it.
			Q_ASSERT(!WindowParent || 0 == WindowParent->parentWidget());
#ifdef Q_OS_WIN
			// Without QueuedConnection I hit infinite loop here.
			QMetaObject::invokeMethod(this, "setHiddenParent", Qt::QueuedConnection);
#endif
			emit parentChanged(WindowParent);
		}
	}
}

bool KaduWindow::supportsActionType(ActionDescription::ActionType type)
{
	return type & (ActionDescription::TypeGlobal | ActionDescription::TypeUserList | ActionDescription::TypeUser);
}

TalkableTreeView * KaduWindow::talkableTreeView()
{
	return Roster->talkableTreeView();
}

TalkableProxyModel * KaduWindow::talkableProxyModel()
{
	return Roster->talkableProxyModel();
}

void KaduWindow::configurationUpdated()
{
#ifdef Q_OS_WIN
	hideWindowFromTaskbar();
#endif

	setDocked(Docked);

	ChangeStatusButtons->setVisible(configuration()->deprecatedApi()->readBoolEntry("Look", "ShowStatusButton"));

	triggerCompositingStateChanged();
	setBlur(configuration()->deprecatedApi()->readBoolEntry("Look", "UserboxTransparency") && configuration()->deprecatedApi()->readBoolEntry("Look", "UserboxBlur"));
}

void KaduWindow::createDefaultToolbars(Configuration *configuration, QDomElement parentConfig)
{
	QDomElement dockAreaConfig = getDockAreaConfigElement(configuration, parentConfig, "topDockArea");
	QDomElement toolbarConfig = configuration->api()->createElement(dockAreaConfig, "ToolBar");

	addToolButton(configuration, toolbarConfig, "addUserAction", Qt::ToolButtonTextUnderIcon);
	addToolButton(configuration, toolbarConfig, "addGroupAction", Qt::ToolButtonTextUnderIcon);
	addToolButton(configuration, toolbarConfig, "muteSoundsAction", Qt::ToolButtonTextUnderIcon);
}

void KaduWindow::setDocked(bool docked)
{
	Docked = docked;
}

#include "moc_kadu-window.cpp"
