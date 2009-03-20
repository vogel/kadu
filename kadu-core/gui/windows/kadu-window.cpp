/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QHBoxLayout>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSplitter>

#include "chat/chat_manager.h"
#include "contacts/contact-manager.h"
#include "contacts/model/contacts-model.h"
#include "contacts/model/filter/group-contact-filter.h"
#include "gui/widgets/contact-info-panel.h"
#include "gui/widgets/contacts-list-widget.h"
#include "gui/widgets/group-tab-bar.h"
#include "gui/widgets/kadu_text_browser.h"
#include "gui/windows/kadu-window-actions.h"

#include "misc/misc.h"
#include "config_file.h"
#include "debug.h"
#include "hot_key.h"
#include "icons_manager.h"

#include "kadu-window.h"

KaduWindow::KaduWindow(QWidget *parent) :
		KaduMainWindow(parent), Docked(false)
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
	createStatusPopupMenu();

	MainWidget = new QWidget;
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

	ContactsWidget = new ContactsListWidget(this);
	ContactsWidget->setModel(new ContactsModel(ContactManager::instance(), this));
	ContactsWidget->addFilter(GroupBar->filter());

	connect(ContactsWidget, SIGNAL(contactActivated(Contact)), this, SLOT(sendMessage(Contact)));

	hboxLayout->addWidget(GroupBar);
	hboxLayout->setStretchFactor(GroupBar, 1);
	hboxLayout->addWidget(ContactsWidget);
	hboxLayout->setStretchFactor(ContactsWidget, 100);
	hboxLayout->setAlignment(GroupBar, Qt::AlignTop);

	InfoPanel = new ContactInfoPanel(split);
	connect(ContactsWidget, SIGNAL(currentContactChanged(Contact)), InfoPanel, SLOT(displayContact(Contact)));

	if (!config_file.readBoolEntry("Look", "ShowInfoPanel"))
		InfoPanel->QWidget::hide();

	StatusButton = new QPushButton(icons_manager->loadIcon("Offline"), tr("Offline"), this);
	MainLayout->addWidget(StatusButton);
	StatusButton->setMenu(StatusMenu);

	if (!config_file.readBoolEntry("Look", "ShowStatusButton"))
		StatusButton->hide();

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
	KaduMenu->setTitle("Kadu");

	RecentChatsMenu = new QMenu();
	RecentChatsMenu->setIcon(icons_manager->loadIcon("OpenChat"));
	RecentChatsMenu->setTitle(tr("Recent chats..."));
	connect(RecentChatsMenu, SIGNAL(triggered(QAction *)), this, SLOT(openRecentChats(QAction *)));

	insertMenuActionDescription(Actions->Configuration, MenuKadu);
	insertMenuActionDescription(Actions->YourAccounts, MenuKadu);

	KaduMenu->addSeparator();
	KaduMenu->addMenu(RecentChatsMenu);
	KaduMenu->addSeparator();

	insertMenuActionDescription(Actions->HideKadu, MenuKadu);
	insertMenuActionDescription(Actions->ExitKadu, MenuKadu);

	menuBar()->addMenu(KaduMenu);
}

void KaduWindow::createContactsMenu()
{
	ContactsMenu = new QMenu();
	ContactsMenu->setTitle(tr("Contacts"));

	insertMenuActionDescription(Actions->AddUser, MenuContacts);
	insertMenuActionDescription(Actions->AddGroup, MenuContacts);
	insertMenuActionDescription(Actions->OpenSearch, MenuContacts);

	ContactsMenu->addSeparator();
	insertMenuActionDescription(chat_manager->openChatWithActionDescription, MenuContacts);
	ContactsMenu->addSeparator();

	insertMenuActionDescription(Actions->ManageIgnored, MenuContacts);
	insertMenuActionDescription(Actions->ImportExportContacts, MenuContacts);

	menuBar()->addMenu(ContactsMenu);
}

void KaduWindow::createHelpMenu()
{
	HelpMenu = new QMenu();
	HelpMenu->setTitle(tr("Help"));

	insertMenuActionDescription(Actions->Help, MenuHelp);
	HelpMenu->addSeparator();
	insertMenuActionDescription(Actions->Bugs, MenuHelp);
	insertMenuActionDescription(Actions->Support, MenuHelp);
	insertMenuActionDescription(Actions->GetInvolved, MenuHelp);
	HelpMenu->addSeparator();
	insertMenuActionDescription(Actions->About, MenuHelp);

	menuBar()->addMenu(HelpMenu);
}

void KaduWindow::createStatusPopupMenu()
{
	kdebugf();

	QPixmap pix;
	QIcon icon;

	StatusMenu = new QMenu(this);

	ChangeStatusActionGroup = new QActionGroup(this);
	ChangeStatusActionGroup->setExclusive(false); // HACK

	// TODO: 0.6.6

	ChangeStatusToOnline = new QAction(/*icons_manager->loadIcon(s.pixmapName(Online, false, false)), */tr("Online"), this);
	ChangeStatusToOnline->setCheckable(true);
	ChangeStatusToOnline->setData(0);
	connect(ChangeStatusToOnline, SIGNAL(triggered()), this, SLOT(changeStatusSlot()));

	ChangeStatusToOnlineDesc = new QAction(/*icons_manager->loadIcon(s.pixmapName(Online, true, false)), */tr("Online (d.)"), this);
	ChangeStatusToOnlineDesc->setCheckable(true);
	ChangeStatusToOnlineDesc->setData(1);
	connect(ChangeStatusToOnlineDesc, SIGNAL(triggered()), this, SLOT(changeStatusSlot()));

	ChangeStatusToBusy = new QAction(/*icons_manager->loadIcon(s.pixmapName(Busy, false, false)), */tr("Busy"), this);
	ChangeStatusToBusy->setCheckable(true);
	ChangeStatusToBusy->setData(2);
	connect(ChangeStatusToBusy, SIGNAL(triggered()), this, SLOT(changeStatusSlot()));

	ChangeStatusToBusyDesc = new QAction(/*icons_manager->loadIcon(s.pixmapName(Busy, true, false)), */tr("Busy (d.)"), this);
	ChangeStatusToBusyDesc->setCheckable(true);
	ChangeStatusToBusyDesc->setData(3);
	connect(ChangeStatusToBusyDesc, SIGNAL(triggered()), this, SLOT(changeStatusSlot()));

	ChangeStatusToInvisible = new QAction(/*icons_manager->loadIcon(s.pixmapName(Invisible, false, false)), */tr("Invisible"), this);
	ChangeStatusToInvisible->setCheckable(true);
	ChangeStatusToInvisible->setData(4);
	connect(ChangeStatusToInvisible, SIGNAL(triggered()), this, SLOT(changeStatusSlot()));

	ChangeStatusToInvisibleDesc = new QAction(/*icons_manager->loadIcon(s.pixmapName(Invisible, true, false)), */tr("Invisible (d.)"), this);
	ChangeStatusToInvisibleDesc->setCheckable(true);
	ChangeStatusToInvisibleDesc->setData(5);
	connect(ChangeStatusToInvisibleDesc, SIGNAL(triggered()), this, SLOT(changeStatusSlot()));

	ChangeStatusToOffline = new QAction(/*icons_manager->loadIcon(s.pixmapName(Offline, false, false)), */tr("Offline"), this);
	ChangeStatusToOffline->setCheckable(true);
	ChangeStatusToOffline->setData(6);
	connect(ChangeStatusToOffline, SIGNAL(triggered()), this, SLOT(changeStatusSlot()));

	ChangeStatusToOfflineDesc = new QAction(/*icons_manager->loadIcon(s.pixmapName(Offline, true, false)), */tr("Offline (d.)"), this);
	ChangeStatusToOfflineDesc->setCheckable(true);
	ChangeStatusToOfflineDesc->setData(7);
	connect(ChangeStatusToOfflineDesc, SIGNAL(triggered()), this, SLOT(changeStatusSlot()));

	ChangePrivateStatus = new QAction(tr("Private"), this);
	ChangePrivateStatus->setCheckable(true);
	connect(ChangePrivateStatus, SIGNAL(toggled(bool)), this, SLOT(changePrivateStatusSlot(bool)));

	bool privateStatus = config_file.readBoolEntry("General", "PrivateStatus");
	ChangePrivateStatus->setChecked(privateStatus);

	ChangeStatusActionGroup->addAction(ChangeStatusToOnline);
	ChangeStatusActionGroup->addAction(ChangeStatusToOnlineDesc);
	ChangeStatusActionGroup->addAction(ChangeStatusToBusy);
	ChangeStatusActionGroup->addAction(ChangeStatusToBusyDesc);
	ChangeStatusActionGroup->addAction(ChangeStatusToInvisible);
	ChangeStatusActionGroup->addAction(ChangeStatusToInvisibleDesc);
	ChangeStatusActionGroup->addAction(ChangeStatusToOffline);
	ChangeStatusActionGroup->addAction(ChangeStatusToOfflineDesc);

	StatusMenu->addAction(ChangeStatusToOnline);
	StatusMenu->addAction(ChangeStatusToOnlineDesc);
	StatusMenu->addAction(ChangeStatusToBusy);
	StatusMenu->addAction(ChangeStatusToBusyDesc);
	StatusMenu->addAction(ChangeStatusToInvisible);
	StatusMenu->addAction(ChangeStatusToInvisibleDesc);
	StatusMenu->addAction(ChangeStatusToOffline);
	StatusMenu->addAction(ChangeStatusToOfflineDesc);
	StatusMenu->addSeparator();
	StatusMenu->addAction(ChangePrivateStatus);

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
	InfoPanel->displayContact(ContactsWidget->currentContact());
}

void KaduWindow::closeEvent(QCloseEvent *e)
{
	if (Docked)
	{
		e->ignore();
		hide();
	}
	else
		e->accept();
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

	KaduMainWindow::keyPressEvent(e);
}

bool KaduWindow::supportsActionType(ActionDescription::ActionType type)
{
	return type & (ActionDescription::TypeGlobal | ActionDescription::TypeUserList | ActionDescription::TypeUser);
}

ContactsListWidget * KaduWindow::contactsListWidget()
{
	return ContactsWidget;
}

ContactList KaduWindow::contacts()
{
	return ContactsWidget->selectedContacts();
}

void KaduWindow::configurationUpdated()
{
	InfoPanel->setVisible(config_file.readBoolEntry("Look", "ShowInfoPanel"));
	setDocked(Docked);
}

void KaduWindow::insertMenuActionDescription(ActionDescription *actionDescription, MenuType type, int pos)
{
	kdebugf();
	if (!actionDescription)
		return;
	KaduAction *action = actionDescription->createAction(this);

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
	KaduAction *action = MenuActions[actionDescription].first;

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

	addToolButton(toolbarConfig, "inactiveUsersAction");
	addToolButton(toolbarConfig, "descriptionUsersAction");
	addToolButton(toolbarConfig, "configurationAction");
	addToolButton(toolbarConfig, "editUserAction");
	addToolButton(toolbarConfig, "openSearchAction");
	addToolButton(toolbarConfig, "addUserAction");
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
