/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QApplication>
#include <QClipboard>
#include <QMenuBar>
#include <QPushButton>
#include <QSplitter>
#include <QVBoxLayout>

#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>

#include "about.h"
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
#include "updates.h"
#include "userbox.h"
#include "userinfo.h"

#include "kadu.h"

static QTimer *blinktimer;
QMenu *dockMenu;

int lockFileHandle;
QFile *lockFile;
struct flock *lock_str;

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
	{
		if (!Userbox->selectedUsers().isEmpty())
			deleteUsers();
	}
	else if (HotKey::shortCut(e,"ShortCuts", "kadu_persinfo"))
	{
		if (Userbox->selectedUsers().count() == 1)
			showUserInfo();
	}
	else if (HotKey::shortCut(e,"ShortCuts", "kadu_searchuser"))
		lookupInDirectory();
//	else if (HotKey::shortCut(e,"ShortCuts", "kadu_showoffline"))
//		groups_manager->changeDisplayingOffline();
// 	else if (HotKey::shortCut(e,"ShortCuts", "kadu_showonlydesc"))
// 		groups_manager->changeDisplayingWithoutDescription();
	else if (HotKey::shortCut(e,"ShortCuts", "kadu_configure"))
		configure();
	else if (HotKey::shortCut(e,"ShortCuts", "kadu_modulesmanager"))
		modules_manager->showDialog();

	emit keyPressed(e);

	QWidget::keyPressEvent(e);
//	kdebugf2();
}

/* a monstrous constructor so Kadu would take longer to start up */
Kadu::Kadu(QWidget *parent)
	: KaduMainWindow(parent),
	InfoPanel(0), MenuBar(0), MainMenu(0), RecentChatsMenu(0), GroupBar(0),
	Userbox(0), statusMenu(0), statusButton(), lastPositionBeforeStatusMenuHide(),
	StartTime(QDateTime::currentDateTime()), updateInformationPanelTimer(), NextStatus(),
	selectedUsers(new UserGroup(userlist->count() / 2)), ShowMainWindowOnStart(true),
	DoBlink(false), BlinkOn(false),Docked(false), dontHideOnClose(false), personalInfoMenuId(-1)
{
	kdebugf();
	kadu = this;
	blinktimer = 0;

	Myself.addProtocol("Gadu", QString::number(config_file.readUnsignedNumEntry("General", "UIN", 0)));
	Myself.setAltNick(config_file.readEntry("General", "Nick"));

	createDefaultConfiguration();

	MainWidget = new QWidget;
	MainLayout = new QVBoxLayout(MainWidget);
	MainLayout->setMargin(0);
	MainLayout->setSpacing(0);

	QSplitter *split = new QSplitter(Qt::Vertical, this, "splitter");
	MainLayout->addWidget(split);

	QWidget* hbox = new QWidget(split);
	QHBoxLayout *hbox_layout = new QHBoxLayout;

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
	Userbox = new UserBox(true, userlist, this, "userbox");

	hbox_layout->setStretchFactor(Userbox, 100);
	hbox_layout->addWidget(GroupBar);
	hbox_layout->addWidget(Userbox);
	hbox_layout->setAlignment(GroupBar, Qt::AlignTop);
	hbox->setLayout(hbox_layout);
	connect(UserBox::userboxmenu, SIGNAL(popup()), this, SLOT(popupMenu()));
	connect(Userbox, SIGNAL(rightButtonPressed(Q3ListBoxItem *, const QPoint &)),
		UserBox::userboxmenu, SLOT(show(Q3ListBoxItem *)));
	connect(UserBox::management, SIGNAL(aboutToShow()), this, SLOT(popupMenu()));
	connect(Userbox, SIGNAL(doubleClicked(UserListElement)), this, SLOT(sendMessage(UserListElement)));
	connect(Userbox, SIGNAL(returnPressed(UserListElement)), this, SLOT(sendMessage(UserListElement)));
	connect(Userbox, SIGNAL(mouseButtonClicked(int, Q3ListBoxItem *, const QPoint &)),
		this, SLOT(mouseButtonClicked(int, Q3ListBoxItem *)));
	connect(Userbox, SIGNAL(currentChanged(UserListElement)), this, SLOT(currentChanged(UserListElement)));

	UserBox::userboxmenu->addItem("OpenChat", tr("Open chat window") ,this, SLOT(openChat()));
	UserBox::userboxmenu->insertSeparator();
	UserBox::userboxmenu->addItem("WriteEmail", tr("Write email message"), this, SLOT(writeMail()));
	UserBox::userboxmenu->addItem("CopyDescription", tr("Copy description"), this, SLOT(copyDescription()));
	UserBox::userboxmenu->addItem("OpenDescriptionLink", tr("Open description link in browser"), this, SLOT(openDescriptionLink()));
	UserBox::userboxmenu->addItem("CopyPersonalInfo", tr("Copy personal info"), this, SLOT(copyPersonalInfo()));
	UserBox::userboxmenu->addItem("LookupUserInfo", tr("Search in directory"), this, SLOT(lookupInDirectory()), HotKey::shortCutFromFile("ShortCuts", "kadu_searchuser"));
	UserBox::userboxmenu->addItem("EditUserInfo", tr("Contact data"), this, SLOT(showUserInfo()), HotKey::shortCutFromFile("ShortCuts", "kadu_persinfo"));
	UserBox::userboxmenu->insertSeparator();

	UserBox::management->addItem("Ignore", tr("Ignore"), this, SLOT(ignoreUser()));
	UserBox::management->addItem("Blocking", tr("Block"), this, SLOT(blockUser()));
	UserBox::management->addItem("NotifyAboutUser", tr("Notify about user"), this, SLOT(notifyUser()));
	UserBox::management->addItem("Offline", tr("Offline to user"), this, SLOT(offlineToUser()));
	UserBox::management->addItem("HideDescription", tr("Hide description"), this, SLOT(hideDescription()));
	UserBox::management->insertSeparator();
	UserBox::management->addItem("RemoveFromUserlist", tr("Delete"), this, SLOT(deleteUsers()),HotKey::shortCutFromFile("ShortCuts", "kadu_deleteuser"));

	UserBox::userboxmenu->insertItem(tr("User management"), UserBox::management);

	groups_manager->setTabBar(GroupBar);
	setDocked(Docked, dontHideOnClose);

	loadGeometry(this, "General", "Geometry", 0, 30, 205, 465);
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

	descriptionUsersAction = new ActionDescription(
		ActionDescription::TypeUserList, "descriptionUsersAction",
		this, SLOT(descriptionUsersActionActivated(QAction *, bool)),
		"ShowOnlyDescriptionUsers", tr("Hide users without description"),
		true, tr("Show users without description")
	);

	onlineAndDescriptionUsersAction = new ActionDescription(
		ActionDescription::TypeUserList, "onlineAndDescriptionUsersAction",
		this, SLOT(onlineAndDescUsersActionActivated(QAction *, bool)),
		"ShowOnlineAndDescriptionUsers", tr("Show only online and description users"),
		true, tr("Show all users")
	);

	configurationActionDescription = new ActionDescription(
		ActionDescription::TypeGlobal, "configurationAction",
		this, SLOT(configurationActionActivated(QAction *, bool)),
		"Configuration", tr("Configuration")
	);

	editUserActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "editUserAction",
		this, SLOT(editUserActionActivated(QAction *, bool)),
		"EditUserInfo", tr("Contact data")
	);

	addUserActionDescription = new ActionDescription(
		ActionDescription::TypeGlobal, "addUserAction",
		this, SLOT(addUserActionActivated(QAction *, bool)),
		"AddUser", tr("Add user")
	);

	openSearchActionDescription = new ActionDescription(
		ActionDescription::TypeGlobal, "openSearchAction",
		this, SLOT(searchInDirectoryActionActivated(QAction *, bool)),
		"LookupUserInfo", tr("Search user in directory")
	);

/*	TODO: port this.
	Action* open_status_action = new Action("Offline", tr("Change status"), "openStatusAction", Action::TypeGlobal);
	connect(open_status_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(showStatusActionActivated()));
	connect(open_status_action, SIGNAL(addedToToolbar(ToolButton*, ToolBar*)),
		this, SLOT(showStatusActionAddedToToolbar(ToolButton*)));*/

	ToolBar::addDefaultAction("Kadu toolbar", "inactiveUsersAction");
	ToolBar::addDefaultAction("Kadu toolbar", "descriptionUsersAction");
	ToolBar::addDefaultAction("Kadu toolbar", "configurationAction");
	ToolBar::addDefaultAction("Kadu toolbar", "editUserAction");
	ToolBar::addDefaultAction("Kadu toolbar", "openSearchAction");
	ToolBar::addDefaultAction("Kadu toolbar", "addUserAction");

	/* guess what */
	createMenu();
	createStatusPopupMenu();
	loadToolBarsFromConfig("");

	connect(statusMenu, SIGNAL(aboutToHide()), this, SLOT(statusMenuAboutToHide()));
	connect(dockMenu, SIGNAL(aboutToHide()), this, SLOT(dockMenuAboutToHide()));
	connect(RecentChatsMenu, SIGNAL(aboutToShow()), this, SLOT(createRecentChatsMenu()));

	dockMenu->insertSeparator();
	dockMenu->addAction(icons_manager->loadIcon("Exit"), tr("&Exit Kadu"), this, SLOT(quit()));
// 	icons_manager->registerMenuItem(dockMenu, tr("&Exit Kadu"), "Exit");

	InfoPanel = new KaduTextBrowser(split);
	InfoPanel->setFrameStyle(QFrame::NoFrame);
	InfoPanel->setMinimumHeight(int(1.5 * QFontMetrics(InfoPanel->QTextEdit::font()).height()));
//	InfoPanel->resize(InfoPanel->size().width(), int(1.5 * QFontMetrics(InfoPanel->font()).height()));
	InfoPanel->setTextFormat(Qt::RichText);
	InfoPanel->setAlignment(Qt::AlignVCenter/** | Qt::WordBreak | Qt::DontClip*/);
	if (!config_file.readBoolEntry("Look", "PanelVerticalScrollbar"))
		InfoPanel->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	InfoPanel->setPaletteBackgroundColor(config_file.readColorEntry("Look", "InfoPanelBgColor"));
	InfoPanel->setPaletteForegroundColor(config_file.readColorEntry("Look", "InfoPanelFgColor"));
	InfoPanel->QTextEdit::setFont(config_file.readFontEntry("Look", "PanelFont"));
// 	if((EmoticonsStyle)config_file.readNumEntry("Chat", "EmoticonsStyle") == EMOTS_ANIMATED)
// 		InfoPanel->setStyleSheet(new AnimStyleSheet(InfoPanel, emoticons->themePath()));
// 	else
// 		InfoPanel->setStyleSheet(new StaticStyleSheet(InfoPanel, emoticons->themePath()));

	if (!config_file.readBoolEntry("Look", "ShowInfoPanel"))
		InfoPanel->QWidget::hide();
	connect(&updateInformationPanelTimer, SIGNAL(timeout()), this, SLOT(updateInformationPanel()));

	statusButton = new QPushButton(icons_manager->loadIcon("Offline"), tr("Offline"), this, "statusButton");
	MainLayout->addWidget (statusButton);
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
	connect(gadu, SIGNAL(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)),
		this, SLOT(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)));
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

	MainLayout->setResizeMode(QLayout::Minimum);
	MainWidget->setLayout(MainLayout);
	setCentralWidget(MainWidget);

	if (config_file.readBoolEntry("Chat", "SaveOpenedWindows", true))
		chat_manager->loadOpenedWindows();

	configurationUpdated();

	kdebugf2();
}

QVBoxLayout * Kadu::mainLayout() const
{
	return MainLayout;
}

void Kadu::popupMenu()
{
	kdebugf();

	UserBox *activeUserBox = UserBox::activeUserBox();
	if (activeUserBox == NULL) //to si� zdarza...
	{
		kdebugf2();
		return;
	}

	UserListElements users = activeUserBox->selectedUsers();
	if (users.count() == 0)
		return;

	UserListElement firstUser = *users.constBegin();

	bool containsMe = false;
	bool containsUserWithoutID = false;
	CONST_FOREACH(user, users)
	{
		if (!containsUserWithoutID && !(*user).usesProtocol("Gadu"))
			containsUserWithoutID = true;
		if (!containsMe && (*user).usesProtocol("Gadu") && (*user).ID("Gadu") == Myself.ID("Gadu"))
			containsMe = true;
	}

	int ignoreuseritem = UserBox::management->getItem(tr("Ignore"));
	int blockuseritem = UserBox::management->getItem(tr("Block"));
	int notifyuseritem = UserBox::management->getItem(tr("Notify about user"));
	int offlinetouseritem = UserBox::management->getItem(tr("Offline to user"));
	int hidedescriptionitem = UserBox::management->getItem(tr("Hide description"));
	int chatitem = UserBox::userboxmenu->getItem(tr("Open chat window"));

	if (containsUserWithoutID)
	{
		UserBox::management->setItemVisible(ignoreuseritem, false);
		UserBox::management->setItemVisible(blockuseritem, false);
		UserBox::management->setItemVisible(notifyuseritem, false);
		UserBox::management->setItemVisible(offlinetouseritem, false);
		UserBox::management->setItemVisible(hidedescriptionitem, false);
		UserBox::userboxmenu->setItemVisible(chatitem, false);
	}
	else
	{
		bool on;
		UserListElements selectedUsers = activeUserBox->selectedUsers();
		UserBox::management->setItemChecked(ignoreuseritem, IgnoredManager::isIgnored(selectedUsers));

		on = true;
		CONST_FOREACH(user, users)
			if (!(*user).usesProtocol("Gadu") || !(*user).protocolData("Gadu", "Blocking").toBool())
			{
				on = false;
				break;
			}
		UserBox::management->setItemChecked(blockuseritem, on);

		on = true;
		CONST_FOREACH(user, users)
			if (!(*user).usesProtocol("Gadu") || !(*user).protocolData("Gadu", "OfflineTo").toBool())
			{
				on = false;
				break;
			}
		UserBox::management->setItemVisible(offlinetouseritem, config_file.readBoolEntry("General", "PrivateStatus"));
		UserBox::management->setItemChecked(offlinetouseritem, on);

		on = false;
		CONST_FOREACH(user, users)
			if ((*user).data("HideDescription").toString() == "true")
			{
				on = true;
				break;
			}
		UserBox::management->setItemChecked(hidedescriptionitem, on);

		on = true;
		CONST_FOREACH(user, users)
			if (!(*user).notify())
			{
				on = false;
				break;
			}
		UserBox::management->setItemVisible(notifyuseritem, !config_file.readBoolEntry("Notify", "NotifyAboutAll"));
		UserBox::management->setItemChecked(notifyuseritem, on);

		if (containsMe)
		{
			UserBox::management->setItemVisible(ignoreuseritem, false);
			UserBox::management->setItemVisible(blockuseritem, false);
			UserBox::management->setItemVisible(offlinetouseritem, false);
			UserBox::userboxmenu->setItemVisible(chatitem, false);
		}
	}

	if (users.count() != 1)
		UserBox::userboxmenu->setItemVisible(UserBox::userboxmenu->getItem(tr("Contact data")), false);
	if ((users.count() != 1) || !firstUser.usesProtocol("Gadu"))
		UserBox::userboxmenu->setItemVisible(UserBox::userboxmenu->getItem(tr("Search in directory")), false);
	if ((users.count() != 1) || !firstUser.usesProtocol("Gadu") || firstUser.status("Gadu").description().isEmpty())
		UserBox::userboxmenu->setItemVisible(UserBox::userboxmenu->getItem(tr("Copy description")), false);
	if ((users.count() != 1) || !firstUser.usesProtocol("Gadu") || firstUser.status("Gadu").description().isEmpty() ||
		firstUser.status("Gadu").description().find(HtmlDocument::urlRegExp()) == -1)
		UserBox::userboxmenu->setItemVisible(UserBox::userboxmenu->getItem(tr("Open description link in browser")), false);
	if ((users.count() != 1) || firstUser.email().isEmpty() || firstUser.email().find(HtmlDocument::mailRegExp()) == -1)
		UserBox::userboxmenu->setItemVisible(UserBox::userboxmenu->getItem(tr("Write email message")), false);
	kdebugf2();
}

void Kadu::configure()
{
	configurationActionActivated(0, false);
}

void Kadu::copyDescription()
{
	kdebugf();
	UserBox *activeUserBox = UserBox::activeUserBox();
	if (activeUserBox == NULL)
	{
		kdebugf2();
		return;
	}

	UserListElement user = activeUserBox->selectedUsers().first();
	QString status;
	if (user.usesProtocol("Gadu"))
		status = user.status("Gadu").description();
	if (!status.isEmpty())
	{
		QClipboard *clipboard = QApplication::clipboard();

		clipboard->setText(status, QClipboard::Clipboard);
		clipboard->setText(status, QClipboard::Selection);
	}
	kdebugf2();
}

void Kadu::openDescriptionLink()
{
	kdebugf();
	UserBox *activeUserBox = UserBox::activeUserBox();
	if (activeUserBox == NULL)
	{
		kdebugf2();
		return;
	}

	UserListElement user = activeUserBox->selectedUsers().first();

	if (user.usesProtocol("Gadu"))
	{
		QString status = user.status("Gadu").description();
		if (!status.isEmpty())
		{
			QRegExp url = HtmlDocument::urlRegExp();
			int idx_start = url.search(status);
			if (idx_start >= 0)
				openWebBrowser(status.mid(idx_start, url.matchedLength()));
		}
	}

	kdebugf2();
}

void Kadu::writeMail()
{
	kdebugf();
	UserBox *activeUserBox = UserBox::activeUserBox();
	if (!activeUserBox)
	{
		kdebugf2();
		return;
	}

	UserListElement user = activeUserBox->selectedUsers().first();
	if (!user.email().isEmpty())
		openMailClient(user.email());
	kdebugf2();
}

void Kadu::copyPersonalInfo()
{
	kdebugf();
	UserBox *activeUserBox = UserBox::activeUserBox();
	if (activeUserBox == NULL)
	{
		kdebugf2();
		return;
	}

	UserListElements users = activeUserBox->selectedUsers();
	QStringList infoList;
	QString copyPersonalDataSyntax = config_file.readEntry("General", "CopyPersonalDataSyntax", tr("Contact: %a[ (%u)]\n[First name: %f\n][Last name: %r\n][Mobile: %m\n]"));
	CONST_FOREACH(user, users)
		infoList.append(KaduParser::parse(copyPersonalDataSyntax, *user, false));

	QString info = infoList.join("\n");
	if (!info.isEmpty())
	{
		QClipboard *clipboard = QApplication::clipboard();

		clipboard->setText(info, QClipboard::Clipboard);
		clipboard->setText(info, QClipboard::Selection);
	}
	kdebugf2();
}

void Kadu::openRecentChats(int index)
{
	kdebugf();

	chat_manager->openPendingMsgs(chat_manager->closedChatUsers().at(index));

	kdebugf2();
}

void Kadu::lookupInDirectory()
{
	kdebugf();

	UserBox *activeUserBox = UserBox::activeUserBox();
	if (activeUserBox == NULL)
	{
		kdebugf2();
		return;
	}

	UserListElements users = activeUserBox->selectedUsers();
	if (users.count() == 1)
	{
		UserListElement user = *(users.constBegin());
		if (user.usesProtocol("Gadu"))
		{
			SearchDialog *sd = new SearchDialog(kadu, "user_info", user.ID("Gadu").toUInt());
			sd->show();
			sd->firstSearch();
		}
	}
	else
		searchInDirectoryActionActivated(0, false);

	kdebugf2();
}

void Kadu::selectedUsersNeeded(const UserGroup *&users)
{
	kdebugf();

	UserBox *activeUserBox = UserBox::activeUserBox();
	if (activeUserBox == NULL)
	{
		users = NULL;
		kdebugf2();
		return;
	}

	selectedUsers->clear();
	selectedUsers->addUsers(activeUserBox->selectedUsers());
	users = selectedUsers;

	kdebugf2();
}

void Kadu::inactiveUsersActionActivated(QAction *sender, bool toggled)
{
	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	groups_manager->changeDisplayingOffline(window->getUserBox(), !toggled);
}

void Kadu::descriptionUsersActionActivated(QAction *sender, bool toggled)
{
	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	groups_manager->changeDisplayingWithoutDescription(window->getUserBox(), !toggled);
}

void Kadu::onlineAndDescUsersActionActivated(QAction *sender, bool toggled)
{
	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	groups_manager->changeDisplayingOnlineAndDescription(window->getUserBox(), !toggled);
}

void Kadu::configurationActionActivated(QAction *sender, bool toggled)
{
	MainConfigurationWindow::instance()->show();
}

void Kadu::editUserActionSetParams(QString /*protocolName*/, UserListElement users)
{
	kdebugf();
/*
	Action *action = KaduActions["editUserAction"];
	UserListElements elems = UserListElements(users);
	if (users.isAnonymous())
	{
		action->setIcons(elems, icons_manager->loadIcon("AddUser"));
		action->setTexts(elems, tr("Add user"));
	}
	else
	{
		action->setIcons(elems, icons_manager->loadIcon("EditUserInfo"));
		action->setTexts(elems, tr("Contact data"));
	}*/

	kdebugf2();
}

void Kadu::editUserActionAddedToToolbar(const UserGroup *users)
{
	kdebugf();
	if ((users->count()) == 1 && (*users->begin()).isAnonymous())
		editUserActionSetParams("", *users->begin());
	kdebugf2();
}

void Kadu::editUserActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;
	
	UserListElements selectedUsers = window->getUserListElements();
	
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
		UserListElements selectedUsers = window->getUserListElements();
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

void Kadu::showUserInfo()
{
	kdebugf();
	KaduActions.getAction("editUserAction", this)->activate(QAction::Trigger);
	kdebugf2();
}

void Kadu::deleteUsers()
{
	kdebugf();
	UserBox *activeUserBox=UserBox::activeUserBox();
	if (activeUserBox==NULL)
	{
		kdebugf2();
		return;
	}
	removeUsers(activeUserBox->selectedUsers());
	if (!Userbox->isSelected(Userbox->currentItem()))
		InfoPanel->clear();
	kdebugf2();
}

void Kadu::personalInfo()
{
	(new PersonalInfoDialog(kadu, "personal_info"))->show();
}

void Kadu::addUserAction()
{
	const UserGroup* users;
	selectedUsersNeeded(users);
// 	addUserActionActivated(users);
}

void Kadu::manageIgnored()
{
	(new Ignored(kadu, "ignored"))->show();
}

void Kadu::openChat()
{
	kdebugf();

	UserBox *activeUserBox = UserBox::activeUserBox();
	if (activeUserBox == NULL)
	{
		kdebugf2();
		return;
	}
	sendMessage(activeUserBox->selectedUsers()[0]);

	kdebugf2();
}

//void Kadu::showStatusActionActivated()
//{
//}
//
//void Kadu::showStatusActionAddedToToolbar(ToolButton* button)
//{
//	button->setPixmap(gadu->currentStatus().pixmap());
//}

void Kadu::searchInDirectoryActionActivated(QAction *sender, bool toggled)
{
	(new SearchDialog(kadu, "search_user"))->show();
}

void Kadu::help()
{
	if (config_file.readEntry("General", "Language", QString(QTextCodec::locale()).mid(0,2)) == "pl")
		openWebBrowser("http://www.kadu.net/w/Kadu:Pomoc");
	else
		openWebBrowser("http://www.kadu.net/w/English:Kadu:Help");
}

void Kadu::about()
{
	(new About(kadu))->show();
}

void Kadu::quit()
{
	kdebugf();

	if (measureTime)
	{
		struct timeval tv;
		struct timezone tz;
		gettimeofday(&tv, &tz);
		endingTime = (tv.tv_sec % 1000) * 1000000 + tv.tv_usec;
	}
	close(true);
}

void Kadu::importExportUserlist()
{
	(new UserlistImportExport(kadu))->show();
}

void Kadu::hideKadu()
{
	if (Docked)
		if (dontHideOnClose)
			showMinimized();
		else
			close();
}

void Kadu::ignoreUser()
{
	kdebugf();
	UserBox *activeUserBox = UserBox::activeUserBox();
	if (activeUserBox == NULL)
	{
		kdebugf2();
		return;
	}
	UserListElements users = activeUserBox->selectedUsers();
	if (IgnoredManager::isIgnored(users))
		IgnoredManager::remove(users);
	else
		IgnoredManager::insert(users);
	IgnoredManager::writeToConfiguration();
	kdebugf2();
}

void Kadu::blockUser()
{
	kdebugf();
	UserBox *activeUserBox = UserBox::activeUserBox();
	if (activeUserBox == NULL)
	{
		kdebugf2();
		return;
	}

	UserListElements users = activeUserBox->selectedUsers();
	bool on = true;
	CONST_FOREACH(user, users)
		if (!(*user).usesProtocol("Gadu") || !(*user).protocolData("Gadu", "Blocking").toBool())
		{
			on = false;
			break;
		}

 	FOREACH(user, users)
		if ((*user).usesProtocol("Gadu") && (*user).protocolData("Gadu", "Blocking").toBool() != !on)
			(*user).setProtocolData("Gadu", "Blocking", !on);
	userlist->writeToConfig();
	kdebugf2();
}

void Kadu::notifyUser()
{
	kdebugf();
	UserBox *activeUserBox = UserBox::activeUserBox();
	if (activeUserBox == NULL)
	{
		kdebugf2();
		return;
	}

	UserListElements users = activeUserBox->selectedUsers();
	bool on = true;
	CONST_FOREACH(user, users)
		if (!(*user).notify())
		{
			on = false;
			break;
		}

	FOREACH(user, users)
		if ((*user).notify() != !on)
			(*user).setNotify(!on);

	userlist->writeToConfig();
	kdebugf2();
}

void Kadu::offlineToUser()
{
	kdebugf();
	UserBox *activeUserBox = UserBox::activeUserBox();
	if (activeUserBox == NULL)
	{
		kdebugf2();
		return;
	}

	UserListElements users = activeUserBox->selectedUsers();
	bool on = true;
	CONST_FOREACH(user, users)
		if (!(*user).usesProtocol("Gadu") || !(*user).protocolData("Gadu", "OfflineTo").toBool())
		{
			on = false;
			break;
		}

	FOREACH(user, users)
		if ((*user).usesProtocol("Gadu") && (*user).protocolData("Gadu", "OfflineTo").toBool() != !on)
			(*user).setProtocolData("Gadu", "OfflineTo", !on);

	userlist->writeToConfig();
	kdebugf2();
}

void Kadu::hideDescription()
{
	kdebugf();
	UserBox *activeUserBox = UserBox::activeUserBox();
	if (activeUserBox == NULL)
	{
		kdebugf2();
		return;
	}

	UserListElements users = activeUserBox->selectedUsers();
	bool on = true;
	CONST_FOREACH(user, users)
		if ((*user).data("HideDescription").toString() == "true")
		{
			on = false;
			break;
		}

	FOREACH(user, users)
		(*user).setData("HideDescription", on ? "true" : "false");

	userlist->writeToConfig();
	kdebugf2();
}

void Kadu::changeAppearance()
{
	kdebugf();

	QApplication::setStyle(config_file.readEntry("Look", "QtStyle"));

	GroupBar->setFont(QFont(config_file.readFontEntry("Look", "UserboxFont").family(), config_file.readFontEntry("Look", "UserboxFont").pointSize(),75));
	GroupBar->setShape(QTabBar::RoundedWest);

	if (config_file.readBoolEntry("Look", "ShowInfoPanel"))
	{
		InfoPanel->show();
		InfoPanel->setPaletteBackgroundColor(config_file.readColorEntry("Look", "InfoPanelBgColor"));
		InfoPanel->setPaletteForegroundColor(config_file.readColorEntry("Look", "InfoPanelFgColor"));
		InfoPanel->QTextEdit::setFont(config_file.readFontEntry("Look", "PanelFont"));

		if (config_file.readBoolEntry("Look", "PanelVerticalScrollbar"))
			InfoPanel->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		else
			InfoPanel->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}
	else
		dynamic_cast<QWidget *>(InfoPanel)->hide();

	kadu->statusButton->setShown(config_file.readBoolEntry("Look", "ShowStatusButton"));

	const UserStatus &stat = gadu->currentStatus();

	QPixmap pix = stat.pixmap();
	QIcon icon(pix);
	statusButton->setIcon(icon);
//	KaduActions["openStatusAction"]->setPixmaps(pix);
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
		InfoPanel->clear();
	kdebugf2();
}

/* if something's pending, open it, if not, open new message */
void Kadu::sendMessage(UserListElement elem)
{
	kdebugf();

	UserBox *activeUserBox = UserBox::activeUserBox();
	if (activeUserBox == NULL)
	{
		kdebugf2();
		return;
	}

	UserListElements users = activeUserBox->selectedUsers();
	if (!users.isEmpty())
	{
		if (elem.usesProtocol("Gadu") && !users.contains("Gadu", Myself.ID("Gadu"))) //TODO: elem.hasFeature("SendingMessages")
			chat_manager->sendMessage(elem, users);
		else if (elem.mobile().isEmpty() && !elem.email().isEmpty())
			openMailClient(users.first().email());
	}

	kdebugf2();
}

void Kadu::goOnline()
{
	slotHandleState(0);
}

void Kadu::goOnlineDesc()
{
	slotHandleState(1);
}

void Kadu::goBusy()
{
	slotHandleState(2);
}

void Kadu::goBusyDesc()
{
	slotHandleState(3);
}

void Kadu::goInvisible()
{
	slotHandleState(4);
}

void Kadu::goInvisibleDesc()
{
	slotHandleState(5);
}

void Kadu::goOffline()
{
	slotHandleState(6);
}

void Kadu::goOfflineDesc()
{
	slotHandleState(7);
}

void Kadu::changePrivateStatusSlot(bool toggled)
{
	UserStatus status;
	status.setStatus(userStatusChanger->status());
	status.setFriendsOnly(toggled);
	setStatus(status);

	config_file.writeEntry("General", "PrivateStatus", toggled);
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
	}

	kdebugf2();
}

void Kadu::changeStatus(UserStatus newStatus)
{
	if (gadu->nextStatus() == newStatus)
		return;

	NextStatus.setStatus(newStatus);
	gadu->writeableStatus().setStatus(NextStatus);

	if (NextStatus.isOffline())
	{
		statusMenu->setItemEnabled(7, false);
		dockMenu->setItemEnabled(7, false);
	}
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

void Kadu::imageReceivedAndSaved(UinType sender, uint32_t size, uint32_t crc32, const QString &/*path*/)
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
	if (!quit && Docked && !dontHideOnClose)
	{
		kdebugmf(KDEBUG_INFO, "hiding\n");
		hide();
		return false;
	}
	else
	{
		Closing = true;

		writeToolBarsToConfig("");

		if (config_file.readBoolEntry("Look", "ShowInfoPanel"))
		{
			config_file.writeEntry("General", "UserBoxHeight", Userbox->size().height());
			config_file.writeEntry("General", "DescriptionHeight", InfoPanel->size().height());
		}
		if (config_file.readBoolEntry("Look", "ShowStatusButton"))
			config_file.writeEntry("General", "UserBoxHeight", Userbox->size().height());
// 		saveGeometry(this, "General", "Geometry");

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
		disconnect(gadu, SIGNAL(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)),
				this, SLOT(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)));
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

		disconnect(UserBox::userboxmenu, SIGNAL(popup()), this, SLOT(popupMenu()));
		disconnect(Userbox, SIGNAL(rightButtonPressed(Q3ListBoxItem *, const QPoint &)),
					UserBox::userboxmenu, SLOT(show(Q3ListBoxItem *)));
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
		setMainWindowIcon(QPixmap(dataPath("kadu.png")));
#endif

		QWidget::close(true);

		lock_str->l_type = F_UNLCK;
		fcntl(lockFileHandle, F_SETLK, lock_str);
//		flock(lockFileHandle, LOCK_UN);
		lockFile->close();
		delete lockFile;
		lockFile=NULL;
		kdebugmf(KDEBUG_INFO, "Graceful shutdown...\n");

		delete xml_config_file;
		delete config_file_ptr;

		return true;
	}
}

// void Kadu::quitApplication()
// {
// 	kdebugf();
// 	close(true);
// }

Kadu::~Kadu(void)
{
	kdebugf();

#if DEBUG_ENABLED
	// for valgrind
	QStringList mainActions;
	mainActions << "inactiveUsersAction" << "descriptionUsersAction"
				<< "configurationAction" << "editUserAction"
				<< "addUserAction" << "openSearchAction";

	CONST_FOREACH(act, mainActions)
	{
		ActionDescription *a = KaduActions[*act];
		delete a;
	}
#endif
	delete selectedUsers;

	kdebugf2();
}

void Kadu::createRecentChatsMenu()
{
	kdebugf();

	RecentChatsMenu->clear();

	if (chat_manager->closedChatUsers().isEmpty())
	{
		RecentChatsMenu->addAction(tr("No closed chats found")/*, 0*/);
		RecentChatsMenu->setItemEnabled(0, false);

		kdebugf2();
		return;
	}

	unsigned int index = 0; // indeks pozycji w popupie

	CONST_FOREACH(users, chat_manager->closedChatUsers())
	{
		QStringList altnicks = (*users).altNicks(); // lista nick�w z okna rozmowy
		QString chat_users;

		if (altnicks.count() <= 5)
			chat_users = altnicks.join(", ");
		else // je�eli jest wi�cej ni� pi�ciu u�ytkownik�w...
		{
			for (int i = 0; i < 4; i++) // to i tak dodajemy tylko pierwszych pi�ciu :)
				chat_users.append(*altnicks.at(i) + ", ");
			chat_users.append(*altnicks.at(4) + " [...]");
		}

		RecentChatsMenu->addAction(icons_manager->loadIcon("OpenChat"), chat_users, this, SLOT(openRecentChats(int))/*, 0, index*/);

		index++;
	}

	kdebugf2();
}

void Kadu::createMenu()
{
	kdebugf();
	MainMenu = new QMenu;
	MainMenu->setTitle(tr("Menu"));
	RecentChatsMenu = new QMenu;
	RecentChatsMenu->setIcon(icons_manager->loadIcon("OpenChat"));
	RecentChatsMenu->setTitle(tr("Recent chats..."));

	MainMenu->addAction(icons_manager->loadIcon("Ignore"), tr("Manage &ignored"), this, SLOT(manageIgnored()));
	MainMenu->addAction(icons_manager->loadIcon("Configuration"), tr("&Configuration"), this, SLOT(configure()), HotKey::shortCutFromFile("ShortCuts", "kadu_configure"));
	MainMenu->insertSeparator();

	QAction *personalInfoMenu = MainMenu->addAction(icons_manager->loadIcon("PersonalInfo"), tr("Personal information"), this, SLOT(personalInfo()));
	MainMenu->insertSeparator();
	MainMenu->addMenu(RecentChatsMenu);
	MainMenu->addAction(icons_manager->loadIcon("LookupUserInfo"), tr("&Search user in directory"), this, SLOT(lookupInDirectory()));
	MainMenu->addAction(icons_manager->loadIcon("ImportExport"), tr("I&mport / Export userlist"), this, SLOT(importExportUserlist()));
	MainMenu->addAction(icons_manager->loadIcon("AddUser"), tr("&Add user"), this, SLOT(addUserAction()), HotKey::shortCutFromFile("ShortCuts", "kadu_adduser"));
	MainMenu->addAction(icons_manager->loadIcon("OpenChat"), tr("&Open chat with..."), this, SLOT(openChatWith()), HotKey::shortCutFromFile("ShortCuts", "kadu_openchatwith"));
	MainMenu->insertSeparator();
	MainMenu->addAction(icons_manager->loadIcon("HelpMenuItem"), tr("H&elp"), this, SLOT(help()));
	MainMenu->addAction(icons_manager->loadIcon("AboutMenuItem"), tr("A&bout..."), this, SLOT(about()));
	MainMenu->insertSeparator();
	MainMenu->addAction(icons_manager->loadIcon("HideKadu"), tr("&Hide Kadu"), this, SLOT(hideKadu()));
	MainMenu->addAction(icons_manager->loadIcon("Exit"), tr("&Exit Kadu"), this, SLOT(quit()));
	menuBar()->addMenu(MainMenu);

// 	icons_manager->registerMenu(MainMenu);
// 	icons_manager->registerMenuItem(MainMenu, tr("Manage &ignored"), "Ignore");
// 	icons_manager->registerMenuItem(MainMenu, tr("&Configuration"), "Configuration");
// 	icons_manager->registerMenuItem(MainMenu, tr("Personal information"), "PersonalInfo");
// 	icons_manager->registerMenuItem(MainMenu, tr("Recent chats..."), "OpenChat");
// 	icons_manager->registerMenuItem(MainMenu, tr("&Search user in directory"), "LookupUserInfo");
// 	icons_manager->registerMenuItem(MainMenu, tr("I&mport / Export userlist"), "ImportExport");
// 	icons_manager->registerMenuItem(MainMenu, tr("&Add user"), "AddUser");
// 	icons_manager->registerMenuItem(MainMenu, tr("&Open chat with..."), "OpenChat");
// 	icons_manager->registerMenuItem(MainMenu, tr("H&elp"), "HelpMenuItem");
// 	icons_manager->registerMenuItem(MainMenu, tr("A&bout..."), "AboutMenuItem");
// 	icons_manager->registerMenuItem(MainMenu, tr("&Hide Kadu"), "HideKadu");
// 	icons_manager->registerMenuItem(MainMenu, tr("&Exit Kadu"), "Exit");

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

	GaduStatus s;

	changeStatusToOnline = new QAction(icons_manager->loadIcon(s.pixmapName(Online, false, false)), tr("Online"), this);
	changeStatusToOnline->setCheckable(true);
	connect(changeStatusToOnline, SIGNAL(triggered()), this, SLOT(goOnline()));

	changeStatusToOnlineDesc = new QAction(icons_manager->loadIcon(s.pixmapName(Online, true, false)), tr("Online (d.)"), this);
	changeStatusToOnlineDesc->setCheckable(true);
	connect(changeStatusToOnlineDesc, SIGNAL(triggered()), this, SLOT(goOnlineDesc()));

	changeStatusToBusy = new QAction(icons_manager->loadIcon(s.pixmapName(Busy, false, false)), tr("Busy"), this);
	changeStatusToBusy->setCheckable(true);
	connect(changeStatusToBusy, SIGNAL(triggered()), this, SLOT(goBusy()));

	changeStatusToBusyDesc = new QAction(icons_manager->loadIcon(s.pixmapName(Busy, true, false)), tr("Busy (d.)"), this);
	changeStatusToBusyDesc->setCheckable(true);
	connect(changeStatusToBusyDesc, SIGNAL(triggered()), this, SLOT(goBusyDesc()));

	changeStatusToInvisible = new QAction(icons_manager->loadIcon(s.pixmapName(Invisible, false, false)), tr("Invisible"), this);
	changeStatusToInvisible->setCheckable(true);
	connect(changeStatusToInvisible, SIGNAL(triggered()), this, SLOT(goInvisible()));

	changeStatusToInvisibleDesc = new QAction(icons_manager->loadIcon(s.pixmapName(Invisible, true, false)), tr("Invisible (d.)"), this);
	changeStatusToInvisibleDesc->setCheckable(true);
	connect(changeStatusToInvisibleDesc, SIGNAL(triggered()), this, SLOT(goInvisibleDesc()));

	changeStatusToOffline = new QAction(icons_manager->loadIcon(s.pixmapName(Offline, false, false)), tr("Offline"), this);
	changeStatusToOffline->setCheckable(true);
	connect(changeStatusToOffline, SIGNAL(triggered()), this, SLOT(goOffline()));

	changeStatusToOfflineDesc = new QAction(icons_manager->loadIcon(s.pixmapName(Offline, true, false)), tr("Offline (d.)"), this);
	changeStatusToOfflineDesc->setCheckable(true);
	connect(changeStatusToOfflineDesc, SIGNAL(triggered()), this, SLOT(goOfflineDesc()));

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

	statusMenu->addAction(changeStatusToOnline);
	statusMenu->addAction(changeStatusToOnlineDesc);
	statusMenu->addAction(changeStatusToBusy);
	statusMenu->addAction(changeStatusToBusyDesc);
	statusMenu->addAction(changeStatusToInvisible);
	statusMenu->addAction(changeStatusToInvisibleDesc);
	statusMenu->addAction(changeStatusToOffline);
	statusMenu->addAction(changeStatusToOfflineDesc);
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
		kdebugmf(KDEBUG_INFO, "%s\n", user.altNick().local8Bit().data());
		HtmlDocument doc;
		doc.parseHtml(KaduParser::parse(InfoPanelSyntax, user));
		doc.convertUrlsToHtml();
		doc.convertMailToHtml();
		if((EmoticonsStyle)config_file.readNumEntry("Chat", "EmoticonsStyle") != EMOTS_NONE && config_file.readBoolEntry("General", "ShowEmotPanel"))
		{
//			InfoPanel->mimeSourceFactory()->addFilePath(emoticons->themePath());
			emoticons->expandEmoticons(doc, config_file.readColorEntry("Look", "InfoPanelBgColor"), (EmoticonsStyle)config_file.readNumEntry("Chat", "EmoticonsStyle"));
		}
		InfoPanel->setText(doc.generateHtml());
//		if (config_file.readBoolEntry("General", "ShowEmotPanel"))
//			InfoPanel->scrollToBottom();
		kdebugf2();
	}
}

void Kadu::currentChanged(UserListElement user)
{
	updateInformationPanel(user);
}

// QMenuBar* Kadu::menuBar() const
// {
// 	return MenuBar;
// }

QMenu* Kadu::mainMenu() const
{
	return MainMenu;
}

KaduTabBar* Kadu::groupBar() const
{
	return GroupBar;
}

UserBox* Kadu::userbox() const
{
	return Userbox;
}

UserListElements Kadu::getUserListElements()
{
	return Userbox->selectedUsers();
}

void Kadu::setDocked(bool docked, bool dontHideOnClose1)
{
	Docked = docked;
	dontHideOnClose = dontHideOnClose1;
	qApp->setQuitOnLastWindowClosed(Docked && dontHideOnClose);
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
	if (statusMenu->isItemChecked(8) == privateStatus)
		return;

	UserStatus status = userStatusChanger->status();
	status.setFriendsOnly(privateStatus);
	userStatusChanger->userStatusSet(status);

	statusMenu->setItemChecked(8, privateStatus);
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
		kadu->setCaption(tr("Kadu: %1").arg(uin));
	}
	Myself.setAltNick(config_file.readEntry("General", "Nick"));

	kadu->setDocked(kadu->Docked, kadu->dontHideOnClose);

	InfoPanelSyntax = SyntaxList::readSyntax("infopanel", config_file.readEntry("Look", "InfoPanelSyntaxFile"),
		"<table><tr><td><img width=\"32\" height=\"32\" align=\"left\" valign=\"top\" src=\"@{ManageUsersWindowIcon}\"></td><td> "
		"<div align=\"left\"> [<b>%a</b>][ (%u)] [<br>tel.: %m][<br>IP: %i]</div></td></tr></table> <hr> <b>%s</b> [<br>%d]");
	InfoPanel->setText("<body bgcolor=\"" + config_file.readEntry("Look", "InfoPanelBgColor") + "\"></body>");
	updateInformationPanel();

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
		statusIndex = 1;
	else if (startupStatus == "Busy")
		statusIndex = 3;
	else if (startupStatus == "Invisible")
		statusIndex = 5;
	else if (startupStatus == "Offline")
		statusIndex = 6;

	if ((statusIndex == 6 || statusIndex == 7) && offlineToInvisible)
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
	showStatusOnMenu(desc.isEmpty() ? 0 : 1);
}

void Kadu::wentBusy(const QString &desc)
{
	kdebugf();
	DoBlink = false;
	showStatusOnMenu(desc.isEmpty() ? 2 : 3);
}

void Kadu::wentInvisible(const QString &desc)
{
	kdebugf();
	DoBlink = false;
	showStatusOnMenu(desc.isEmpty() ? 4 : 5);
}

void Kadu::wentOffline(const QString &desc)
{
	kdebugf();
	DoBlink = false;
	showStatusOnMenu(desc.isEmpty() ? 6 : 7);
}

void Kadu::showStatusOnMenu(int statusNr)
{
	kdebugf();

	switch (statusNr)
	{
		case 0: changeStatusToOnline->setChecked(true); break;
		case 1: changeStatusToOnlineDesc->setChecked(true); break;
		case 2: changeStatusToBusy->setChecked(true); break;
		case 3: changeStatusToBusyDesc->setChecked(true); break;
		case 4: changeStatusToInvisible->setChecked(true); break;
		case 5: changeStatusToInvisibleDesc->setChecked(true); break;
		case 6: changeStatusToOffline->setChecked(true); break;
		case 7: changeStatusToOfflineDesc->setChecked(true); break;
	}

	for(int i = 0; i < 8; ++i)
	{
		statusMenu->setItemChecked(i, false);
		dockMenu->setItemChecked(i, false);
	}
	statusMenu->setItemChecked(statusNr, true);
	dockMenu->setItemChecked(statusNr, true);
	statusMenu->setItemChecked(8, gadu->currentStatus().isFriendsOnly());
	dockMenu->setItemChecked(8, gadu->currentStatus().isFriendsOnly());

	statusButton->setText(qApp->translate("@default", gadu->currentStatus().name().ascii()));
	statusMenu->setItemEnabled(7, statusNr != 6);
	dockMenu->setItemEnabled(7, statusNr != 6);

	QPixmap pix = gadu->currentStatus().pixmap();
	QIcon icon(pix);
	QString iconName = gadu->currentStatus().toString();

	statusButton->setIcon(icon);
	setMainWindowIcon(pix);
//	KaduActions["openStatusAction"]->setPixmaps(pix);
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

extern char SystemUserName[];
void Kadu::deleteOldConfigFiles()
{
	kdebugf();
	////////////start - to be removed at 0.6-svn
	QDir oldConfigs(ggPath(), "kadu.conf.backup.*", QDir::Name, QDir::Files);
//	kdebugm(KDEBUG_INFO, "%d\n", oldConfigs.count());
	if (oldConfigs.count() > 20)
		for (unsigned int i = 0, max = oldConfigs.count() - 20; i < max; ++i)
		{
//			kdebugm(KDEBUG_DUMP, "deleting %s\n", oldConfigs[i].local8Bit().data());
			QFile::remove(ggPath(oldConfigs[i]));
		}
//	kdebugm(KDEBUG_INFO, "configs deleted\n");
	//////////stop - to be removed at 0.6-svn

	QDir oldConfigs2(ggPath(), "kadu.conf.xml.backup.*", QDir::Name, QDir::Files);
//	kdebugm(KDEBUG_INFO, "%d\n", oldConfigs2.count());
	if (oldConfigs2.count() > 20)
		for (unsigned int i = 0, max = oldConfigs2.count() - 20; i < max; ++i)
		{
//			kdebugm(KDEBUG_DUMP, "deleting %s\n", oldConfigs2[i].local8Bit().data());
			QFile::remove(ggPath(oldConfigs2[i]));
		}
//	kdebugm(KDEBUG_INFO, "configs2 deleted\n");

	QDir oldBacktraces(ggPath(), "kadu.backtrace.*", QDir::Name, QDir::Files);
	if (oldBacktraces.count() > 20)
		for (unsigned int i = 0, max = oldBacktraces.count() - 20; i < max; ++i)
		{
//			kdebugm(KDEBUG_DUMP, "deleting %s\n", oldBacktraces[i].local8Bit().data());
			QFile::remove(ggPath(oldBacktraces[i]));
		}
//	kdebugm(KDEBUG_INFO, "bts deleted\n");

	QDir oldDebugs("/tmp/", QString("kadu-%1-*.dbg").arg(SystemUserName), QDir::Name, QDir::Files);
	if (oldDebugs.count() > 5)
		for (unsigned int i = 0, max = oldDebugs.count() - 5; i < max; ++i)
		{
//			kdebugm(KDEBUG_DUMP, "deleting %s\n", oldDebugs[i].local8Bit().data());
			QFile::remove("/tmp/" + oldDebugs[i]);
		}
//	kdebugm(KDEBUG_INFO, "debugs deleted\n");
	kdebugf2();
}

void Kadu::setMainWindowIcon(const QPixmap &icon)
{
	bool blocked = false;
	emit settingMainIconBlocked(blocked);
	if (!blocked)
	{
		setWindowIcon(icon);
		QApplication::setWindowIcon(icon);
	}
}

const QDateTime &Kadu::startTime() const
{
	return StartTime;
}

void Kadu::customEvent(QCustomEvent *e)
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
	config_file.addVariable("Look", "ChatFont", defaultFont);
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
	config_file.addVariable("Look", "PanelFont", defaultFont);
	config_file.addVariable("Look", "PanelVerticalScrollBar", false);
	config_file.addVariable("Look", "ParagraphSeparator", 4);
	config_file.addVariable("Look", "ShowBold", true);
	config_file.addVariable("Look", "ShowDesc", true);
	config_file.addVariable("Look", "ShowInfoPanel", true);
	config_file.addVariable("Look", "ShowMultilineDesc", true);
	config_file.addVariable("Look", "ShowStatusButton", true);
	config_file.addVariable("Look", "Style", "kadu");
	config_file.addVariable("Look", "UserboxBackgroundDisplayStyle", "Stretched");
	config_file.addVariable("Look", "UserboxBgColor", w.paletteBackgroundColor());
	config_file.addVariable("Look", "UserBoxColumnCount", 1);
	config_file.addVariable("Look", "UserboxFgColor", w.paletteForegroundColor());
	config_file.addVariable("Look", "UserboxFont", defaultFont);
	config_file.addVariable("Look", "UseUserboxBackground", false);

	config_file.addVariable("Network", "AllowDCC", true);
	config_file.addVariable("Network", "DefaultPort", 0);
	config_file.addVariable("Network", "isDefServers", true);
	config_file.addVariable("Network", "Server", "");
	config_file.addVariable("Network", "TimeoutInMs", 5000);
	config_file.addVariable("Network", "UseProxy", false);

	config_file.addVariable("ShortCuts", "chat_bold", "Ctrl+B");
	config_file.addVariable("ShortCuts", "chat_clear", "F9");
	config_file.addVariable("ShortCuts", "chat_close", "Esc");
	config_file.addVariable("ShortCuts", "chat_italic", "Ctrl+I");
	config_file.addVariable("ShortCuts", "chat_newline", "Return");
	config_file.addVariable("ShortCuts", "chat_underline", "Ctrl+U");
	config_file.addVariable("ShortCuts", "kadu_adduser", "Ctrl+N");
	config_file.addVariable("ShortCuts", "kadu_configure", "F2");
	config_file.addVariable("ShortCuts", "kadu_deleteuser", "Del");
	config_file.addVariable("ShortCuts", "kadu_modulesmanager", "F4");
	config_file.addVariable("ShortCuts", "kadu_openchatwith", "Ctrl+L");
	config_file.addVariable("ShortCuts", "kadu_persinfo", "Ins");
	config_file.addVariable("ShortCuts", "kadu_searchuser", "Ctrl+F");
	config_file.addVariable("ShortCuts", "kadu_showoffline", "F9");
	config_file.addVariable("ShortCuts", "kadu_showonlydesc", "F10");
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
