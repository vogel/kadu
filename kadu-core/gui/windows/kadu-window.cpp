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
#include "gui/widgets/contacts-list-widget.h"
#include "gui/widgets/group-tab-bar.h"
#include "gui/widgets/kadu_text_browser.h"

#include "config_file.h"
#include "debug.h"
#include "icons_manager.h"
#include "misc.h"

#include "kadu-window.h"

KaduWindow::KaduWindow(QWidget *parent) :
		KaduMainWindow(parent)
{
	createGui();
}

KaduWindow::~KaduWindow()
{
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
	QHBoxLayout *hbox_layout = new QHBoxLayout(hbox);
	hbox_layout->setMargin(0);
	hbox_layout->setSpacing(0);

	// groupbar
	GroupBar = new GroupTabBar(this);
	hbox_layout->setStretchFactor(GroupBar, 1);

	ContactsWidget = new ContactsListWidget(this);
	ContactsWidget->setModel(new ContactsModel(ContactManager::instance(), this));
	ContactsWidget->addFilter(GroupBar->filter());

	hbox_layout->setStretchFactor(ContactsWidget, 100);
	hbox_layout->addWidget(GroupBar);
	hbox_layout->addWidget(ContactsWidget);
	hbox_layout->setAlignment(GroupBar, Qt::AlignTop);

	connect(ContactsWidget, SIGNAL(contactActivated(Contact)), this, SLOT(sendMessage(Contact)));
	connect(ContactsWidget, SIGNAL(currentContactChanged(Contact)), this, SLOT(currentChanged(Contact)));

	InfoPanel = new KaduTextBrowser(split);
// TODO: 0.6.5
// 	InfoPanel->setFrameStyle(QFrame::NoFrame);
// 	InfoPanel->setMinimumHeight(int(1.5 * QFontMetrics(InfoPanel->QTextEdit::font()).height()));
//	InfoPanel->resize(InfoPanel->size().width(), int(1.5 * QFontMetrics(InfoPanel->font()).height()));
// 	InfoPanel->setTextFormat(Qt::RichText);
// 	InfoPanel->setAlignment(Qt::AlignVCenter/** | Qt::WordBreak | Qt::DontClip*/);

	if (!config_file.readBoolEntry("Look", "ShowInfoPanel"))
		InfoPanel->QWidget::hide();

	StatusButton = new QPushButton(icons_manager->loadIcon("Offline"), tr("Offline"), this);
	MainLayout->addWidget(StatusButton);
	StatusButton->setMenu(StatusMenu);

	if (!config_file.readBoolEntry("Look", "ShowStatusButton"))
		StatusButton->hide();

	QList<int> splitsizes;

	splitsizes.append(config_file.readNumEntry("General", "UserBoxHeight"));
	splitsizes.append(config_file.readNumEntry("General", "DescriptionHeight"));

	split->setSizes(splitsizes);

	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	setCentralWidget(MainWidget);

#ifdef Q_OS_MAC
	qt_mac_set_dock_menu(dockMenu);
#endif

	loadWindowGeometry(this, "General", "Geometry", 0, 50, 205, 465);
}

void KaduWindow::createMenu()
{
	kdebugf();
// Kadu Menu
	KaduMenu = new QMenu;
	KaduMenu->setTitle("Kadu");

	RecentChatsMenu = new QMenu;
	RecentChatsMenu->setIcon(icons_manager->loadIcon("OpenChat"));
	RecentChatsMenu->setTitle(tr("Recent chats..."));
	connect(RecentChatsMenu, SIGNAL(triggered(QAction *)), this, SLOT(openRecentChats(QAction *)));

// 	insertMenuActionDescription(configurationActionDescription, MenuKadu);

	ActionDescription *yourAccountsActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "yourAccountsAction",
		this, SLOT(yourAccounts(QAction *, bool)),
		"PersonalInfo", tr("Your accounts")
	);//TODO 0.6.6: implement
	insertMenuActionDescription(yourAccountsActionDescription, MenuKadu);

	KaduMenu->addSeparator();

	KaduMenu->addMenu(RecentChatsMenu);
	KaduMenu->addSeparator();
	ActionDescription *hideKaduActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "hideKaduAction",
		this, SLOT(hideKadu(QAction *, bool)),
		"HideKadu", tr("&Hide")
	);
	insertMenuActionDescription(hideKaduActionDescription, MenuKadu);

	ActionDescription *exitKaduActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "exitKaduAction",
		this, SLOT(quit()),
		"Exit", tr("&Exit")
	);
	insertMenuActionDescription(exitKaduActionDescription, MenuKadu);

// ContactsMenu
	ContactsMenu = new QMenu;
	ContactsMenu->setTitle(tr("Contacts"));

// 	insertMenuActionDescription(addUserActionDescription, MenuContacts);
	ActionDescription *addGroupActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "addGroupAction",
		this, SLOT(addGroupActionActivated(QAction *, bool)),
		"", tr("Add Group")
	);//TODO 0.6.6: implement and update icons
	insertMenuActionDescription(addGroupActionDescription, MenuContacts);
// 	insertMenuActionDescription(openSearchActionDescription, MenuContacts);
	ContactsMenu->addSeparator();
	insertMenuActionDescription(chat_manager->openChatWithActionDescription, MenuContacts);
	ContactsMenu->addSeparator();
	ActionDescription *manageIgnoredActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "manageIgnoredAction",
		this, SLOT(manageIgnored(QAction *, bool)),
		"Ignore", tr("&Ignored users")
	);
	insertMenuActionDescription(manageIgnoredActionDescription, MenuContacts);

	ActionDescription *importExportUserlisActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "importExportUserlisAction",
		this, SLOT(importExportUserlist(QAction *, bool)),
		"ImportExport", tr("I&mport / Export userlist")
	); //TODO 0.6.6: remove
	insertMenuActionDescription(importExportUserlisActionDescription, MenuContacts);

// Help Menu
	HelpMenu = new QMenu;
	HelpMenu->setTitle(tr("Help"));

	ActionDescription *helpActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "helpAction",
		this, SLOT(help(QAction *, bool)),
		"HelpMenuItem", tr("Getting H&elp")
	);
	insertMenuActionDescription(helpActionDescription, MenuHelp);
	HelpMenu->addSeparator();
	ActionDescription *bugsActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "bugsAction",
		this, SLOT(bugs(QAction *, bool)),
		"", tr("Submitt Bug Report")
	);
	insertMenuActionDescription(bugsActionDescription, MenuHelp);
	ActionDescription *supportActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "supportAction",
		this, SLOT(support(QAction *, bool)),
		"", tr("Support us")
	);
	insertMenuActionDescription(supportActionDescription, MenuHelp);
	ActionDescription *getInvolvedActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "getInvolvedAction",
		this, SLOT(getInvolved(QAction *, bool)),
		"", tr("Get Involved")
	);
	insertMenuActionDescription(getInvolvedActionDescription, MenuHelp);
	HelpMenu->addSeparator();
	ActionDescription *aboutActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "aboutAction",
		this, SLOT(about(QAction *, bool)),
		"AboutMenuItem", tr("A&bout Kadu")
	);
	insertMenuActionDescription(aboutActionDescription, MenuHelp);
/*
TODO 0.6.6:
add new action: "History" in MenuKadu
*/
	menuBar()->addMenu(KaduMenu);
	menuBar()->addMenu(ContactsMenu);
	menuBar()->addMenu(HelpMenu);

	kdebugf2();
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
