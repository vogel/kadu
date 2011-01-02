/*
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtGui/QApplication>
#include <QtGui/QCloseEvent>
#include <QtGui/QHBoxLayout>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QScrollBar>
#include <QtGui/QSplitter>

#include "accounts/account-manager.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-set.h"
#include "buddies/buddy-shared.h"
#include "buddies/model/buddies-model.h"
#include "buddies/filter/anonymous-without-messages-buddy-filter.h"
#include "buddies/filter/group-buddy-filter.h"
#include "chat/type/chat-type-manager.h"
#include "chat/chat-manager.h"
#include "chat/recent-chat-manager.h"
#include "configuration/configuration-file.h"
#include "contacts/contact.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "core/core.h"
#include "gui/hot-key.h"
#include "gui/actions/action.h"
#include "gui/widgets/buddy-info-panel.h"
#include "gui/widgets/buddies-list-view.h"
#include "gui/widgets/buddies-list-widget.h"
#include "gui/widgets/chat-widget-actions.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/group-tab-bar.h"
#include "gui/widgets/kadu-web-view.h"
#include "gui/windows/kadu-window-actions.h"
#include "gui/widgets/status-buttons.h"
#include "gui/widgets/status-menu.h"
#include "notify/notification-manager.h"
#include "activate.h"

#include "misc/misc.h"
#include "debug.h"
#include "icons-manager.h"

#include "kadu-window.h"

#ifdef Q_OS_MAC
extern void qt_mac_set_menubar_icons(bool enable);
#endif

KaduWindow::KaduWindow(QWidget *parent) :
		MainWindow(QString(), parent), Docked(false), ContactsWidget(0), CompositingEnabled(false)
{
	setWindowRole("kadu-main");

	WindowParent = parent;

#ifdef Q_OS_MAC
	/* Dorr: workaround for Qt window geometry bug when unified toolbars enabled */
	setUnifiedTitleAndToolBarOnMac(false);
	MenuBar = new QMenuBar(0); // TODO: couldn't it have "this" as parent?
#endif

	setAttribute(Qt::WA_DeleteOnClose, true);
	setWindowTitle(QLatin1String("Kadu"));

	// we need to create gui first, then actions, then menus
	// TODO: fix it in 0.8 or whenever
	createGui();
	Actions = new KaduWindowActions(this);
	loadToolBarsFromConfig();
	createMenu();

	configurationUpdated();

	loadWindowGeometry(this, "General", "Geometry", 0, 50, 255, 565);

#if defined(Q_OS_MAC)
	/* Dorr: workaround for Qt window geometry bug when unified toolbars enabled */
	setUnifiedTitleAndToolBarOnMac(true);
#endif
}

KaduWindow::~KaduWindow()
{
	storeConfiguration();
}

void KaduWindow::createGui()
{
	MainWidget = new QWidget();
	MainLayout = new QVBoxLayout(MainWidget);
	MainLayout->setMargin(0);
	MainLayout->setSpacing(0);

	Split = new QSplitter(Qt::Vertical, this);
	MainLayout->addWidget(Split);

	QWidget* hbox = new QWidget(Split);
	QHBoxLayout *hboxLayout = new QHBoxLayout(hbox);
	hboxLayout->setMargin(0);
	hboxLayout->setSpacing(0);

	/* Must create empty widget so it can fill its background. */
	GroupBarWidget = new QWidget(this);
	QVBoxLayout *GroupBarLayout = new QVBoxLayout(GroupBarWidget);
	GroupBarLayout->setMargin(0);
	GroupBarLayout->setSpacing(0);
	GroupBarWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

	GroupBar = new GroupTabBar(this);

	GroupBarLayout->addWidget(GroupBar, 0, Qt::AlignTop);
	GroupBarWidget->setLayout(GroupBarLayout);

	ContactsWidget = new BuddiesListWidget(BuddiesListWidget::FilterAtTop, hbox);
	ContactsWidget->view()->useConfigurationColors(true);
	ContactsWidget->view()->setModel(new BuddiesModel(this));
	ContactsWidget->view()->addFilter(GroupBar->filter());
	AnonymousWithoutMessagesBuddyFilter *anonymousFilter = new AnonymousWithoutMessagesBuddyFilter(this);
	anonymousFilter->setEnabled(true);
	ContactsWidget->view()->addFilter(anonymousFilter);
	ContactsWidget->view()->setContextMenuEnabled(true);

	connect(ContactsWidget->view(), SIGNAL(chatActivated(Chat )), this, SLOT(openChatWindow(Chat )));

	hboxLayout->addWidget(GroupBarWidget);
	hboxLayout->setStretchFactor(GroupBarWidget, 1);
	hboxLayout->addWidget(ContactsWidget);
	hboxLayout->setStretchFactor(ContactsWidget, 100);
	hboxLayout->setAlignment(GroupBar, Qt::AlignTop);

	InfoPanel = new BuddyInfoPanel(Split);
	connect(ContactsWidget->view(), SIGNAL(currentChanged(BuddyOrContact)), InfoPanel, SLOT(displayItem(BuddyOrContact)));

	if (!config_file.readBoolEntry("Look", "ShowInfoPanel"))
		InfoPanel->setVisible(false);;

	ChangeStatusButtons = new StatusButtons(this);
	MainLayout->addWidget(ChangeStatusButtons);

	if (!config_file.readBoolEntry("Look", "ShowStatusButton"))
		ChangeStatusButtons->hide();

	QList<int> splitSizes;

	splitSizes.append(config_file.readNumEntry("General", "UserBoxHeight"));
	splitSizes.append(config_file.readNumEntry("General", "DescriptionHeight"));

	Split->setSizes(splitSizes);

	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	setCentralWidget(MainWidget);
}

void KaduWindow::createMenu()
{
#ifdef Q_OS_MAC
	qt_mac_set_menubar_icons(false);
#endif
	createKaduMenu();
	createContactsMenu();
	createHelpMenu();
}

void KaduWindow::createKaduMenu()
{
	KaduMenu = new QMenu(this);
#ifdef Q_OS_MAC
	KaduMenu->setTitle(tr("General"));
#else
	KaduMenu->setTitle("&Kadu");
#endif
	RecentChatsMenu = new QMenu(this);
	RecentChatsMenu->setIcon(IconsManager::instance()->iconByPath("internet-group-chat"));
	RecentChatsMenu->setTitle(tr("Recent chats"));
	connect(KaduMenu, SIGNAL(aboutToShow()), this, SLOT(createRecentChatsMenu()));
	connect(RecentChatsMenu, SIGNAL(triggered(QAction *)), this, SLOT(openRecentChats(QAction *)));

	insertMenuActionDescription(Actions->Configuration, MenuKadu);
	insertMenuActionDescription(Actions->ShowYourAccounts, MenuKadu);
	insertMenuActionDescription(Actions->ManageModules, MenuKadu);

	KaduMenu->addSeparator();
	RecentChatsMenuAction = KaduMenu->addMenu(RecentChatsMenu);
	KaduMenu->addSeparator();

	insertMenuActionDescription(NotificationManager::instance()->silentModeActionDescription(), MenuKadu);
	KaduMenu->addSeparator();

	insertMenuActionDescription(Actions->ExitKadu, MenuKadu);

	menuBar()->addMenu(KaduMenu);
}

void KaduWindow::createContactsMenu()
{
	ContactsMenu = new QMenu(this);
	ContactsMenu->setTitle(tr("&Buddies"));

	insertMenuActionDescription(Actions->AddUser, MenuContacts);
	insertMenuActionDescription(Actions->AddGroup, MenuContacts);
	insertMenuActionDescription(Actions->OpenSearch, MenuContacts);

	ContactsMenu->addSeparator();
	insertMenuActionDescription(ChatWidgetManager::instance()->actions()->openChatWith(), MenuContacts);

	ContactsMenu->addSeparator();
	insertMenuActionDescription(Actions->InactiveUsers, MenuContacts);
	insertMenuActionDescription(Actions->ShowBlockedBuddies, MenuContacts);
	insertMenuActionDescription(Actions->ShowInfoPanel, MenuContacts);

	menuBar()->addMenu(ContactsMenu);
}

void KaduWindow::createHelpMenu()
{
	HelpMenu = new QMenu(this);
	HelpMenu->setTitle(tr("&Help"));

	insertMenuActionDescription(Actions->Help, MenuHelp);
	insertMenuActionDescription(Actions->Bugs, MenuHelp);
	HelpMenu->addSeparator();
	insertMenuActionDescription(Actions->GetInvolved, MenuHelp);
	insertMenuActionDescription(Actions->Support, MenuHelp);
	insertMenuActionDescription(Actions->Translate, MenuHelp);
	HelpMenu->addSeparator();
	insertMenuActionDescription(Actions->About, MenuHelp);

	menuBar()->addMenu(HelpMenu);
}

#ifdef Q_OS_MAC
QMenuBar* KaduWindow::menuBar() const
{
	return MenuBar;
}
#endif

void KaduWindow::compositingEnabled()
{
	if (config_file.readBoolEntry("Look", "UserboxTransparency"))
	{
		if (!CompositingEnabled)
		{
			CompositingEnabled = true;
			setTransparency(true);
			menuBar()->setAutoFillBackground(true);
			GroupBarWidget->setAutoFillBackground(true);
			InfoPanel->setAutoFillBackground(true);
			ChangeStatusButtons->setAutoFillBackground(true);
			ContactsWidget->nameFilterWidget()->setAutoFillBackground(true);
			ContactsWidget->view()->verticalScrollBar()->setAutoFillBackground(true);
			// TODO: find a way to paint this QFrame outside its viewport still allowing the viewport to be transparent
			ContactsWidget->view()->setFrameShape(QFrame::NoFrame);
			for (int i = 1; i < Split->count(); ++i)
			{
				QSplitterHandle *splitterHandle = Split->handle(i);
				splitterHandle->setAutoFillBackground(true);
			}
			configurationUpdated();
		}
	}
	else
		compositingDisabled();
}

void KaduWindow::compositingDisabled()
{
	if (CompositingEnabled)
	{
		CompositingEnabled = false;
		menuBar()->setAutoFillBackground(false);
		GroupBarWidget->setAutoFillBackground(false);
		InfoPanel->setAutoFillBackground(false);
		ChangeStatusButtons->setAutoFillBackground(false);
		ContactsWidget->nameFilterWidget()->setAutoFillBackground(false);
		ContactsWidget->view()->verticalScrollBar()->setAutoFillBackground(false);
		ContactsWidget->view()->setFrameShape(QFrame::StyledPanel);
		for (int i = 1; i < Split->count(); ++i)
		{
			QSplitterHandle *splitterHandle = Split->handle(i);
			splitterHandle->setAutoFillBackground(false);
		}
		setTransparency(false);
		configurationUpdated();
	}
}

void KaduWindow::openChatWindow(Chat chat)
{
	if (!chat.contacts().toBuddySet().contains(Core::instance()->myself()))
	{
		ChatWidgetManager::instance()->sendMessage(chat);
		return;
	}

// TODO: 0.6.6
// 	contact = *contacts.begin();
// 	if (contact.mobile().isEmpty() && !contact.email().isEmpty())
// 		openMailClient(contact.email());
}

void KaduWindow::createRecentChatsMenu()
{
	kdebugf();

	RecentChatsMenu->clear();

	QList<Chat> recentChats = RecentChatManager::instance()->recentChats();
	bool addedAnyChat = false;
	foreach (const Chat chat, recentChats)
		if (!ChatWidgetManager::instance()->byChat(chat))
		{
			ChatType *type = ChatTypeManager::instance()->chatType(chat.type());
			QAction *action = new QAction(type ? type->icon() : QIcon(), chat.name(), this);
			action->setData(QVariant::fromValue<Chat>(chat));
			RecentChatsMenu->addAction(action);

			addedAnyChat = true;
		}

	RecentChatsMenuAction->setEnabled(addedAnyChat);

	kdebugf2();
}

void KaduWindow::openRecentChats(QAction *action)
{
	kdebugf();
	ChatWidgetManager::instance()->openPendingMsgs(action->data().value<Chat>(), true);
	kdebugf2();
}

void KaduWindow::storeConfiguration()
{
#ifdef Q_OS_MAC
	/* Dorr: workaround for Qt window geometry bug when unified toolbars enabled */
	setUnifiedTitleAndToolBarOnMac(false);
#endif
	saveWindowGeometry(this, "General", "Geometry");
#ifdef Q_OS_MAC
	/* Dorr: workaround for Qt window geometry bug when unified toolbars enabled */
	setUnifiedTitleAndToolBarOnMac(true);
#endif

	if (config_file.readBoolEntry("Look", "ShowInfoPanel"))
	{
		config_file.writeEntry("General", "UserBoxHeight", ContactsWidget->size().height());
		config_file.writeEntry("General", "DescriptionHeight", InfoPanel->size().height());
	}
	if (config_file.readBoolEntry("Look", "ShowStatusButton"))
		config_file.writeEntry("General", "UserBoxHeight", ContactsWidget->size().height());

// TODO: 0.6.6
//	config_file.writeEntry("General", "DefaultDescription", defaultdescriptions.join("<-->"));
}

void KaduWindow::closeEvent(QCloseEvent *e)
{
	e->ignore();

	if (Docked)
		hide();
	else
		qApp->quit();
}

void KaduWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		if (Docked)
		{
			kdebugm(KDEBUG_INFO, "Kadu::keyPressEvent(Key_Escape): Kadu hide\n");
			hide();
		}
	}
	// TODO: 0.6.6 THIS SUXX
	// after action moving this could be restored
	// else if (HotKey::shortCut(e,"ShortCuts", "kadu_deleteuser"))
	//	deleteUsersActionDescription->createAction(this)->trigger();
	else if (e->matches(QKeySequence::Copy))
		InfoPanel->pageAction(QWebPage::Copy)->trigger();

	emit keyPressed(e);

	MainWindow::keyPressEvent(e);
}

void KaduWindow::changeEvent(QEvent *event)
{
	MainWindow::changeEvent(event);
	if (event->type() == QEvent::ActivationChange)
	{
		if (!_isActiveWindow(this))
			ContactsWidget->clearFilter();
	}
	if (event->type() == QEvent::ParentChange)
	{
		emit parentChanged(WindowParent);
		WindowParent = parentWidget();
	}
}

bool KaduWindow::supportsActionType(ActionDescription::ActionType type)
{
	return type & (ActionDescription::TypeGlobal | ActionDescription::TypeUserList | ActionDescription::TypeUser);
}

BuddiesListView * KaduWindow::buddiesListView()
{
	return ContactsWidget
			? ContactsWidget->view()
			: 0;
}

StatusContainer * KaduWindow::statusContainer()
{
	return 0;
}

ContactSet KaduWindow::contacts()
{
	return ContactsWidget
			? ContactsWidget->view()->selectedContacts()
			: ContactSet();
}

BuddySet KaduWindow::buddies()
{
	return ContactsWidget
			? ContactsWidget->view()->selectedBuddies()
			: BuddySet();
}

Chat KaduWindow::chat()
{
	return ContactsWidget
			? ContactsWidget->view()->currentChat()
			: Chat::null;
}

bool KaduWindow::hasContactSelected()
{
	return ContactsWidget
			? ContactsWidget->view()->hasContactSelected()
			: false;
}

void KaduWindow::configurationUpdated()
{
	QString bgColor = config_file.readColorEntry("Look","UserboxBgColor").name();
	QString alternateBgColor = config_file.readColorEntry("Look","UserboxAlternateBgColor").name();
	QFont userboxFont = QFont(config_file.readFontEntry("Look", "UserboxFont"));
	GroupBar->setFont(QFont(userboxFont.family(), userboxFont.pointSize(), 75));

	setDocked(Docked);

	if (CompositingEnabled && config_file.readBoolEntry("Look", "UserboxTransparency"))
	{
		int alpha = config_file.readNumEntry("Look", "UserboxAlpha");

		QColor color(bgColor);
		bgColor = QString("rgba(%1,%2,%3,%4)").arg(color.red()).arg(color.green()).arg(color.blue()).arg(alpha);

		color = QColor(alternateBgColor);
		alternateBgColor = QString("rgba(%1,%2,%3,%4)").arg(color.red()).arg(color.green()).arg(color.blue()).arg(alpha);

		if (!bgColor.compare(alternateBgColor))
			alternateBgColor = QString("transparent");
	}

	if (config_file.readBoolEntry("Look", "UseUserboxBackground", true))
	{
		QString type = config_file.readEntry("Look", "UserboxBackgroundDisplayStyle");
		ContactsWidget->view()->setBackground(bgColor, alternateBgColor,
			config_file.readEntry("Look", "UserboxBackground"),
			type == "Centered" ? BuddiesListView::BackgroundCentered
			: type == "Tiled" ? BuddiesListView::BackgroundTiled
			: type == "Stretched" ? BuddiesListView::BackgroundStretched
			: type == "TiledAndCentered" ? BuddiesListView::BackgroundTiledAndCentered
			: BuddiesListView::BackgroundNone);
	}
	else
		ContactsWidget->view()->setBackground(bgColor, alternateBgColor);

	ChangeStatusButtons->setVisible(config_file.readBoolEntry("Look", "ShowStatusButton"));

	triggerCompositingStateChanged();
}

void KaduWindow::insertMenuActionDescription(ActionDescription *actionDescription, MenuType type, int pos)
{
	kdebugf();
	if (!actionDescription)
		return;
	Action *action = actionDescription->createAction(this, this);

	QMenu *menu = 0;

	switch (type)
	{
		case MenuKadu:
			menu = KaduMenu;
			break;
		case MenuContacts:
			menu = ContactsMenu;
			break;
		case MenuHelp:
			menu = HelpMenu;
	}

	if (!menu)
		return;

	QList<QAction *> menuActions = menu->actions();
	if (pos >= menuActions.count() - 1 || pos == -1)
		menu->addAction(action);
	else
		menu->insertAction(menuActions[pos], action);

	MenuActions[actionDescription] = MenuAction(action, type);
}

void KaduWindow::removeMenuActionDescription(ActionDescription *actionDescription)
{
	if (!actionDescription)
		return;
	Action *action = MenuActions[actionDescription].first;

	if (!action)
		return;
	switch (MenuActions[actionDescription].second)
	{
		case MenuKadu:
			KaduMenu->removeAction(action);
			break;
		case MenuContacts:
			ContactsMenu->removeAction(action);
			break;
		case MenuHelp:
			HelpMenu->removeAction(action);
	}
	MenuActions.remove(actionDescription);
}

void KaduWindow::createDefaultToolbars(QDomElement parentConfig)
{
	QDomElement dockAreaConfig = getDockAreaConfigElement(parentConfig, "topDockArea");
	QDomElement toolbarConfig = xml_config_file->createElement(dockAreaConfig, "ToolBar");

#ifdef Q_WS_MAEMO_5
	addToolButton(toolbarConfig, "addUserAction", Qt::ToolButtonIconOnly);
	addToolButton(toolbarConfig, "addGroupAction", Qt::ToolButtonIconOnly);
	addToolButton(toolbarConfig, "muteSoundsAction", Qt::ToolButtonIconOnly);
#else
	addToolButton(toolbarConfig, "addUserAction", Qt::ToolButtonTextUnderIcon);
	addToolButton(toolbarConfig, "addGroupAction", Qt::ToolButtonTextUnderIcon);
	addToolButton(toolbarConfig, "muteSoundsAction", Qt::ToolButtonTextUnderIcon);
#endif
}

void KaduWindow::addAction(const QString &actionName, Qt::ToolButtonStyle style)
{
	addToolButton(findExistingToolbar(QString()), actionName, style);
	Core::instance()->kaduWindow()->refreshToolBars();
}

ActionDataSource * KaduWindow::actionSource()
{
	return buddiesListView();
}

void KaduWindow::setDocked(bool docked)
{
	Docked = docked;
	qApp->setQuitOnLastWindowClosed(!Docked);

// TODO: 0.6.6
// 	if (config_file.readBoolEntry("General", "ShowAnonymousWithMsgs") || !Docked || dontHideOnClose)
// 	{
// 	Userbox->removeNegativeFilter(anonymousUsers);
// 	Userbox->applyNegativeFilter(anonymousUsersWithoutMessages);
// 	}
// 	else
// 	{
// 		Userbox->removeNegativeFilter(anonymousUsersWithoutMessages);
// 		Userbox->applyNegativeFilter(anonymousUsers);
// 	}
}
