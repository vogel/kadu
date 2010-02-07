/*
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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
#include "gui/widgets/kadu-text-browser.h"
#include "gui/windows/kadu-window-actions.h"
#include "gui/widgets/status-buttons.h"
#include "gui/widgets/status-menu.h"

#include "misc/misc.h"
#include "debug.h"
#include "icons-manager.h"

#include "kadu-window.h"

KaduWindow::KaduWindow(QWidget *parent) :
		MainWindow(parent), Docked(false)
{
	setAttribute(Qt::WA_DeleteOnClose, true);

	Actions = new KaduWindowActions(this);

	createGui();
	loadToolBarsFromConfig("");
	configurationUpdated();

	loadWindowGeometry(this, "General", "Geometry", 0, 50, 205, 465);
}

KaduWindow::~KaduWindow()
{
	storeConfiguration();
}

void KaduWindow::createGui()
{
	createMenu();

	MainWidget = new QWidget();
	MainLayout = new QVBoxLayout(MainWidget);
	MainLayout->setMargin(0);
	MainLayout->setSpacing(0);

	QSplitter *split = new QSplitter(Qt::Vertical, this);
	MainLayout->addWidget(split);

	QWidget* hbox = new QWidget(split);
	QHBoxLayout *hboxLayout = new QHBoxLayout(hbox);
	hboxLayout->setMargin(0);
	hboxLayout->setSpacing(0);

	// groupbar
	GroupBar = new GroupTabBar(this);

	ContactsWidget = new BuddiesListWidget(BuddiesListWidget::FilterAtTop, this);
	ContactsWidget->view()->setModel(new BuddiesModel(BuddyManager::instance(), this));
	ContactsWidget->view()->addFilter(GroupBar->filter());
	AnonymousWithoutMessagesBuddyFilter *anonymousFilter = new AnonymousWithoutMessagesBuddyFilter(this);
	anonymousFilter->setEnabled(true);
	ContactsWidget->view()->addFilter(anonymousFilter);

	connect(ContactsWidget->view(), SIGNAL(chatActivated(Chat )), this, SLOT(openChatWindow(Chat )));

	hboxLayout->addWidget(GroupBar);
	hboxLayout->setStretchFactor(GroupBar, 1);
	hboxLayout->addWidget(ContactsWidget);
	hboxLayout->setStretchFactor(ContactsWidget, 100);
	hboxLayout->setAlignment(GroupBar, Qt::AlignTop);

	InfoPanel = new BuddyInfoPanel(split);
	connect(ContactsWidget->view(), SIGNAL(currentBuddyChanged(Buddy)), InfoPanel, SLOT(displayBuddy(Buddy)));

	if (!config_file.readBoolEntry("Look", "ShowInfoPanel"))
		InfoPanel->QWidget::hide();

	ChangeStatusButtons = new StatusButtons(this);
	MainLayout->addWidget(ChangeStatusButtons);

	if (!config_file.readBoolEntry("Look", "ShowStatusButton"))
		ChangeStatusButtons->hide();

	QList<int> splitSizes;

	splitSizes.append(config_file.readNumEntry("General", "UserBoxHeight"));
	splitSizes.append(config_file.readNumEntry("General", "DescriptionHeight"));

	split->setSizes(splitSizes);

	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	setCentralWidget(MainWidget);

#ifdef Q_OS_MAC
	qt_mac_set_dock_menu(dockMenu);
#endif
}

void KaduWindow::createMenu()
{
	createKaduMenu();
	createContactsMenu();
	createHelpMenu();
}

void KaduWindow::createKaduMenu()
{
	KaduMenu = new QMenu();
	KaduMenu->setTitle("&Kadu");

	RecentChatsMenu = new QMenu();
	RecentChatsMenu->setIcon(IconsManager::instance()->iconByName("OpenChat"));
	RecentChatsMenu->setTitle(tr("Recent chats..."));
	connect(KaduMenu, SIGNAL(aboutToShow()), this, SLOT(createRecentChatsMenu()));
	connect(RecentChatsMenu, SIGNAL(triggered(QAction *)), this, SLOT(openRecentChats(QAction *)));

	insertMenuActionDescription(Actions->Configuration, MenuKadu);
	insertMenuActionDescription(Actions->ShowYourAccounts, MenuKadu);
	insertMenuActionDescription(Actions->ManageModules, MenuKadu);

	KaduMenu->addSeparator();
	RecentChatsMenuAction = KaduMenu->addMenu(RecentChatsMenu);
	KaduMenu->addSeparator();

	insertMenuActionDescription(Actions->ExitKadu, MenuKadu);

	menuBar()->addMenu(KaduMenu);
}

void KaduWindow::createContactsMenu()
{
	ContactsMenu = new QMenu();
	ContactsMenu->setTitle(tr("&Buddies"));

	insertMenuActionDescription(Actions->AddUser, MenuContacts);
	insertMenuActionDescription(Actions->AddGroup, MenuContacts);
	insertMenuActionDescription(Actions->OpenSearch, MenuContacts);

	ContactsMenu->addSeparator();
//	insertMenuActionDescription(ChatWidgetManager::instance()->actions()->openChatWith(), MenuContacts);
	ContactsMenu->addSeparator();

	insertMenuActionDescription(Actions->ManageIgnored, MenuContacts);
//	insertMenuActionDescription(Actions->ImportExportContacts, MenuContacts);

	menuBar()->addMenu(ContactsMenu);
}

void KaduWindow::createHelpMenu()
{
	HelpMenu = new QMenu();
	HelpMenu->setTitle(tr("&Help"));

	insertMenuActionDescription(Actions->Help, MenuHelp);
	HelpMenu->addSeparator();
	insertMenuActionDescription(Actions->Bugs, MenuHelp);
	insertMenuActionDescription(Actions->Support, MenuHelp);
	insertMenuActionDescription(Actions->GetInvolved, MenuHelp);
	HelpMenu->addSeparator();
	insertMenuActionDescription(Actions->About, MenuHelp);

	menuBar()->addMenu(HelpMenu);
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
	ChatWidgetManager::instance()->openPendingMsgs(qvariant_cast<Chat>(action->data()), true);
	kdebugf2();
}

void KaduWindow::storeConfiguration()
{
	writeToolBarsToConfig("");
	saveWindowGeometry(this, "General", "Geometry");

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

void KaduWindow::updateInformationPanel()
{
	InfoPanel->displayBuddy(ContactsWidget->view()->currentContact().ownerBuddy());
}

void KaduWindow::closeEvent(QCloseEvent *e)
{
	e->ignore();

	if (Docked)
		hide();
	else
		qApp->quit();
}

void KaduWindow::customEvent(QEvent *e)
{
// TODO: 0.6.6
// 	Account defaultAccount = AccountManager::instance()->defaultAccount();
// 
// 	if (int(e->type()) == 4321)
// 		show();
// 	else if (int(e->type()) == 5432)
// 	{
// 		OpenGGChatEvent *ev = static_cast<OpenGGChatEvent *>(e);
// 		if (ev->number() > 0)
// 		{
// 			Contact contact = userlist->byID("Gadu", QString::number(ev->number())).toContact(defaultAccount);
// 			ContactList contacts;
// 			contacts << contact;
// 			chat_manager->openChatWidget(defaultAccount, contacts, true);
// 		}
// 	}
// 	else
// 		KaduMainWIndow::customEvent(e);
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
	else if (e->key() == Qt::Key_C && e->modifiers() & Qt::ControlModifier)
		InfoPanel->pageAction(QWebPage::Copy)->trigger();

	emit keyPressed(e);

	MainWindow::keyPressEvent(e);
}

bool KaduWindow::supportsActionType(ActionDescription::ActionType type)
{
	return type & (ActionDescription::TypeGlobal | ActionDescription::TypeUserList | ActionDescription::TypeUser);
}

BuddiesListView * KaduWindow::contactsListView()
{
	return ContactsWidget->view();
}

StatusContainer * KaduWindow::statusContainer()
{
	return 0;
}

ContactSet KaduWindow::contacts()
{
	return ContactsWidget->view()->selectedContacts();
}

BuddySet KaduWindow::buddies()
{
	return ContactsWidget->view()->selectedBuddies();
}

Chat  KaduWindow::chat()
{
	return ContactsWidget->view()->currentChat();
}

void KaduWindow::configurationUpdated()
{
	QFont userboxFont = QFont(config_file.readFontEntry("Look", "UserboxFont"));
	GroupBar->setFont(QFont(userboxFont.family(), userboxFont.pointSize(), 75));

	InfoPanel->setVisible(config_file.readBoolEntry("Look", "ShowInfoPanel"));
	setDocked(Docked);

	if (config_file.readBoolEntry("Look", "UseUserboxBackground", true))
	{
		QString type = config_file.readEntry("Look", "UserboxBackgroundDisplayStyle");
		ContactsWidget->view()->setBackground(config_file.readColorEntry("Look","UserboxBgColor").name(),
			config_file.readEntry("Look", "UserboxBackground"),
			type == "Centered" ? BuddiesListView::BackgroundCentered
			: type == "Tiled" ? BuddiesListView::BackgroundTiled
			: type == "Stretched" ? BuddiesListView::BackgroundStretched
			: type == "TiledAndCentered" ? BuddiesListView::BackgroundTiledAndCentered
			: BuddiesListView::BackgroundNone);
	}
	else
		ContactsWidget->view()->setBackground(config_file.readColorEntry("Look","UserboxBgColor").name());

	ChangeStatusButtons->setVisible(config_file.readBoolEntry("Look", "ShowStatusButton"));
}

void KaduWindow::insertMenuActionDescription(ActionDescription *actionDescription, MenuType type, int pos)
{
	kdebugf();
	if (!actionDescription)
		return;
	Action *action = actionDescription->createAction(this);

	QMenu *menu;

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

	addToolButton(toolbarConfig, "addUserAction", Qt::ToolButtonTextUnderIcon);
	addToolButton(toolbarConfig, "addGroupAction", Qt::ToolButtonTextUnderIcon);
	addToolButton(toolbarConfig, "muteSoundsAction", Qt::ToolButtonTextUnderIcon);
}

void KaduWindow::addAction(const QString &actionName, Qt::ToolButtonStyle style)
{
	addToolButton(findExistingToolbar(""), actionName, style);
	Core::instance()->kaduWindow()->refreshToolBars("");
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
