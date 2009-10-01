/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QSettings>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSplitter>
#include <QtGui/QVBoxLayout>
#include <QtWebKit/QWebFrame>

#ifndef _MSC_VER
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include "about.h"
#include "chat_edit_box.h"
#include "chat_manager.h"
#include "config_file.h"
#include "debug.h"
#include "emoticons.h"
#include "expimp.h"
#include "gadu.h"
#include "gadu_images_manager.h"
#include "groups_manager.h"
#include "hot_key.h"
#include "html_document.h"
#include "icons_manager.h"
#include "ignore.h"
#include "kadu_parser.h"
#include "main_configuration_window.h"
#include "message_box.h"
#include "modules.h"
#include "misc.h"
#include "pending_msgs.h"
#include "personal_info.h"
#include "protocols_manager.h"
#include "search.h"
#include "status_changer.h"
#include "syntax_editor.h"
#include "tabbar.h"
#include "toolbar.h"
#include "updates.h"
#include "userbox.h"
#include "userinfo.h"

#include "kadu.h"

#ifdef Q_OS_MAC
extern void qt_mac_set_dock_menu(QMenu *); //there's no header for it
#endif

//look for comment in config_file.h
ConfigFile *config_file_ptr;

Kadu *kadu;

static QTimer *blinktimer;
QMenu *dockMenu;

const char *Kadu::SyntaxText = QT_TRANSLATE_NOOP
(
	"@default", "Syntax: %s - status, %d - description, %i - ip, %n - nick, %a - altnick, %f - first name\n"
	"%r - surname, %m - mobile, %u - uin, %g - group, %o - return _space_ if user doesn't have us in userlist\n"
	"%h - gg version, %v - revDNS, %p - port, %e - email, %x - max image size\n"
);

const char *Kadu::SyntaxTextExtended = QT_TRANSLATE_NOOP
(
	"@default", "Syntax: %s - status, %d - description, %i - ip, %n - nick, %a - altnick, %f - first name\n"
	"%r - surname, %m - mobile, %u - uin, %g - group, %o - return _space_ if user doesn't have us in userlist\n"
	"%h - gg version, %v - revDNS, %p - port, %e - email, %x - max image size,\n"
	"#{message} - message content,\n"
	"#{backgroundColor} - background color of message,\n"
	"#{fontColor} - font color of message,\n"
	"#{nickColor} - font color of nick,\n"
	"#{sentDate} - when message was sent,\n"
	"#{receivedDate} - when message was received,\n"
	"#{separator} - separator between messages,\n"
	"<kadu:header>text</kadu:header> - text will not be displayed in 'Remove repeated headers' mode\n"
);

const char *Kadu::SyntaxTextNotify = QT_TRANSLATE_NOOP
(
	"@default", "Syntax: %s - status, %d - description, %i - ip, %n - nick, %a - altnick, %f - first name\n"
	"%r - surname, %m - mobile, %u - uin, %g - group, %o - return _space_ if user doesn't have us in userlist\n"
	"%h - gg version, %v - revDNS, %p - port, %e - email, %x - max image size,\n"
	"#{protocol} - protocol that triggered event,\n"
	"#{event} - name of event,\n"
);

bool Kadu::Closing = false;

void Kadu::keyPressEvent(QKeyEvent *e)
{
//	kdebugf();
	if (e->key() == Qt::Key_Escape)
	{
		if (Docked)
		{
			kdebugm(KDEBUG_INFO, "Kadu::keyPressEvent(Key_Escape): Kadu hide\n");
			if (dontHideOnClose)
				showMinimized();
			else
				hide();
		}
	}
	else if (HotKey::shortCut(e,"ShortCuts", "kadu_deleteuser"))
		deleteUsersActionDescription->createAction(this)->trigger();
	else if (e->key() == Qt::Key_C && e->modifiers() & Qt::ControlModifier)
		InfoPanel->pageAction(QWebPage::Copy)->trigger();

	emit keyPressed(e);

	QWidget::keyPressEvent(e);
//	kdebugf2();
}

void Kadu::closeEvent(QCloseEvent *event)
{	 
	kdebugf();	 

	if (!Closing)	 
	{	 
		event->ignore();	 
		close();	 
	}	 
	else	 
		event->accept();	 

	kdebugf2();	 
}


void disableIllegalIdUles(KaduAction *action)
{
	kdebugf();
	foreach(const UserListElement &user, action->userListElements())
		if ((!user.usesProtocol("Gadu")) || (user.usesProtocol("Gadu") && user.ID("Gadu") == kadu->myself().ID("Gadu")))
		{
			action->setEnabled(false);
			return;
		}

	action->setEnabled(true);
	kdebugf2();
}

void checkNotify(KaduAction *action)
{
	kdebugf();

	if (config_file.readBoolEntry("Notify", "NotifyAboutAll"))
	{
		action->setEnabled(false);
		return;
	}

	foreach(const UserListElement &user, action->userListElements())
		if (!user.usesProtocol("Gadu"))
		{
			action->setEnabled(false);
			return;
		}
	action->setEnabled(true);

	bool on = true;
	foreach(const UserListElement &user, action->userListElements())
		if (!user.notify())
		{
			on = false;
			break;
		}
	action->setChecked(on);

	kdebugf2();
}

void checkOfflineTo(KaduAction *action)
{
	kdebugf();
	bool on = true;
	foreach(const UserListElement &user, action->userListElements())
		if (!user.usesProtocol("Gadu") || !user.protocolData("Gadu", "OfflineTo").toBool())
		{
			on = false;
			break;
		}
	action->setChecked(on);
	kdebugf2();
}

void checkHideDescription(KaduAction *action)
{
	foreach(const UserListElement &user, action->userListElements())
		if (!user.usesProtocol("Gadu"))
		{
			action->setEnabled(false);
			return;
		}
	action->setEnabled(true);

	bool on = false;
	foreach(const UserListElement &user, action->userListElements())
		if (user.data("HideDescription").toString() == "true")
		{
			on = true;
			break;
		}
	action->setChecked(on);
}

void disableNotOneUles(KaduAction *action)
{
	kdebugf();
	if (action->userListElements().count() != 1)
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(true);
	kdebugf2();
}

void disableNoGaduUle(KaduAction *action)
{
	kdebugf();

	if (action->userListElements().count() != 1)
	{
		action->setEnabled(false);
		return;
	}

	if (!action->userListElements()[0].usesProtocol("Gadu"))
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(true);
	kdebugf2();
}

void disableNoGaduDescription(KaduAction *action)
{
	kdebugf();

	if (action->userListElements().count() != 1)
	{
		action->setEnabled(false);
		return;
	}

	if (!action->userListElements()[0].usesProtocol("Gadu"))
	{
		action->setEnabled(false);
		return;
	}

	if (action->userListElements()[0].status("Gadu").description().isEmpty())
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(true);
	kdebugf2();
}

void disableNoGaduDescriptionUrl(KaduAction *action)
{
	kdebugf();

	if (action->userListElements().count() != 1)
	{
		action->setEnabled(false);
		return;
	}

	if (!action->userListElements()[0].usesProtocol("Gadu"))
	{
		action->setEnabled(false);
		return;
	}

	if (action->userListElements()[0].status("Gadu").description().isEmpty())
	{
		action->setEnabled(false);
		return;
	}

	if (action->userListElements()[0].status("Gadu").description().indexOf(HtmlDocument::urlRegExp()) < 0)
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(true);
	kdebugf2();
}

void disableNoEMail(KaduAction *action)
{
	kdebugf();

	if (action->userListElements().count() != 1)
	{
		action->setEnabled(false);
		return;
	}

	const UserListElements &ules = action->userListElements();
	if (ules[0].email().isEmpty() || ules[0].email().indexOf(HtmlDocument::mailRegExp()) < 0)
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(true);
	kdebugf2();
}

/* a monstrous constructor so Kadu would take longer to start up */
Kadu::Kadu(QWidget *parent)
	: KaduMainWindow(parent),
	InfoPanel(0), MenuBar(0), MainMenu(0), RecentChatsMenu(0), GroupBar(0),
	Userbox(0), statusMenu(0), statusButton(), lastPositionBeforeStatusMenuHide(),
	StartTime(QDateTime::currentDateTime()), updateInformationPanelTimer(), NextStatus(),
	selectedUsers(new UserGroup()), ShowMainWindowOnStart(true),
	DoBlink(false), BlinkOn(false),Docked(false), dontHideOnClose(false), personalInfoMenuId(-1)
{
	kdebugf();
	kadu = this;
	blinktimer = 0;

	Myself.addProtocol("Gadu", QString::number(config_file.readUnsignedNumEntry("General", "UIN", 0)));
	Myself.setAltNick(config_file.readEntry("General", "Nick"));

	createDefaultConfiguration();

#ifdef Q_OS_MAC
	setUnifiedTitleAndToolBarOnMac(true);
#endif

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
	GroupBar = new KaduTabBar(this);
	hbox_layout->setStretchFactor(GroupBar, 1);

	StatusChangerManager::initModule();
	connect(status_changer_manager, SIGNAL(statusChanged(UserStatus)), this, SLOT(changeStatus(UserStatus)));

	userStatusChanger = new UserStatusChanger();
	status_changer_manager->registerStatusChanger(userStatusChanger);

#if 0
	splitStatusChanger = new SplitStatusChanger(GG_STATUS_DESCR_MAXSIZE);
	status_changer_manager->registerStatusChanger(splitStatusChanger);
#endif

	// gadu, chat, search
	GaduProtocol::initModule();
	ChatManager::initModule();
	SearchDialog::initModule();

	// userbox
	UserBox::initModule();
	Userbox = new UserBox(this, true, userlist, this, "userbox");

	hbox_layout->setStretchFactor(Userbox, 100);
	hbox_layout->addWidget(GroupBar);
	hbox_layout->addWidget(Userbox);
	hbox_layout->setAlignment(GroupBar, Qt::AlignTop);

	connect(Userbox, SIGNAL(doubleClicked(UserListElement)), this, SLOT(sendMessage(UserListElement)));
	connect(Userbox, SIGNAL(returnPressed(UserListElement)), this, SLOT(sendMessage(UserListElement)));
	connect(Userbox, SIGNAL(mouseButtonClicked(int, Q3ListBoxItem *, const QPoint &)),
		this, SLOT(mouseButtonClicked(int, Q3ListBoxItem *)));
	connect(Userbox, SIGNAL(currentChanged(UserListElement)), this, SLOT(currentChanged(UserListElement)));


	ActionDescription *writeEmailActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "writeEmailAction",
		this, SLOT(writeEMailActionActivated(QAction *, bool)),
		"WriteEmail", tr("Write email message"), false, "",
		disableNoEMail
	);
	UserBox::addActionDescription(writeEmailActionDescription);

	ActionDescription *copyDescriptionActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "copyDescriptionAction",
		this, SLOT(copyDescriptionActionActivated(QAction *, bool)),
		"CopyDescription", tr("Copy description"), false, "",
		disableNoGaduDescription
	);
	UserBox::addActionDescription(copyDescriptionActionDescription);

	ActionDescription *openDescriptionLinkActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "openDescriptionLinkAction",
		this, SLOT(openDescriptionLinkActionActivated(QAction *, bool)),
		"OpenDescriptionLink", tr("Open description link in browser"), false, "",
		disableNoGaduDescriptionUrl
	);
	UserBox::addActionDescription(openDescriptionLinkActionDescription);

	ActionDescription *copyPersonalInfoActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "copyPersonalInfoAction",
		this, SLOT(copyPersonalInfoActionActivated(QAction *, bool)),
		"CopyPersonalInfo", tr("Copy personal info")
	);
	UserBox::addActionDescription(copyPersonalInfoActionDescription);

	ActionDescription *lookupUserInfoActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "lookupUserInfoAction",
		this, SLOT(lookupInDirectoryActionActivated(QAction *, bool)),
		"LookupUserInfo", tr("Search in directory"), false, "",
		disableNoGaduUle
	);
	UserBox::addActionDescription(lookupUserInfoActionDescription);

	UserBox::addSeparator();

	UserBox::addManagementActionDescription(chat_manager->ignoreUserActionDescription);
	UserBox::addManagementActionDescription(chat_manager->blockUserActionDescription);

	notifyAboutUserActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "notifyAboutUserAction",
		this, SLOT(notifyAboutUserActionActivated(QAction *, bool)),
		"NotifyAboutUser", tr("Notify about user"), true, "",
		checkNotify
	);
	UserBox::addManagementActionDescription(notifyAboutUserActionDescription);

	offlineToUserActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "offlineToUserAction",
		this, SLOT(offlineToUserActionActivated(QAction *, bool)),
		"Offline", tr("Offline to user"), true, "",
		checkOfflineTo
	);
	UserBox::addManagementActionDescription(offlineToUserActionDescription);

	hideDescriptionActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "hideDescriptionAction",
		this, SLOT(hideDescriptionActionActivated(QAction *, bool)),
		"ShowDescription_off", tr("Hide description"), true, "",
		checkHideDescription
	);
	UserBox::addManagementActionDescription(hideDescriptionActionDescription);

	UserBox::addManagementSeparator();

	deleteUsersActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "deleteUsersAction",
		this, SLOT(deleteUsersActionActivated(QAction *, bool)),
		"RemoveFromUserlist", tr("Delete")
	);
	deleteUsersActionDescription->setShortcut("kadu_deleteuser");
	UserBox::addManagementActionDescription(deleteUsersActionDescription);

	groups_manager->setTabBar(GroupBar);
	setDocked(Docked, dontHideOnClose);

	loadWindowGeometry(this, "General", "Geometry", 0, 50, 205, 465);

	import_0_5_0_configuration();

	IgnoredManager::loadFromConfiguration();

	/* a newbie? */

	setWindowTitle(tr("Kadu: %1").arg(Myself.ID("Gadu")));

	pending.loadFromFile();

	inactiveUsersAction = new ActionDescription(
		ActionDescription::TypeUserList, "inactiveUsersAction",
		this, SLOT(inactiveUsersActionActivated(QAction *, bool)),
		"ShowHideInactiveUsers", tr("Hide offline users"),
		true, tr("Show offline users")
	);
	connect(inactiveUsersAction, SIGNAL(actionCreated(KaduAction *)), this, SLOT(inactiveUsersActionCreated(KaduAction *)));
	inactiveUsersAction->setShortcut("kadu_showoffline");

	descriptionUsersAction = new ActionDescription(
		ActionDescription::TypeUserList, "descriptionUsersAction",
		this, SLOT(descriptionUsersActionActivated(QAction *, bool)),
		"ShowOnlyDescriptionUsers", tr("Hide users without description"),
		true, tr("Show users without description")
	);
	connect(descriptionUsersAction, SIGNAL(actionCreated(KaduAction *)), this, SLOT(descriptionUsersActionCreated(KaduAction *)));
	descriptionUsersAction->setShortcut("kadu_showonlydesc");

	onlineAndDescriptionUsersAction = new ActionDescription(
		ActionDescription::TypeUserList, "onlineAndDescriptionUsersAction",
		this, SLOT(onlineAndDescUsersActionActivated(QAction *, bool)),
		"ShowOnlineAndDescriptionUsers", tr("Show only online or description buddies"),
		true, tr("Show all users")
	);
	connect(onlineAndDescriptionUsersAction, SIGNAL(actionCreated(KaduAction *)), this, SLOT(onlineAndDescUsersActionCreated(KaduAction *)));

	configurationActionDescription = new ActionDescription(
		ActionDescription::TypeGlobal, "configurationAction",
		this, SLOT(configurationActionActivated(QAction *, bool)),
		"Configuration", tr("Configuration")
	);
	configurationActionDescription->setShortcut("kadu_configure", Qt::ApplicationShortcut);

	editUserActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "editUserAction",
		this, SLOT(editUserActionActivated(QAction *, bool)),
		"EditUserInfo", tr("Contact data"), false, QString::null,
		disableNotOneUles
	);
	connect(editUserActionDescription, SIGNAL(actionCreated(KaduAction *)), this, SLOT(editUserActionCreated(KaduAction *)));
	editUserActionDescription->setShortcut("kadu_persinfo");
	UserBox::addActionDescription(editUserActionDescription);

	addUserActionDescription = new ActionDescription(
		ActionDescription::TypeGlobal, "addUserAction",
		this, SLOT(addUserActionActivated(QAction *, bool)),
		"AddUser", tr("Add user")
	);
	addUserActionDescription->setShortcut("kadu_adduser", Qt::ApplicationShortcut);

	openSearchActionDescription = new ActionDescription(
		ActionDescription::TypeGlobal, "openSearchAction",
		this, SLOT(searchInDirectoryActionActivated(QAction *, bool)),
		"LookupUserInfo", tr("Search user in directory")
	);
	openSearchActionDescription->setShortcut("kadu_searchuser");

	showStatusActionDescription = new ActionDescription(
		ActionDescription::TypeGlobal, "openStatusAction",
		this, SLOT(showStatusActionActivated(QAction *, bool)),
		"Offline", tr("Change status")
	);
	connect(showStatusActionDescription, SIGNAL(actionCreated(KaduAction *)), this, SLOT(showStatusActionCreated(KaduAction *)));


	/* guess what */
	createMenu();
	createStatusPopupMenu();
	loadToolBarsFromConfig("");

	connect(statusMenu, SIGNAL(aboutToHide()), this, SLOT(statusMenuAboutToHide()));
	connect(dockMenu, SIGNAL(aboutToHide()), this, SLOT(dockMenuAboutToHide()));
	connect(RecentChatsMenu, SIGNAL(aboutToShow()), this, SLOT(createRecentChatsMenu()));

#ifndef Q_OS_MAC /* Dorr: On Mac there is already Exit entry in dock menu */
	dockMenu->insertSeparator();
	dockMenu->addAction(icons_manager->loadIcon("Exit"), tr("&Exit Kadu"), this, SLOT(quit()));
#endif

	InfoPanel = new KaduTextBrowser(split);
	InfoPanel->resize(InfoPanel->width(), config_file.readNumEntry("General", "DescriptionHeight"));
	if (!config_file.readBoolEntry("Look", "ShowInfoPanel"))
		InfoPanel->QWidget::hide();
	connect(&updateInformationPanelTimer, SIGNAL(timeout()), this, SLOT(updateInformationPanel()));

	statusButton = new QPushButton(icons_manager->loadIcon("Offline"), tr("Offline"), this, "statusButton");
	MainLayout->addWidget(statusButton);
	statusButton->setPopup(statusMenu);

	if (!config_file.readBoolEntry("Look", "ShowStatusButton"))
		statusButton->hide();

	QList<int> splitsizes;

	splitsizes.append(config_file.readNumEntry("General", "UserBoxHeight"));
	splitsizes.append(config_file.readNumEntry("General", "DescriptionHeight"));

	split->setSizes(splitsizes);

	connect(gadu, SIGNAL(messageReceived(Protocol *, UserListElements, const QString &, time_t)),
		this, SLOT(messageReceived(Protocol *, UserListElements, const QString &, time_t)));
	connect(gadu, SIGNAL(connecting()), this, SLOT(connecting()));
	connect(gadu, SIGNAL(connected()), this, SLOT(connected()));
	connect(gadu, SIGNAL(disconnected()), this, SLOT(disconnected()));
	connect(gadu, SIGNAL(imageReceivedAndSaved(UinType, quint32, quint32, const QString &)),
		this, SLOT(imageReceivedAndSaved(UinType, quint32, quint32, const QString &)));
	connect(gadu, SIGNAL(needTokenValue(QPixmap, QString &)),
		this, SLOT(readTokenValue(QPixmap, QString &)));
	connect(gadu, SIGNAL(systemMessageReceived(const QString &)), this, SLOT(systemMessageReceived(const QString &)));

	connect(userlist, SIGNAL(usersDataChanged(QString)), this, SLOT(updateInformationPanelLater()));
	connect(userlist, SIGNAL(protocolUsersDataChanged(QString, QString)), this, SLOT(updateInformationPanelLater()));
	connect(userlist, SIGNAL(usersStatusChanged(QString)), this, SLOT(updateInformationPanelLater()));

	connect(userlist, SIGNAL(protocolUserDataChanged(QString, UserListElement, QString, QVariant, QVariant, bool, bool)),
		this, SLOT(editUserActionSetParams(QString, UserListElement)));

	connect(&(gadu->currentStatus()), SIGNAL(goOnline(const QString &)),
		this, SLOT(wentOnline(const QString &)));
	connect(&(gadu->currentStatus()), SIGNAL(goBusy(const QString &)),
		this, SLOT(wentBusy(const QString &)));
	connect(&(gadu->currentStatus()), SIGNAL(goInvisible(const QString &)),
		this, SLOT(wentInvisible(const QString &)));
	connect(&(gadu->currentStatus()), SIGNAL(goOffline(const QString &)),
		this, SLOT(wentOffline(const QString &)));
	connect(&(gadu->currentStatus()), SIGNAL(goTalkWithMe(const QString &)),
		this, SLOT(wentTalkWithMe(const QString &)));
	connect(&(gadu->currentStatus()), SIGNAL(goDoNotDisturb(const QString &)),
		this, SLOT(wentDoNotDisturb(const QString &)));

	MainLayout->setResizeMode(QLayout::Minimum);
	setCentralWidget(MainWidget);

#ifdef Q_OS_MAC
	qt_mac_set_dock_menu(dockMenu);
#endif

	if (config_file.readBoolEntry("Chat", "SaveOpenedWindows", true))
		chat_manager->loadOpenedWindows();

	configurationUpdated();

	kdebugf2();
}

QVBoxLayout * Kadu::mainLayout() const
{
	return MainLayout;
}

void Kadu::writeEMailActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	UserListElements users = window->userListElements();
	if (users.count() < 1)
		return;

	UserListElement user = users[0];
	if (!user.email().isEmpty())
		openMailClient(user.email());

	kdebugf2();
}

void Kadu::copyDescriptionActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	UserListElements users = window->userListElements();
	if (users.count() < 1)
		return;

	UserListElement user = users[0];
	if (!user.usesProtocol("Gadu"))
		return;

	QString status = user.status("Gadu").description();
	if (status.isEmpty())
		return;

	QApplication::clipboard()->setText(status, QClipboard::Selection);
	QApplication::clipboard()->setText(status, QClipboard::Clipboard);

	kdebugf2();
}

void Kadu::openDescriptionLinkActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	UserListElements users = window->userListElements();
	if (users.count() < 1)
		return;

	UserListElement user = users[0];
	if (!user.usesProtocol("Gadu"))
		return;

	QString status = user.status("Gadu").description();
	if (status.isEmpty())
		return;

	QRegExp url = HtmlDocument::urlRegExp();
	int idx_start = url.search(status);
	if (idx_start >= 0)
		openWebBrowser(status.mid(idx_start, url.matchedLength()));

	kdebugf2();
}

void Kadu::copyPersonalInfoActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	UserListElements users = window->userListElements();

	QStringList infoList;
	QString copyPersonalDataSyntax = config_file.readEntry("General", "CopyPersonalDataSyntax", tr("Contact: %a[ (%u)]\n[First name: %f\n][Last name: %r\n][Mobile: %m\n]"));
	foreach(const UserListElement &user, users)
		infoList.append(KaduParser::parse(copyPersonalDataSyntax, user, false));

	QString info = infoList.join("\n");
	if (info.isEmpty())
		return;

	QApplication::clipboard()->setText(info, QClipboard::Selection);
	QApplication::clipboard()->setText(info, QClipboard::Clipboard);

	kdebugf2();
}

void Kadu::lookupInDirectoryActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	UserListElements users = window->userListElements();

	if (users.count() != 1)
	{
		searchInDirectoryActionActivated(0, false);
		return;
	}

	UserListElement user = *(users.constBegin());
	if (!user.usesProtocol("Gadu"))
		return;

	SearchDialog *sd = new SearchDialog(kadu, user.ID("Gadu").toUInt());
	sd->show();
	sd->firstSearch();

	kdebugf2();
}

void Kadu::notifyAboutUserActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	UserListElements users = window->userListElements();

	bool on = true;
	foreach(const UserListElement &user, users)
		if (!user.notify())
		{
			on = false;
			break;
		}

	foreach(const UserListElement &user, users)
		if (user.notify() == on)
			user.setNotify(!on);

	userlist->writeToConfig();

	foreach(KaduAction *action, notifyAboutUserActionDescription->actions())
	{
		if (action->userListElements() == users)
			action->setChecked(!on);
	}

	kdebugf2();
}

void Kadu::offlineToUserActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	if (toggled && !config_file.readBoolEntry("General", "PrivateStatus"))
	{
		if (MessageBox::ask("You need to have private status to do it, would you like to set private status now?"))
			changePrivateStatus->setChecked(true);
		else
		{
			sender->setChecked(!toggled);
			return;
		}
	}

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	UserListElements users = window->userListElements();

	bool on = true;
	foreach (const UserListElement &user, users)
		if (!user.usesProtocol("Gadu") || !user.protocolData("Gadu", "OfflineTo").toBool())
		{
			on = false;
			break;
		}

	foreach (const UserListElement &user, users)
		if (user.usesProtocol("Gadu") && user.protocolData("Gadu", "OfflineTo").toBool() == on)
			user.setProtocolData("Gadu", "OfflineTo", !on); // TODO: here boolean

	userlist->writeToConfig();

	foreach (KaduAction *action, offlineToUserActionDescription->actions())
	{
		if (action->userListElements() == users)
			action->setChecked(!on);
	}

	kdebugf2();
}

void Kadu::hideDescriptionActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	UserListElements users = window->userListElements();
	bool on = true;
	foreach(const UserListElement &user, users)
		if (user.data("HideDescription").toString() == "true")
		{
			on = false;
			break;
		}

	foreach(const UserListElement &user, users)
		user.setData("HideDescription", on ? "true" : "false"); // TODO: here string, LOL

	userlist->writeToConfig();

	foreach(KaduAction *action, hideDescriptionActionDescription->actions())
	{
		if (action->userListElements() == users)
			action->setChecked(on);
	}

	kdebugf2();
}

void Kadu::deleteUsersActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	UserListElements users = window->userListElements();
	removeUsers(users);

	kdebugf2();
}

void Kadu::openRecentChats(QAction *action)
{
	kdebugf();

	chat_manager->openPendingMsgs(chat_manager->closedChatUsers().at(action->data().toInt()));

	kdebugf2();
}

void Kadu::inactiveUsersActionActivated(QAction *sender, bool toggled)
{
	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	groups_manager->changeDisplayingOffline(window->userBox(), !toggled);
}

void Kadu::descriptionUsersActionActivated(QAction *sender, bool toggled)
{
	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	groups_manager->changeDisplayingWithoutDescription(window->userBox(), !toggled);
}

void Kadu::onlineAndDescUsersActionActivated(QAction *sender, bool toggled)
{
	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	groups_manager->changeDisplayingOnlineAndDescription(window->userBox(), toggled);
}

void Kadu::inactiveUsersActionCreated(KaduAction *action)
{
	action->setChecked(!config_file.readBoolEntry("General", "ShowOffline"));
}
void Kadu::descriptionUsersActionCreated(KaduAction *action)
{
	action->setChecked(!config_file.readBoolEntry("General", "ShowWithoutDescription"));
}
void Kadu::onlineAndDescUsersActionCreated(KaduAction *action)
{
	action->setChecked(config_file.readBoolEntry("General", "ShowOnlineAndDescription"));
}

void Kadu::configurationActionActivated(QAction *sender, bool toggled)
{
	MainConfigurationWindow::instance()->show();
}

void Kadu::editUserActionSetParams(QString /*protocolName*/, UserListElement user)
{
	kdebugf();
 	foreach(KaduAction *action, editUserActionDescription->actions())
	{
		KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(action->parent());
		if (!window)
			continue;

		UserListElements users = window->userListElements();

		if (users.count() == 1 && users[0] == user)
		{
			if (user.isAnonymous())
			{
				action->setIcon(icons_manager->loadIcon("AddUser"));
				action->setText(tr("Add user"));
			}
			else
			{
				action->setIcon(icons_manager->loadIcon("EditUserInfo"));
				action->setText(tr("Contact data"));
			}
		}
	}
	kdebugf2();
}

void Kadu::editUserActionCreated(KaduAction *action)
{
	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(action->parent());
	if (!window)
		return;

	UserListElements users = window->userListElements();
	if ((users.count()) == 1 && (*users.begin()).isAnonymous())
	{
		action->setIcon(icons_manager->loadIcon("AddUser"));
		action->setText(tr("Add user"));
	}
}

void Kadu::editUserActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;
	
	UserListElements selectedUsers = window->userListElements();
	
	if (selectedUsers.count() == 1)
		(new UserInfo(*selectedUsers.begin(), kadu))->show();
			
	kdebugf2();
}

void Kadu::addUserActionActivated(QAction *sender, bool toggled)
{
 	kdebugf();
	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (window)
	{	
		UserListElements selectedUsers = window->userListElements();
 		if ((selectedUsers.count() == 1) && (selectedUsers[0].isAnonymous()))
		{
 			(new UserInfo(selectedUsers[0], kadu))->show();
			return;
		}
	}
	(new UserInfo(UserListElement(), kadu))->show();

 	kdebugf2();
}

void Kadu::openChatWith()
{
	kdebugf();

	(new OpenChatWith(this))->show();

	kdebugf2();
}

void Kadu::personalInfo(QAction *sender, bool toggled)
{
	(new PersonalInfoDialog(kadu))->show();
}

void Kadu::manageIgnored(QAction *sender, bool toggled)
{
	(new Ignored(kadu, "ignored"))->show();
}

void Kadu::showStatusActionActivated(QAction *sender, bool toggled)
{
	statusMenu->exec(QCursor::pos());
}

void Kadu::showStatusActionCreated(KaduAction *action)
{
	action->setIcon(gadu->currentStatus().pixmap());
}

void Kadu::searchInDirectoryActionActivated(QAction *sender, bool toggled)
{
	(new SearchDialog(kadu))->show();
}

void Kadu::help(QAction *sender, bool toggled)
{
	if (config_file.readEntry("General", "Language", QString(QTextCodec::locale()).mid(0,2)) == "pl")
		openWebBrowser("http://www.kadu.net/w/Kadu:Pomoc");
	else
		openWebBrowser("http://www.kadu.net/w/English:Kadu:Help");
}

void Kadu::about(QAction *sender, bool toggled)
{
	(new About(kadu))->show();
}

void Kadu::quit()
{
	kdebugf();

	if (measureTime)
	{
		time_t sec;
		int msec;
		getTime(&sec, &msec);
		endingTime = (sec % 1000) * 1000 + msec;
	}
	qApp->quit();
}

void Kadu::importExportUserlist(QAction *sender, bool toggled)
{
	(new UserlistImportExport(kadu))->show();
}

void Kadu::hideKadu(QAction *sender, bool toggled)
{
	if (Docked)
		if (dontHideOnClose)
			showMinimized();
		else
			close();
}

void Kadu::changeAppearance()
{
	kdebugf();

	QApplication::setStyle(config_file.readEntry("Look", "QtStyle"));

	GroupBar->setFont(QFont(config_file.readFontEntry("Look", "UserboxFont").family(), config_file.readFontEntry("Look", "UserboxFont").pointSize(),75));

	if (config_file.readBoolEntry("Look", "ShowInfoPanel"))
	{
		InfoPanel->show();

		QFont font = config_file.readFontEntry("Look", "PanelFont");

		QString fontFamily = font.family();
		QString fontSize;
		if (font.pointSize() > 0)
			fontSize = QString::number(font.pointSize()) + "pt";
		else
			fontSize = QString::number(font.pixelSize()) + "px";
		QString fontStyle = font.italic() ? "italic" : "normal";
		QString fontWeight = font.bold() ? "bold" : "normal";
		QString textDecoration = font.underline() ? "underline" : "none";
		QString backgroundColor = config_file.readColorEntry("Look","InfoPanelBgColor").name();
		QString fontColor = config_file.readColorEntry("Look","InfoPanelFgColor").name();

		infoPanelStyle = QString(
			"html {"
			"	color: %1;"
			"	font: %2 %3 %4 %5;"
			"	text-decoration: %6;"
			"	margin: 0;"
			"	padding: 0;"
			"	background-color: %7;"
			"}"
			"div {"
			"	color: %1;"
			"	font: %2 %3 %4 %5;"
			"	text-decoration: %6;"
			"	margin: 0;"
			"	padding: 0;"
			"	background-color: %7;"
			"}").arg(fontColor, fontStyle, fontWeight, fontSize, fontFamily, textDecoration, backgroundColor);

 		if (config_file.readBoolEntry("Look", "PanelVerticalScrollbar"))
 			InfoPanel->page()->mainFrame()->setScrollBarPolicy (Qt::Vertical, Qt::ScrollBarAsNeeded);
 		else
 			InfoPanel->page()->mainFrame()->setScrollBarPolicy (Qt::Vertical, Qt::ScrollBarAlwaysOff);
	}
	else
		dynamic_cast<QWidget *>(InfoPanel)->hide();

	kadu->statusButton->setShown(config_file.readBoolEntry("Look", "ShowStatusButton"));

	const UserStatus &stat = gadu->currentStatus();

	QPixmap pix = stat.pixmap();
	QIcon icon(pix);
	statusButton->setIcon(icon);

	foreach(KaduAction *action, showStatusActionDescription->actions())
		action->setIcon(icon);

	setMainWindowIcon(pix);
	emit statusPixmapChanged(icon, stat.toString());
	kdebugf2();
}

void Kadu::removeUsers(UserListElements users)
{
	kdebugf();
	
	if (users.count())
	{
		QString altNicks = users.altNicks().join(", ");
		QString tmp;

		for (unsigned int i = 0; i < users.count(); i+=10)
			tmp += (altNicks.section(", ", i, (i + 9)) + ",\n");

		if (MessageBox::ask(tr("Selected users:\n%0will be deleted. Are you sure?").arg(tmp), "Warning", kadu))
		{
			emit removingUsers(users);
			userlist->removeUsers(users);
			userlist->writeToConfig();
		}
	}

	kdebugf2();
}

void Kadu::blink()
{
	QIcon icon;

	kdebugf();

	if (!DoBlink && !gadu->currentStatus().isOffline())
		return;
	else if (!DoBlink && gadu->currentStatus().isOffline())
	{
		icon = QIcon(gadu->nextStatus().pixmap(Offline, false));
		statusButton->setIcon(icon);
		emit statusPixmapChanged(icon, "Offline");
		return;
	}

	QString iconName;
	if (BlinkOn)
	{
		icon = QIcon(gadu->nextStatus().pixmap(Offline, false));
		iconName = "Offline";
	}
	else
	{
		const UserStatus &stat = gadu->nextStatus();
		icon = QIcon(stat.pixmap(NextStatus));
		iconName = stat.toString();
	}

	statusButton->setIcon(icon);
	emit statusPixmapChanged(icon, iconName);

	BlinkOn = !BlinkOn;

	blinktimer->start(1000, TRUE);
}

void Kadu::mouseButtonClicked(int button, Q3ListBoxItem *item)
{
	kdebugmf(KDEBUG_FUNCTION_START, "button=%d\n", button);
	if (!item)
		InfoPanel->setHtml("<body bgcolor=\"" + config_file.readEntry("Look", "InfoPanelBgColor") + "\"></body>");
	kdebugf2();
}

/* if something's pending, open it, if not, open new message */
void Kadu::sendMessage(UserListElement elem)
{
	kdebugf();
	UserBox *userbox = dynamic_cast<UserBox *>(sender());
	if (!userbox)
		return;

	UserListElements users  = userbox->selectedUsers();
	if (!users.isEmpty())
	{
		if (elem.usesProtocol("Gadu") && elem != Myself) //TODO: elem.hasFeature("SendingMessages")
			chat_manager->sendMessage(elem, users);
		else if (elem.mobile().isEmpty() && !elem.email().isEmpty())
			openMailClient(elem.email());

	}
	kdebugf2();
}

void Kadu::changeStatusSlot()
{
	QAction *action = dynamic_cast<QAction *>(sender());
	if (action)
	{
		foreach(QAction *action, changeStatusActionGroup->actions())
			action->setChecked(action->data().toInt() == gadu->currentStatus().index());
		slotHandleState(action->data().toInt());
	}
}

void Kadu::changePrivateStatusSlot(bool toggled)
{
	UserStatus status;
	status.setStatus(userStatusChanger->status());
	status.setFriendsOnly(toggled);
	setStatus(status);

	config_file.writeEntry("General", "PrivateStatus", toggled);
	UserBox::refreshAllLater();
}

/* when we want to change the status */
// TODO: fix it
void Kadu::slotHandleState(int command)
{
	kdebugf();

	UserStatus status;
	status.setStatus(userStatusChanger->status());
	switch (command)
	{
		case 0:
			status.setOnline();
			setStatus(status);
			break;
		case 1:
			status.setOnline(status.description());
			ChooseDescription::show(status, lastPositionBeforeStatusMenuHide);
			break;
		case 2:
			status.setBusy();
			setStatus(status);
			break;
		case 3:
			status.setBusy(status.description());
			ChooseDescription::show(status, lastPositionBeforeStatusMenuHide);
			break;
		case 4:
			status.setInvisible();
			setStatus(status);
			break;
		case 5:
			status.setInvisible(status.description());
			ChooseDescription::show(status, lastPositionBeforeStatusMenuHide);
			break;
		case 6:
			status.setOffline();
			setStatus(status);
			break;
		case 7:
			status.setOffline(status.description());
			ChooseDescription::show(status, lastPositionBeforeStatusMenuHide);
			break;
		case 8:
			status.setTalkWithMe();
			setStatus(status);
			break;
		case 9:
			status.setTalkWithMe(status.description());
			ChooseDescription::show(status, lastPositionBeforeStatusMenuHide);
			break;
		case 10:
			status.setDoNotDisturb();
			setStatus(status);
			break;
		case 11:
			status.setDoNotDisturb(status.description());
			ChooseDescription::show(status, lastPositionBeforeStatusMenuHide);
			break;
	}

	kdebugf2();
}

void Kadu::changeStatus(UserStatus newStatus)
{
	if (NextStatus.isOffline())
	{
		changeStatusToOfflineDesc->setEnabled(false);
		changeStatusToOffline->setChecked(true);
	}

	if (gadu->nextStatus() == newStatus)
		return;

	NextStatus.setStatus(newStatus);
	gadu->writeableStatus().setStatus(NextStatus);

}

void Kadu::connecting()
{
	kdebugf();

	DoBlink = true;

	if (!blinktimer)
	{
		blinktimer = new QTimer(this, "blinktimer");
		QObject::connect(blinktimer, SIGNAL(timeout()), kadu, SLOT(blink()));
	}

	blinktimer->start(1000, true);
	kdebugf2();
}

// TODO: move back to chatManager
void Kadu::messageReceived(Protocol *protocol, UserListElements senders, const QString &msg, time_t time)
{
	kdebugf();

	// TODO: workaround
	emit messageReceivedSignal(protocol, senders, msg, time);

	ChatWidget *chat = chat_manager->findChatWidget(senders);
	if (chat)
		chat->newMessage(protocol->protocolID(), senders, msg, time);
	else
	{
		if (config_file.readBoolEntry("General","AutoRaise"))
		{
			kadu->showNormal();
			kadu->setFocus();
		}

		if (config_file.readBoolEntry("Chat", "OpenChatOnMessage"))
		{
			if (config_file.readBoolEntry("Chat", "OpenChatOnMessageWhenOnline") && !Myself.status(protocol->protocolID()).isOnline())
			{
				pending.addMsg(protocol->protocolID(), senders, msg, GG_CLASS_CHAT, time);
				return;
			}

			// TODO: it is lame
			chat_manager->openChatWidget(protocol, senders);
			chat = chat_manager->findChatWidget(senders);
			chat->newMessage(protocol->protocolID(), senders, msg, time);
		}
		else
			pending.addMsg(protocol->protocolID(), senders, msg, GG_CLASS_CHAT, time);
	}

	kdebugf2();
}

void Kadu::connected()
{
	kdebugf();
	DoBlink = false;
	kdebugf2();
}

void Kadu::imageReceivedAndSaved(UinType sender, quint32 size, quint32 crc32, const QString &/*path*/)
{
	for (int i = 0, count = pending.count(); i < count; i++)
	{
		PendingMsgs::Element& e = pending[i];
		e.msg = gadu_images_manager.replaceLoadingImages(e.msg, sender, size, crc32);
	}
}

// TODO: remove
void Kadu::systemMessageReceived(const QString &msg)
{
//	MessageBox::msg(msg);
}

void Kadu::disconnected()
{
	kdebugmf(KDEBUG_FUNCTION_START, "Disconnection has occured\n");

	chat_manager->refreshTitles();

	DoBlink = false;

	if (blinktimer)
	{
		blinktimer->stop();
		delete blinktimer;
		blinktimer = NULL;
	}

	kdebugf2();
}

bool Kadu::close(bool quit)
{
	int wasHidden = 0;
	if (!quit && Docked && !dontHideOnClose)
	{
		kdebugmf(KDEBUG_INFO, "hiding\n");
		hide();
		return false;
	}
	else
	{
		Closing = true;

		/* Dorr: on Gnome kadu doesn't save position properly when hidden */
		if ((wasHidden = isHidden())) 
			show(); 

		writeToolBarsToConfig("");

		if (config_file.readBoolEntry("Look", "ShowInfoPanel"))
		{
			config_file.writeEntry("General", "UserBoxHeight", Userbox->height());
			config_file.writeEntry("General", "DescriptionHeight", InfoPanel->height());
		}
		if (config_file.readBoolEntry("Look", "ShowStatusButton"))
			config_file.writeEntry("General", "UserBoxHeight", Userbox->height());
 		saveWindowGeometry(this, "General", "Geometry");

		if (wasHidden) hide(); /* Dorr: now we can hide it again */

		config_file.writeEntry("General", "DefaultDescription", defaultdescriptions.join("<-->"));

		if (config_file.readEntry("General", "StartupStatus") == "LastStatus")
			config_file.writeEntry("General", "LastStatusIndex", userStatusChanger->status().index());

		if (config_file.readBoolEntry("General", "StartupLastDescription"))
			config_file.writeEntry("General", "LastStatusDescription", userStatusChanger->status().description());

		pending.writeToFile();
		IgnoredManager::writeToConfiguration();
		if (!gadu->currentStatus().isOffline())
			if (config_file.readBoolEntry("General", "DisconnectWithCurrentDescription"))
				setOffline(gadu->currentStatus().description());
			else
				setOffline(config_file.readEntry("General", "DisconnectDescription"));

		xml_config_file->makeBackup();
		ModulesManager::closeModule();

		Updates::closeModule();
		delete defaultFontInfo;
		delete defaultFont;

		disconnect(gadu, SIGNAL(messageReceived(Protocol *, UserListElements, const QString &, time_t)),
				this, SLOT(messageReceived(Protocol *, UserListElements, const QString &, time_t)));
		disconnect(gadu, SIGNAL(connecting()), this, SLOT(connecting()));
		disconnect(gadu, SIGNAL(connected()), this, SLOT(connected()));
		disconnect(gadu, SIGNAL(disconnected()), this, SLOT(disconnected()));
		disconnect(gadu, SIGNAL(imageReceivedAndSaved(UinType, quint32, quint32, const QString &)),
				this, SLOT(imageReceivedAndSaved(UinType, quint32, quint32, const QString &)));
		disconnect(gadu, SIGNAL(needTokenValue(QPixmap, QString &)),
				this, SLOT(readTokenValue(QPixmap, QString &)));
		disconnect(gadu, SIGNAL(systemMessageReceived(const QString &)), this, SLOT(systemMessageReceived(const QString &)));

		disconnect(userlist, SIGNAL(protocolUserDataChanged(QString, UserListElement, QString, QVariant, QVariant, bool, bool)),
				this, SLOT(editUserActionSetParams(QString, UserListElement)));

		disconnect(userlist, SIGNAL(usersDataChanged(QString)), this, SLOT(updateInformationPanelLater()));
		disconnect(userlist, SIGNAL(protocolUsersDataChanged(QString, QString)), this, SLOT(updateInformationPanelLater()));
		disconnect(userlist, SIGNAL(usersStatusChanged(QString)), this, SLOT(updateInformationPanelLater()));

		disconnect(&(gadu->currentStatus()), SIGNAL(goOnline(const QString &)),
				this, SLOT(wentOnline(const QString &)));
		disconnect(&(gadu->currentStatus()), SIGNAL(goBusy(const QString &)),
				this, SLOT(wentBusy(const QString &)));
		disconnect(&(gadu->currentStatus()), SIGNAL(goInvisible(const QString &)),
				this, SLOT(wentInvisible(const QString &)));
		disconnect(&(gadu->currentStatus()), SIGNAL(goOffline(const QString &)),
				this, SLOT(wentOffline(const QString &)));
		disconnect(&(gadu->currentStatus()), SIGNAL(goTalkWithMe(const QString &)),
				this, SLOT(wentTalkWithMe(const QString &)));
		disconnect(&(gadu->currentStatus()), SIGNAL(goDoNotDisturb(const QString &)),
				this, SLOT(wentDoNotDisturb(const QString &)));

		disconnect(Userbox, SIGNAL(doubleClicked(UserListElement)), this, SLOT(sendMessage(UserListElement)));
		disconnect(Userbox, SIGNAL(returnPressed(UserListElement)), this, SLOT(sendMessage(UserListElement)));
		disconnect(Userbox, SIGNAL(mouseButtonClicked(int, Q3ListBoxItem *, const QPoint &)),
				this, SLOT(mouseButtonClicked(int, Q3ListBoxItem *)));
		disconnect(Userbox, SIGNAL(currentChanged(UserListElement)), this, SLOT(currentChanged(UserListElement)));

#if 0
		status_changer_manager->unregisterStatusChanger(splitStatusChanger);
		delete splitStatusChanger;
		splitStatusChanger = 0;
#endif

		status_changer_manager->unregisterStatusChanger(userStatusChanger);
		delete userStatusChanger;
		userStatusChanger = 0;

		StatusChangerManager::closeModule();

		UserBox::closeModule();
		ChatManager::closeModule();
		SearchDialog::closeModule();
		GaduProtocol::closeModule();

		userlist->writeToConfig();//writeToConfig must be before GroupsManager::closeModule, because GM::cM removes all groups from userlist
		GroupsManager::closeModule();
		xml_config_file->sync();
		UserList::closeModule();
		ProtocolsManager::closeModule();
		EmoticonsManager::closeModule();
		IconsManager::closeModule();

#ifdef Q_OS_MACX
		//na koniec przywracamy domy�ln� ikon�, je�eli tego nie zrobimy, to pozostanie bie��cy status
		QApplication::setWindowIcon(QPixmap(dataPath("kadu.png")));
#endif

		QWidget::close(true);

		kdebugmf(KDEBUG_INFO, "Graceful shutdown...\n");

		qDeleteAll(children());

		delete xml_config_file;
		delete config_file_ptr;

		return true;
	}
}

void Kadu::quitApplication()
{
	kdebugf();
	close(true);
}

Kadu::~Kadu(void)
{
	kdebugf();

	qDeleteAll(KaduActions.values());

	delete selectedUsers;

	kdebugf2();
}

void Kadu::createRecentChatsMenu()
{
	kdebugf();

	RecentChatsMenu->clear();
	QAction *action;
	if (chat_manager->closedChatUsers().isEmpty())
	{
		action = RecentChatsMenu->addAction(tr("No closed chats found")/*, 0*/);
		action->setEnabled(false);

		kdebugf2();
		return;
	}

	unsigned int index = 0; // indeks pozycji w popupie

	foreach(const UserListElements &users, chat_manager->closedChatUsers())
	{
		QStringList altnicks = users.altNicks(); // lista nick�w z okna rozmowy
		QString chat_users;

		if (altnicks.count() <= 5)
			chat_users = altnicks.join(", ");
		else // je�eli jest wi�cej ni� pi�ciu u�ytkownik�w...
		{
			for (int i = 0; i < 4; i++) // to i tak dodajemy tylko pierwszych pi�ciu :)
				chat_users.append(*altnicks.at(i) + ", ");
			chat_users.append(*altnicks.at(4) + " [...]");
		}
		action = new QAction(icons_manager->loadIcon("OpenChat"), chat_users, this);
		action->setData(index);
		RecentChatsMenu->addAction(action);

		index++;
	}

	kdebugf2();
}

void Kadu::addMenuActionDescription(ActionDescription *actionDescription)
{
	if (!actionDescription)
		return;
	KaduAction *action = actionDescription->createAction(this);
	MainMenu->addAction(action);
	mainMenuActions[actionDescription] = action;
}

void Kadu::insertMenuActionDescription(int pos, ActionDescription *actionDescription)
{
	if (!actionDescription)
		return;
	KaduAction *action = actionDescription->createAction(this);
	QList<QAction *> menuActions = MainMenu->actions();
	if (pos >= menuActions.count() - 1)
		MainMenu->addAction(action);
	else
		MainMenu->insertAction(menuActions[pos], action);

	mainMenuActions[actionDescription] = action;
}

void Kadu::insertMenuSubmenu(int pos, QMenu *menu)
{
	QList<QAction *> menuActions = MainMenu->actions();
	if (pos >= menuActions.count() - 1)
		MainMenu->addMenu(menu);
	else
		MainMenu->insertMenu(menuActions[pos], menu);
}

QAction * Kadu::addMenuSeparator()
{
	return MainMenu->addSeparator();
}

void Kadu::removeMenuSeparator(QAction * separator)
{
	if (!separator)
		return;
	MainMenu->removeAction(separator);
}

void Kadu::removeMenuActionDescription(ActionDescription *actionDescription)
{
	if (!actionDescription)
		return;
	KaduAction *action = mainMenuActions[actionDescription];
	
	if (!action)
		return;

	MainMenu->removeAction(action);
	mainMenuActions.remove(actionDescription);

}

void Kadu::removeMenuSubmenu(QMenu *menu)
{
	MainMenu->removeAction(menu->menuAction());
}

void Kadu::createMenu()
{
	kdebugf();
	MainMenu = new QMenu;
	MainMenu->setTitle(tr("Menu"));
	RecentChatsMenu = new QMenu;
	RecentChatsMenu->setIcon(icons_manager->loadIcon("OpenChat"));
	RecentChatsMenu->setTitle(tr("Recent chats..."));
	connect(RecentChatsMenu, SIGNAL(triggered(QAction *)), this, SLOT(openRecentChats(QAction *)));	
	
	ActionDescription *manageIgnoredActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "manageIgnoredAction",
		this, SLOT(manageIgnored(QAction *, bool)),
		"Ignore", tr("Manage &ignored")
	);
	addMenuActionDescription(manageIgnoredActionDescription);
	addMenuActionDescription(configurationActionDescription);

	addMenuSeparator();
	ActionDescription *personalInfoActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "personalInfoAction",
		this, SLOT(personalInfo(QAction *, bool)),
		"PersonalInfo", tr("Personal information")
	);
	addMenuActionDescription(personalInfoActionDescription);
	personalInfoMenuId = MainMenu->actions().indexOf(mainMenuActions[personalInfoActionDescription]) + 1;

	addMenuSeparator();

	MainMenu->addMenu(RecentChatsMenu);
	addMenuActionDescription(openSearchActionDescription);
	ActionDescription *importExportUserlisActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "importExportUserlisAction",
		this, SLOT(importExportUserlist(QAction *, bool)),
		"ImportExport", tr("I&mport / Export userlist")
	);
	addMenuActionDescription(importExportUserlisActionDescription);
	addMenuActionDescription(addUserActionDescription); 
	addMenuActionDescription(chat_manager->openChatWithActionDescription);

	addMenuSeparator();
	ActionDescription *helpActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "helpAction",
		this, SLOT(help(QAction *, bool)),
		"HelpMenuItem", tr("H&elp")
	);
	addMenuActionDescription(helpActionDescription);

	ActionDescription *aboutActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "aboutAction",
		this, SLOT(about(QAction *, bool)),
		"AboutMenuItem", tr("A&bout...")
	);
	addMenuActionDescription(aboutActionDescription);

	addMenuSeparator();

	ActionDescription *hideKaduActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "hideKaduAction",
		this, SLOT(hideKadu(QAction *, bool)),
		"HideKadu", tr("&Hide Kadu")
	);
	addMenuActionDescription(hideKaduActionDescription);

	ActionDescription *exitKaduActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "exitKaduAction",
		this, SLOT(quit()),
		"Exit", tr("&Exit Kadu")
	);
	addMenuActionDescription(exitKaduActionDescription);

	menuBar()->addMenu(MainMenu);

	kdebugf2();
}

void Kadu::statusMenuAboutToHide()
{
	lastPositionBeforeStatusMenuHide = statusMenu->pos();
}

void Kadu::dockMenuAboutToHide()
{
	lastPositionBeforeStatusMenuHide = dockMenu->pos();
}

void Kadu::createStatusPopupMenu()
{
	kdebugf();

	QPixmap pix;
	QIcon icon;

	statusMenu = new QMenu(this);
	dockMenu = new QMenu(this);

	changeStatusActionGroup = new QActionGroup(this);
	changeStatusActionGroup->setExclusive(false); // HACK

	GaduStatus s;

	changeStatusToOnline = new QAction(icons_manager->loadIcon(s.pixmapName(Online, false, false)), tr("Online"), this);
	changeStatusToOnline->setCheckable(true);
	changeStatusToOnline->setData(0);
	connect(changeStatusToOnline, SIGNAL(triggered()), this, SLOT(changeStatusSlot()));

	changeStatusToOnlineDesc = new QAction(icons_manager->loadIcon(s.pixmapName(Online, true, false)), tr("Online (d.)"), this);
	changeStatusToOnlineDesc->setCheckable(true);
	changeStatusToOnlineDesc->setData(1);
	connect(changeStatusToOnlineDesc, SIGNAL(triggered()), this, SLOT(changeStatusSlot()));

	changeStatusToBusy = new QAction(icons_manager->loadIcon(s.pixmapName(Busy, false, false)), tr("Busy"), this);
	changeStatusToBusy->setCheckable(true);
	changeStatusToBusy->setData(2);
	connect(changeStatusToBusy, SIGNAL(triggered()), this, SLOT(changeStatusSlot()));

	changeStatusToBusyDesc = new QAction(icons_manager->loadIcon(s.pixmapName(Busy, true, false)), tr("Busy (d.)"), this);
	changeStatusToBusyDesc->setCheckable(true);
	changeStatusToBusyDesc->setData(3);
	connect(changeStatusToBusyDesc, SIGNAL(triggered()), this, SLOT(changeStatusSlot()));

	changeStatusToInvisible = new QAction(icons_manager->loadIcon(s.pixmapName(Invisible, false, false)), tr("Invisible"), this);
	changeStatusToInvisible->setCheckable(true);
	changeStatusToInvisible->setData(4);
	connect(changeStatusToInvisible, SIGNAL(triggered()), this, SLOT(changeStatusSlot()));

	changeStatusToInvisibleDesc = new QAction(icons_manager->loadIcon(s.pixmapName(Invisible, true, false)), tr("Invisible (d.)"), this);
	changeStatusToInvisibleDesc->setCheckable(true);
	changeStatusToInvisibleDesc->setData(5);
	connect(changeStatusToInvisibleDesc, SIGNAL(triggered()), this, SLOT(changeStatusSlot()));

	changeStatusToOffline = new QAction(icons_manager->loadIcon(s.pixmapName(Offline, false, false)), tr("Offline"), this);
	changeStatusToOffline->setCheckable(true);
	changeStatusToOffline->setData(6);
	connect(changeStatusToOffline, SIGNAL(triggered()), this, SLOT(changeStatusSlot()));

	changeStatusToOfflineDesc = new QAction(icons_manager->loadIcon(s.pixmapName(Offline, true, false)), tr("Offline (d.)"), this);
	changeStatusToOfflineDesc->setCheckable(true);
	changeStatusToOfflineDesc->setData(7);
	connect(changeStatusToOfflineDesc, SIGNAL(triggered()), this, SLOT(changeStatusSlot()));

	changeStatusToTalkWithMe = new QAction(icons_manager->loadIcon(s.pixmapName(FFC, false, false)), tr("Talk With Me"), this);
	changeStatusToTalkWithMe->setCheckable(true);
	changeStatusToTalkWithMe->setData(8);
	connect(changeStatusToTalkWithMe, SIGNAL(triggered()), this, SLOT(changeStatusSlot()));

	changeStatusToTalkWithMeDesc = new QAction(icons_manager->loadIcon(s.pixmapName(FFC, true, false)), tr("Talk With Me (d.)"), this);
	changeStatusToTalkWithMeDesc->setCheckable(true);
	changeStatusToTalkWithMeDesc->setData(9);
	connect(changeStatusToTalkWithMeDesc, SIGNAL(triggered()), this, SLOT(changeStatusSlot()));

	changeStatusToDoNotDisturb = new QAction(icons_manager->loadIcon(s.pixmapName(DND, false, false)), tr("Do Not Disturb"), this);
	changeStatusToDoNotDisturb->setCheckable(true);
	changeStatusToDoNotDisturb->setData(10);
	connect(changeStatusToDoNotDisturb, SIGNAL(triggered()), this, SLOT(changeStatusSlot()));

	changeStatusToDoNotDisturbDesc = new QAction(icons_manager->loadIcon(s.pixmapName(DND, true, false)), tr("Do Not Disturb (d.)"), this);
	changeStatusToDoNotDisturbDesc->setCheckable(true);
	changeStatusToDoNotDisturbDesc->setData(11);
	connect(changeStatusToDoNotDisturbDesc, SIGNAL(triggered()), this, SLOT(changeStatusSlot()));

	changePrivateStatus = new QAction(tr("Private"), this);
	changePrivateStatus->setCheckable(true);
	connect(changePrivateStatus, SIGNAL(toggled(bool)), this, SLOT(changePrivateStatusSlot(bool)));

	bool privateStatus = config_file.readBoolEntry("General", "PrivateStatus");
	changePrivateStatus->setChecked(privateStatus);

	changeStatusActionGroup->addAction(changeStatusToOnline);
	changeStatusActionGroup->addAction(changeStatusToOnlineDesc);
	changeStatusActionGroup->addAction(changeStatusToBusy);
	changeStatusActionGroup->addAction(changeStatusToBusyDesc);
	changeStatusActionGroup->addAction(changeStatusToInvisible);
	changeStatusActionGroup->addAction(changeStatusToInvisibleDesc);
	changeStatusActionGroup->addAction(changeStatusToOffline);
	changeStatusActionGroup->addAction(changeStatusToOfflineDesc);
	changeStatusActionGroup->addAction(changeStatusToTalkWithMe);
	changeStatusActionGroup->addAction(changeStatusToTalkWithMeDesc);
	changeStatusActionGroup->addAction(changeStatusToDoNotDisturb);
	changeStatusActionGroup->addAction(changeStatusToDoNotDisturbDesc);

	statusMenu->addAction(changeStatusToOnline);
	statusMenu->addAction(changeStatusToOnlineDesc);
	statusMenu->addAction(changeStatusToBusy);
	statusMenu->addAction(changeStatusToBusyDesc);
	statusMenu->addAction(changeStatusToInvisible);
	statusMenu->addAction(changeStatusToInvisibleDesc);
	statusMenu->addAction(changeStatusToOffline);
	statusMenu->addAction(changeStatusToOfflineDesc);
	statusMenu->addAction(changeStatusToTalkWithMe);
	statusMenu->addAction(changeStatusToTalkWithMeDesc);
	statusMenu->addAction(changeStatusToDoNotDisturb);
	statusMenu->addAction(changeStatusToDoNotDisturbDesc);
	statusMenu->addSeparator();
	statusMenu->addAction(changePrivateStatus);

	dockMenu->addAction(changeStatusToOnline);
	dockMenu->addAction(changeStatusToOnlineDesc);
	dockMenu->addAction(changeStatusToBusy);
	dockMenu->addAction(changeStatusToBusyDesc);
	dockMenu->addAction(changeStatusToInvisible);
	dockMenu->addAction(changeStatusToInvisibleDesc);
	dockMenu->addAction(changeStatusToOffline);
	dockMenu->addAction(changeStatusToOfflineDesc);
	dockMenu->addAction(changeStatusToTalkWithMe);
	dockMenu->addAction(changeStatusToTalkWithMeDesc);
	dockMenu->addAction(changeStatusToDoNotDisturb);
	dockMenu->addAction(changeStatusToDoNotDisturbDesc);
	dockMenu->addSeparator();
	dockMenu->addAction(changePrivateStatus);

	kdebugf2();
}

void Kadu::showdesc(bool show)
{
	if (show)
		InfoPanel->show();
	else
		InfoPanel->QWidget::hide();
}

void Kadu::updateInformationPanelLater()
{
	updateInformationPanelTimer.start(0, true);
}

void Kadu::updateInformationPanel()
{
	if (Userbox->currentUserExists())
		updateInformationPanel(Userbox->currentUser());
}

void Kadu::updateInformationPanel(UserListElement user)
{
	if (!config_file.readBoolEntry("Look", "ShowInfoPanel"))
		return;
	if (Userbox->currentUserExists() && user == Userbox->currentUser())
	{
		kdebugmf(KDEBUG_INFO, "%s\n", qPrintable(user.altNick()));
		QString text = QString(
			"<html>"
			"	<head>"
			"		<style type='text/css'>") +
			infoPanelStyle +
			"		</style>"
			"	</head>"
			"	<body>";
		HtmlDocument doc;
		doc.parseHtml(KaduParser::parse(InfoPanelSyntax, user));
		doc.convertUrlsToHtml();
		doc.convertMailToHtml();
		if((EmoticonsStyle)config_file.readNumEntry("Chat", "EmoticonsStyle") != EMOTS_NONE && config_file.readBoolEntry("General", "ShowEmotPanel"))
			emoticons->expandEmoticons(doc, config_file.readColorEntry("Look", "InfoPanelBgColor"), (EmoticonsStyle)config_file.readNumEntry("Chat", "EmoticonsStyle"));

		text += doc.generateHtml();
		text += "</body></html>";
		InfoPanel->setHtml(text);

		kdebugf2();
	}
}

void Kadu::currentChanged(UserListElement user)
{
	updateInformationPanel(user);
}

void Kadu::infoPanelSyntaxFixup(QString &syntax)
{
	syntax = QString("<html><head><style type='text/css'>%1</style></head><body>%2</body>").arg(infoPanelStyle, syntax);
}

// QMenuBar* Kadu::menuBar() const
// {
// 	return MenuBar;
// }

//QMenu* Kadu::mainMenu() const
//{
//	return MainMenu;
//}

KaduTabBar* Kadu::groupBar() const
{
	return GroupBar;
}

UserBox* Kadu::userbox() const
{
	return Userbox;
}

UserListElements Kadu::userListElements()
{
	return Userbox->selectedUsers();
}

void Kadu::setDocked(bool docked, bool dontHideOnClose1)
{
	Docked = docked;
	dontHideOnClose = dontHideOnClose1;
	qApp->setQuitOnLastWindowClosed(!Docked || dontHideOnClose);

// 	if (config_file.readBoolEntry("General", "ShowAnonymousWithMsgs") || !Docked || dontHideOnClose)
// 	{
	Userbox->removeNegativeFilter(anonymousUsers);
	Userbox->applyNegativeFilter(anonymousUsersWithoutMessages);
// 	}
// 	else
// 	{
// 		Userbox->removeNegativeFilter(anonymousUsersWithoutMessages);
// 		Userbox->applyNegativeFilter(anonymousUsers);
// 	}
}

bool Kadu::docked() const
{
	return Docked;
}

void Kadu::show()
{
	QWidget::show();

	// TODO: remove after 0.6
	if (config_file.readBoolEntry("Look", "MultiColumnUserbox", false))
	{
		int columns = Userbox->visibleWidth() / config_file.readUnsignedNumEntry("Look", "MultiColumnUserboxWidth", Userbox->visibleWidth());
		if (columns < 1)
			columns = 1;
		config_file.writeEntry("Look", "UserBoxColumnCount", columns);
		KaduListBoxPixmap::setColumnCount(columns);
		Userbox->refresh();
	}
	config_file.removeVariable("Look", "MultiColumnUserbox");
	config_file.removeVariable("Look", "MultiColumnUserboxWidth");

	emit shown();
}

void Kadu::hide()
{
	emit hiding();
	QWidget::hide();
}

void Kadu::refreshPrivateStatusFromConfigFile()
{
	bool privateStatus = config_file.readBoolEntry("General", "PrivateStatus");

	// je�li stan nie uleg� zmianie to nic nie robimy
	if (changePrivateStatus->isChecked() == privateStatus)
		return;

	UserStatus status = userStatusChanger->status();
	status.setFriendsOnly(privateStatus);
	userStatusChanger->userStatusSet(status);

	changePrivateStatus->setChecked(privateStatus);
}

void Kadu::configurationUpdated()
{
	refreshPrivateStatusFromConfigFile();

	changeAppearance();
	groups_manager->refreshTabBar();
	UserBox::setColorsOrBackgrounds();

	QString uin = QString::number(config_file.readUnsignedNumEntry("General", "UIN", 0));
	if (Myself.ID("Gadu").toUInt() != uin.toUInt())
	{
		gadu->changeID(uin);
		Myself.deleteProtocol("Gadu");
		if (uin.toUInt())
			Myself.addProtocol("Gadu", uin);
		kadu->setWindowTitle(tr("Kadu: %1").arg(uin));
	}
	Myself.setAltNick(config_file.readEntry("General", "Nick"));

	kadu->setDocked(kadu->Docked, kadu->dontHideOnClose);

	InfoPanelSyntax = SyntaxList::readSyntax("infopanel", config_file.readEntry("Look", "InfoPanelSyntaxFile"),
		"<table><tr><td><img width=\"32\" height=\"32\" align=\"left\" valign=\"top\" src=\"file:///@{ManageUsersWindowIcon}\"></td><td> "
		"<div align=\"left\"> [<b>%a</b>][ (%u)] [<br>tel.: %m][<br>IP: %i]</div></td></tr></table> <hr> <b>%s</b> [<br>%d]");
	InfoPanel->setHtml("<body bgcolor=\"" + config_file.readEntry("Look", "InfoPanelBgColor") + "\"></body>");
	updateInformationPanel();

#ifdef Q_OS_WIN
	QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
		       QSettings::NativeFormat);
	if(config_file.readBoolEntry("General", "RunOnStartup"))
		settings.setValue("Kadu", 
				QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
	else
		settings.remove("Kadu");
#endif

#ifdef DEBUG_ENABLED
	debug_mask = config_file.readNumEntry("General", "DEBUG_MASK");
	gg_debug_level = debug_mask | ~255;
#endif
}

void Kadu::resizeEvent(QResizeEvent *e)
{
//	kdebugm(KDEBUG_WARNING, "MenuBar->h4w(%d):%d, MenuBar->height():%d, MenuBar->sizeHint().height():%d\n", width(), MenuBar->heightForWidth(width()), MenuBar->height(), MenuBar->sizeHint().height());
//hack
// 	menuBox->setMaximumHeight(menuBar()->heightForWidth(width()));
	QWidget::resizeEvent(e);
}

void Kadu::setDefaultStatus()
{
	kdebugf();

	if (config_file.readNumEntry("General", "UIN", 0) == 0 || config_file.readEntry("General", "Password").isEmpty())
		return;

	QString description;
	QString startupStatus = config_file.readEntry("General", "StartupStatus");
	UserStatus status;

	if (config_file.readBoolEntry("General", "StartupLastDescription"))
		description = config_file.readEntry("General", "LastStatusDescription");
	else
		description = config_file.readEntry("General", "StartupDescription");

	int statusIndex;

	bool offlineToInvisible = false;

	if (startupStatus == "LastStatus")
	{
		statusIndex = config_file.readNumEntry("General", "LastStatusIndex", UserStatus::index(Offline, false));
		offlineToInvisible = config_file.readBoolEntry("General", "StartupStatusInvisibleWhenLastWasOffline");
	}
	else if (startupStatus == "Online")
		statusIndex = UserStatus::index(Online, false);
	else if (startupStatus == "Busy")
		statusIndex = UserStatus::index(Busy, false);
	else if (startupStatus == "Invisible")
		statusIndex = UserStatus::index(Invisible, false);
	else if (startupStatus == "Offline")
		statusIndex = UserStatus::index(Offline, false);
	else if (startupStatus == "Talk With Me")
		statusIndex = UserStatus::index(FFC, false);
	else if (startupStatus == "Do Not Disturb")
		statusIndex = UserStatus::index(DND, false);

	if (UserStatus::isOffline(statusIndex) && offlineToInvisible)
		status.setInvisible(description);
	else
		status.setIndex(statusIndex, description);

	status.setFriendsOnly(config_file.readBoolEntry("General", "PrivateStatus"));
	userStatusChanger->userStatusSet(status);

	kdebugf2();
}

void Kadu::startupProcedure()
{
	kdebugf();

	if (ShowMainWindowOnStart)
		show();

	Updates::initModule();

	xml_config_file->makeBackup();

	status_changer_manager->enable();
	setDefaultStatus();

	kdebugf2();
}

void Kadu::setShowMainWindowOnStart(bool show)
{
	ShowMainWindowOnStart = show;
}

void Kadu::wentOnline(const QString &desc)
{
	kdebugf();
	DoBlink = false;
	showStatusOnMenu(UserStatus::index(Online, !desc.isEmpty()));

}

void Kadu::wentBusy(const QString &desc)
{
	kdebugf();
	DoBlink = false;
	showStatusOnMenu(UserStatus::index(Busy, !desc.isEmpty()));
}

void Kadu::wentInvisible(const QString &desc)
{
	kdebugf();
	DoBlink = false;
	showStatusOnMenu(UserStatus::index(Invisible, !desc.isEmpty()));
}

void Kadu::wentOffline(const QString &desc)
{
	kdebugf();
	DoBlink = false;
	showStatusOnMenu(UserStatus::index(Offline, !desc.isEmpty()));
}

void Kadu::wentTalkWithMe(const QString &desc)
{
	kdebugf();
	DoBlink = false;
	showStatusOnMenu(UserStatus::index(FFC, !desc.isEmpty()));
}

void Kadu::wentDoNotDisturb(const QString &desc)
{
	kdebugf();
	DoBlink = false;
	showStatusOnMenu(UserStatus::index(DND, !desc.isEmpty()));
}

void Kadu::showStatusOnMenu(int statusNr)
{
	kdebugf();

	QList<QAction*> statusActions = changeStatusActionGroup->actions();
	for (int i = 0; i < UserStatus::count()-1; ++i)
		statusActions[i]->setChecked(i == statusNr);

	changePrivateStatus->setChecked(gadu->currentStatus().isFriendsOnly());

	statusButton->setText(qApp->translate("@default", gadu->currentStatus().name().ascii()));
	changeStatusToOfflineDesc->setEnabled(statusNr != 6);
	changeStatusToOffline->setEnabled(statusNr != 7);

	QPixmap pix = gadu->currentStatus().pixmap();
	QIcon icon(pix);
	QString iconName = gadu->currentStatus().toString();

	statusButton->setIcon(icon);
	setMainWindowIcon(pix);

	foreach (KaduAction *action, showStatusActionDescription->actions())
		action->setIcon(icon);

	emit statusPixmapChanged(icon, iconName);
}

void Kadu::readTokenValue(QPixmap tokenImage, QString &tokenValue)
{
	TokenDialog *td = new TokenDialog(tokenImage, 0);

	if (td->exec() == QDialog::Accepted)
		td->getValue(tokenValue);
	else
		tokenValue.truncate(0);

	delete td;
}

char *SystemUserName;
void Kadu::deleteOldConfigFiles()
{
	kdebugf();
	////////////start - to be removed at 0.6-svn
	QDir oldConfigs(ggPath(), "kadu.conf.backup.*", QDir::Name, QDir::Files);
//	kdebugm(KDEBUG_INFO, "%d\n", oldConfigs.count());
	if (oldConfigs.count() > 20)
		for (unsigned int i = 0, max = oldConfigs.count() - 20; i < max; ++i)
		{
//			kdebugm(KDEBUG_DUMP, "deleting %s\n", qPrintable(oldConfigs[i]));
			QFile::remove(ggPath(oldConfigs[i]));
		}
//	kdebugm(KDEBUG_INFO, "configs deleted\n");
	//////////stop - to be removed at 0.6-svn

	QDir oldConfigs2(ggPath(), "kadu.conf.xml.backup.*", QDir::Name, QDir::Files);
//	kdebugm(KDEBUG_INFO, "%d\n", oldConfigs2.count());
	if (oldConfigs2.count() > 20)
		for (unsigned int i = 0, max = oldConfigs2.count() - 20; i < max; ++i)
		{
//			kdebugm(KDEBUG_DUMP, "deleting %s\n", qPrintable(oldConfigs2[i]));
			QFile::remove(ggPath(oldConfigs2[i]));
		}
//	kdebugm(KDEBUG_INFO, "configs2 deleted\n");

	QDir oldBacktraces(ggPath(), "kadu.backtrace.*", QDir::Name, QDir::Files);
	if (oldBacktraces.count() > 20)
		for (unsigned int i = 0, max = oldBacktraces.count() - 20; i < max; ++i)
		{
//			kdebugm(KDEBUG_DUMP, "deleting %s\n", qPrintable(oldBacktraces[i]));
			QFile::remove(ggPath(oldBacktraces[i]));
		}
//	kdebugm(KDEBUG_INFO, "bts deleted\n");

#ifdef Q_OS_WIN
	QString tmp(getenv("TEMP") ? getenv("TEMP") : ".");
	QString mask("kadu-dbg-*.txt");
#else
	QString tmp("/tmp");
	QString mask=QString("kadu-%1-*.dbg").arg(SystemUserName);
#endif
	QDir oldDebugs(tmp, mask, QDir::Name, QDir::Files);
	if (oldDebugs.count() > 5)
		for (unsigned int i = 0, max = oldDebugs.count() - 5; i < max; ++i)
		{
			kdebugm(KDEBUG_DUMP, "deleting %s\n", qPrintable(oldDebugs[i]));
			QFile::remove(tmp + "/" + oldDebugs[i]);
		}
//	kdebugm(KDEBUG_INFO, "debugs deleted\n");
	kdebugf2();
}

void Kadu::setMainWindowIcon(const QPixmap &icon)
{
	/* Dorr: On Mac OS X macosx_docking module handles this */
#ifndef Q_OS_MAC
	bool blocked = false;
	emit settingMainIconBlocked(blocked);
	if (!blocked)
	{
		setWindowIcon(icon);
		QApplication::setWindowIcon(icon);
	}
#endif
}

const QDateTime &Kadu::startTime() const
{
	return StartTime;
}

void Kadu::customEvent(QEvent *e)
{
	if (int(e->type()) == 4321)
		show();
//		QTimer::singleShot(0, this, SLOT(show()));
	else if (int(e->type()) == 5432)
	{
		OpenGGChatEvent *ev = static_cast<OpenGGChatEvent *>(e);
		if (ev->number() > 0)
			chat_manager->openChatWidget(gadu, userlist->byID("Gadu", QString::number(ev->number())));
	}
	else
		QWidget::customEvent(e);
}

void Kadu::setStatus(const UserStatus &status)
{
	UserStatus notConst = status;
	userStatusChanger->userStatusSet(notConst);
}

void Kadu::setOnline(const QString &description)
{
	UserStatus status;

	status.setStatus(gadu->currentStatus());
	status.setOnline(description);

	userStatusChanger->userStatusSet(status);
}

void Kadu::setBusy(const QString &description)
{
	UserStatus status;

	status.setStatus(gadu->currentStatus());
	status.setBusy(description);

	userStatusChanger->userStatusSet(status);
}

void Kadu::setInvisible(const QString &description)
{
	UserStatus status;

	status.setStatus(gadu->currentStatus());
	status.setInvisible(description);

	userStatusChanger->userStatusSet(status);
}

void Kadu::setOffline(const QString &description)
{
	UserStatus status;

	status.setStatus(gadu->currentStatus());
	status.setOffline(description);

	userStatusChanger->userStatusSet(status);
}

void Kadu::setTalkWithMe(const QString &description)
{
	UserStatus status;

	status.setStatus(gadu->currentStatus());
	status.setTalkWithMe(description);

	userStatusChanger->userStatusSet(status);
}

void Kadu::setDoNotDisturb(const QString &description)
{
	UserStatus status;

	status.setStatus(gadu->currentStatus());
	status.setDoNotDisturb(description);

	userStatusChanger->userStatusSet(status);
}

void Kadu::import_0_5_0_configuration()
{
// 	config_file.removeVariable("General", "ShowAnonymousWithMsgs");

	int defaultStatusIndex = config_file.readNumEntry("General", "DefaultStatusIndex", -1);
	if (defaultStatusIndex != -1)
	{
		QString startupStatus;
		switch (defaultStatusIndex)
		{
			case 0:
			case 1: startupStatus = "Online";
			        break;
			case 2:
			case 3: startupStatus = "Busy";
			        break;
			case 4:
			case 5: startupStatus = "Invisible";
			        break;
			case 6: startupStatus = "Offline";
			        break;
			case 7:
			case 8: startupStatus = "LastStatus";
			        break;
		}
		config_file.writeEntry("General", "StartupStatus", startupStatus);
		config_file.addVariable("General", "StartupLastDescription", defaultStatusIndex == 7 || defaultStatusIndex == 8);
		config_file.removeVariable("General", "DefaultStatusIndex");
	}

	QString infoPanelSyntax = config_file.readEntry("Look", "PanelContents", "nothing");
	if (infoPanelSyntax != "nothing")
	{
		config_file.writeEntry("Look", "InfoPanelSyntaxFile", "custom");
		SyntaxList infoPanelList("infopanel");
		infoPanelList.updateSyntax("custom", infoPanelSyntax);
		config_file.removeVariable("Look", "PanelContents");
	}

	QString chatSyntax = config_file.readEntry("Look", "FullStyle", "nothing");
	if (chatSyntax != "nothing")
	{
		SyntaxList chatList("chat");
		chatSyntax = chatSyntax.replace("%1", "#{backgroundColor}");
		chatSyntax = chatSyntax.replace("%2", "#{fontColor}");
		chatSyntax = chatSyntax.replace("%3", "#{nickColor}");
		chatSyntax = chatSyntax.replace("%4", "%a");
		chatSyntax = chatSyntax.replace("%5", "#{receivedDate}");
		chatSyntax = chatSyntax.replace("%6", "#{sentDate}");
		chatSyntax = chatSyntax.replace("%7", "#{message}");
		chatList.updateSyntax("custom", chatSyntax);
		config_file.removeVariable("Look", "FullStyle");

		QString oldStyle = config_file.readEntry("Look", "Style");
		if (oldStyle != "kadu" && oldStyle != "hapi" && oldStyle != "irc")
			config_file.writeEntry("Look", "Style", "custom");
	}

	config_file.removeVariable("Look", "UserboxBackgroundMove");
	config_file.removeVariable("Look", "UserboxBackgroundSX");
	config_file.removeVariable("Look", "UserboxBackgroundSY");
	config_file.removeVariable("Look", "UserboxBackgroundSE");
	config_file.removeVariable("Look", "UserboxBackgroundSH");
}

void Kadu::createDefaultConfiguration()
{
	QWidget w;

	config_file.addVariable("Chat", "ActivateWithNewMessages", false);
	config_file.addVariable("Chat", "AutoSend", true);
	config_file.addVariable("Chat", "BlinkChatTitle", true);
	config_file.addVariable("Chat", "ChatCloseTimer", true);
	config_file.addVariable("Chat", "ChatCloseTimerPeriod", 2);
	config_file.addVariable("Chat", "ChatPrune", false);
	config_file.addVariable("Chat", "ChatPruneLen", 20);
	config_file.addVariable("Chat", "ConfirmChatClear", true);
	config_file.addVariable("Chat", "EmoticonsPaths", "");
	config_file.addVariable("Chat", "EmoticonsStyle", EMOTS_ANIMATED);
	config_file.addVariable("Chat", "EmoticonsTheme", "penguins");
	config_file.addVariable("Chat", "FoldLink", true);
	config_file.addVariable("Chat", "LinkFoldTreshold", 50);
	config_file.addVariable("Chat", "IgnoreAnonymousRichtext", true);
	config_file.addVariable("Chat", "IgnoreAnonymousUsers", false);
	config_file.addVariable("Chat", "IgnoreAnonymousUsersInConferences", false);
	config_file.addVariable("Chat", "LastImagePath", QString(getenv("HOME")) + '/');
	config_file.addVariable("Chat", "MaxImageRequests", 5);
	config_file.addVariable("Chat", "MaxImageSize", 255);
	config_file.addVariable("Chat", "MessageAcks", false);
	config_file.addVariable("Chat", "NewMessagesInChatTitle", false);
	config_file.addVariable("Chat", "OpenChatOnMessage", true);
	config_file.addVariable("Chat", "OpenChatOnMessageWhenOnline", false);
	config_file.addVariable("Chat", "SaveOpenedWindows", true);
	config_file.addVariable("Chat", "ReceiveMessages", true);
	config_file.addVariable("Chat", "ReceiveImagesDuringInvisibility", true);
	config_file.addVariable("Chat", "RememberPosition", true);
	config_file.addVariable("Chat", "ShowEditWindowLabel", true);

	config_file.addVariable("General", "AllowExecutingFromParser", false);
	config_file.addVariable("General", "AutoRaise", false);
	config_file.addVariable("General", "CheckUpdates", true);
	config_file.addVariable("General", "DEBUG_MASK", KDEBUG_ALL & ~KDEBUG_FUNCTION_END);
	config_file.addVariable("General", "DescriptionHeight", 60);
	config_file.addVariable("General", "DisconnectWithCurrentDescription", true);
	config_file.addVariable("General", "HideBaseModules", true);
	config_file.addVariable("General", "Language",  QString(QTextCodec::locale()).mid(0,2));
	config_file.addVariable("General", "Nick", tr("Me"));
	config_file.addVariable("General", "NumberOfDescriptions", 20);
	config_file.addVariable("General", "ParseStatus", false);
	config_file.addVariable("General", "PrivateStatus", false);
	config_file.addVariable("General", "SaveStdErr", false);
	config_file.addVariable("General", "ShowBlocked", true);
	config_file.addVariable("General", "ShowBlocking", true);
	config_file.addVariable("General", "ShowEmotPanel", true);
	config_file.addVariable("General", "ShowOffline", true);
	config_file.addVariable("General", "ShowOnlineAndDescription", false);
	config_file.addVariable("General", "ShowWithoutDescription", true);
	config_file.addVariable("General", "StartDelay", 0);
	config_file.addVariable("General", "StartupLastDescription", true);
	config_file.addVariable("General", "StartupStatus", "LastStatus");
	config_file.addVariable("General", "StartupStatusInvisibleWhenLastWasOffline", true);
	config_file.addVariable("General", "UserBoxHeight", 300);

	config_file.addVariable("Look", "AlignUserboxIconsTop", true);
	config_file.addVariable("Look", "ChatContents", "");
	config_file.addVariable("Look", "ChatFont", *defaultFont);
	config_file.addVariable("Look", "ChatBgColor", QColor("#ffffff"));
	config_file.addVariable("Look", "ChatMyBgColor", QColor("#E0E0E0"));
	config_file.addVariable("Look", "ChatMyFontColor", QColor("#000000"));
	config_file.addVariable("Look", "ChatMyNickColor", QColor("#000000"));
	config_file.addVariable("Look", "ChatTextBgColor", QColor("#ffffff"));
	config_file.addVariable("Look", "ChatUsrBgColor", QColor("#F0F0F0"));
	config_file.addVariable("Look", "ChatUsrFontColor", QColor("#000000"));
	config_file.addVariable("Look", "ChatUsrNickColor", QColor("#000000"));
	config_file.addVariable("Look", "ConferenceContents", "");
	config_file.addVariable("Look", "ConferencePrefix", "");
	config_file.addVariable("Look", "DescriptionColor", w.paletteForegroundColor());
	config_file.addVariable("Look", "DisplayGroupTabs", true);
	config_file.addVariable("Look", "HeaderSeparatorHeight", 1);
	config_file.addVariable("Look", "IconsPaths", "");
	config_file.addVariable("Look", "IconTheme", "default");
	config_file.addVariable("Look", "InfoPanelBgColor", w.paletteBackgroundColor());
	config_file.addVariable("Look", "InfoPanelFgColor", w.paletteForegroundColor());
	config_file.addVariable("Look", "InfoPanelSyntaxFile", "default");
	config_file.addVariable("Look", "NiceDateFormat", true);
	config_file.addVariable("Look", "NoHeaderInterval", 30);
	config_file.addVariable("Look", "NoHeaderRepeat", false);
	config_file.addVariable("Look", "NoServerTime", true);
	config_file.addVariable("Look", "NoServerTimeDiff", 60);
	config_file.addVariable("Look", "PanelFont", *defaultFont);
	config_file.addVariable("Look", "PanelVerticalScrollBar", false);
	config_file.addVariable("Look", "ParagraphSeparator", 4);
	config_file.addVariable("Look", "ShowBold", true);
	config_file.addVariable("Look", "ShowDesc", true);
	config_file.addVariable("Look", "ShowVerticalScrollBar", true);
	config_file.addVariable("Look", "ShowInfoPanel", true);
	config_file.addVariable("Look", "ShowMultilineDesc", true);
	config_file.addVariable("Look", "ShowStatusButton", true);
	config_file.addVariable("Look", "Style", "kadu");
	config_file.addVariable("Look", "UserboxBackgroundDisplayStyle", "Stretched");
	config_file.addVariable("Look", "UserboxBgColor", w.paletteBackgroundColor());
	config_file.addVariable("Look", "UserBoxColumnCount", 1);
	config_file.addVariable("Look", "UserboxFgColor", w.paletteForegroundColor());
	QFont userboxfont(*defaultFont);
	userboxfont.setPointSize(defaultFont->pointSize()+1);
	config_file.addVariable("Look", "UserboxFont", userboxfont);
	config_file.addVariable("Look", "UseUserboxBackground", false);
#ifdef Q_OS_MAC
	/* Dorr: for MacOS X define the icon notification to animated which
	 * will prevent from blinking the dock icon
	 */
	config_file.addVariable("Look", "NewMessageIcon", 2);
#endif

	config_file.addVariable("Network", "AllowDCC", true);
	config_file.addVariable("Network", "DefaultPort", 0);
	config_file.addVariable("Network", "isDefServers", true);
	config_file.addVariable("Network", "Server", "");
	config_file.addVariable("Network", "TimeoutInMs", 5000);
	config_file.addVariable("Network", "UseProxy", false);

#ifdef Q_OS_MAC
	/* Dorr: for MacOS X define the function keys with 'apple' button
	 * as it is the default system configuration */
	config_file.addVariable("ShortCuts", "chat_clear", "Ctrl+F9");
	config_file.addVariable("ShortCuts", "kadu_configure", "Ctrl+F2");
	config_file.addVariable("ShortCuts", "kadu_modulesmanager", "Ctrl+F4");
	config_file.addVariable("ShortCuts", "kadu_showoffline", "Ctrl+F9");
	config_file.addVariable("ShortCuts", "kadu_showonlydesc", "Ctrl+F10");
#else
	config_file.addVariable("ShortCuts", "chat_clear", "F9");
	config_file.addVariable("ShortCuts", "kadu_configure", "F2");
	config_file.addVariable("ShortCuts", "kadu_modulesmanager", "F4");
	config_file.addVariable("ShortCuts", "kadu_showoffline", "F9");
	config_file.addVariable("ShortCuts", "kadu_showonlydesc", "F10");
#endif
	config_file.addVariable("ShortCuts", "chat_bold", "Ctrl+B");
	config_file.addVariable("ShortCuts", "chat_close", "Esc");
	config_file.addVariable("ShortCuts", "chat_italic", "Ctrl+I");
	config_file.addVariable("ShortCuts", "chat_newline", "Return");
	config_file.addVariable("ShortCuts", "chat_underline", "Ctrl+U");
	config_file.addVariable("ShortCuts", "kadu_adduser", "Ctrl+N");
	config_file.addVariable("ShortCuts", "kadu_deleteuser", "Del");
	config_file.addVariable("ShortCuts", "kadu_openchatwith", "Ctrl+L");
	config_file.addVariable("ShortCuts", "kadu_persinfo", "Ins");
	config_file.addVariable("ShortCuts", "kadu_searchuser", "Ctrl+F");

	createAllDefaultToolbars();
}

void Kadu::createAllDefaultToolbars()
{
	// dont use getToolbarsConfigElement here, we have to be sure that this element don'e exists
	QDomElement toolbarsConfig = xml_config_file->findElement(xml_config_file->rootElement(), "Toolbars");

	if (!toolbarsConfig.isNull())
		return; // no need for defaults...

	toolbarsConfig = xml_config_file->createElement(xml_config_file->rootElement(), "Toolbars");

	Kadu::createDefaultToolbars(toolbarsConfig);
	ChatEditBox::createDefaultToolbars(toolbarsConfig);
	SearchDialog::createDefaultToolbars(toolbarsConfig);

	xml_config_file->sync();
}

void Kadu::createDefaultToolbars(QDomElement toolbarsConfig)
{
	QDomElement dockAreaConfig = getDockAreaConfigElement(toolbarsConfig, "topDockArea");
	QDomElement toolbarConfig = xml_config_file->createElement(dockAreaConfig, "ToolBar");

	addToolButton(toolbarConfig, "inactiveUsersAction");
	addToolButton(toolbarConfig, "descriptionUsersAction");
	addToolButton(toolbarConfig, "configurationAction");
	addToolButton(toolbarConfig, "editUserAction");
	addToolButton(toolbarConfig, "openSearchAction");
	addToolButton(toolbarConfig, "addUserAction");
}

void Kadu::addAction(const QString &actionName, bool showLabel)
{
	addToolButton(findExistingToolbar(""), actionName, showLabel);
	kadu->refreshToolBars("");
}
