/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qcheckbox.h>
#include <qclipboard.h>
#include <qcombobox.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qregexp.h>
#include <qsplitter.h>
#include <qstyle.h>
#include <qstylefactory.h>
#include <qtextcodec.h>

#include <sys/file.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <time.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "about.h"
#include "action.h"
#include "chat_manager.h"
#include "config_dialog.h"
#include "config_file.h"
#include "debug.h"
#include "expimp.h"
#include "gadu_images_manager.h"
#include "groups_manager.h"
#include "html_document.h"
#include "icons_manager.h"
#include "ignore.h"
#include "kadu.h"
#include "kadu-config.h"
#include "kadu_parser.h"
#include "kadu_text_browser.h"
#include "message_box.h"
#include "misc.h"
#include "modules.h"
#include "pending_msgs.h"
#include "personal_info.h"
#include "protocols_manager.h"
#include "search.h"
#include "status_changer.h"
#include "tabbar.h"
#include "updates.h"
#include "userbox.h"
#include "userinfo.h"

static QTimer* blinktimer;
QPopupMenu* dockMenu;

int lockFileHandle;
QFile *lockFile;
struct flock *lock_str;

const char *Kadu::SyntaxText = QT_TRANSLATE_NOOP("@default", "Syntax: %s - status, %d - description, %i - ip, %n - nick, %a - altnick, %f - first name\n%r - surname, %m - mobile, %u - uin, %g - group, %o - return _space_ if user doesn't have us in userlist\n%v - revDNS, %p - port %e - email %x - max image size\n");
bool Kadu::Closing = false;

void Kadu::keyPressEvent(QKeyEvent *e)
{
//	kdebugf();
	if (e->key() == Key_Escape)
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
	else if (HotKey::shortCut(e,"ShortCuts", "kadu_showoffline"))
		groups_manager->changeDisplayingOffline();
	else if (HotKey::shortCut(e,"ShortCuts", "kadu_showonlydesc"))
		groups_manager->changeDisplayingWithoutDescription();
	else if (HotKey::shortCut(e,"ShortCuts", "kadu_configure"))
		configure();
	else if (HotKey::shortCut(e,"ShortCuts", "kadu_modulesmanager"))
		modules_manager->showDialog();

	emit keyPressed(e);

	QWidget::keyPressEvent(e);
//	kdebugf2();
}

/* a monstrous constructor so Kadu would take longer to start up */
Kadu::Kadu(QWidget *parent, const char *name) : QWidget(parent, name),
	kaduslots(new KaduSlots(this, "kaduslots")),
	TopDockArea(0), InfoPanel(0), MenuBar(0), MainMenu(0), GroupBar(0),
	Userbox(0), statusMenu(0), statusButton(), lastPositionBeforeStatusMenuHide(),
	StartTime(QDateTime::currentDateTime()), updateInformationPanelTimer(),
	status(), selectedUsers(new UserGroup(userlist->count() / 2)),
	ShowMainWindowOnStart(true), DoBlink(false), BlinkOn(false),
	Docked(false), dontHideOnClose(false), personalInfoMenuId(-1)
{
	kdebugf();
	kadu = this;
	blinktimer = 0;

	UinType myUin = config_file.readUnsignedNumEntry("General", "UIN");

	QWidget w;
	config_file.addVariable("Look", "InfoPanelBgColor", w.paletteBackgroundColor());
	config_file.addVariable("Look", "InfoPanelFgColor", w.paletteForegroundColor());

	ConfigDialog::addHGroupBox("General", "General", QT_TRANSLATE_NOOP("@default", "User data"));
	ConfigDialog::addLineEdit("General", "User data", QT_TRANSLATE_NOOP("@default", "Uin"), "UIN");
	ConfigDialog::addLineEdit("General", "User data", QT_TRANSLATE_NOOP("@default", "Password"), "Password");
	ConfigDialog::addLineEdit("General", "User data", QT_TRANSLATE_NOOP("@default", "Nick"), "Nick", tr("Me"));
	ConfigDialog::addComboBox("General", "General", QT_TRANSLATE_NOOP("@default", "Set language:"));
	ConfigDialog::addGrid("General", "General", "grid-beginner", 2, 0, Beginner);
	ConfigDialog::addGrid("General", "General", "grid-advanced", 2, 0, Advanced);
	ConfigDialog::addGrid("General", "General", "grid-expert", 2, 0, Expert);

	ConfigDialog::addCheckBox("General", "grid-beginner", QT_TRANSLATE_NOOP("@default", "Private status"), "PrivateStatus", false, QT_TRANSLATE_NOOP("@default", "When enabled, you're visible only to users on your list"));

	ConfigDialog::addCheckBox("General", "grid-advanced", QT_TRANSLATE_NOOP("@default", "Check for updates"), "CheckUpdates", true, QT_TRANSLATE_NOOP("@default", "Automatically checks whether a new version is available"), 0, Advanced);
	ConfigDialog::addCheckBox("General", "grid-expert", QT_TRANSLATE_NOOP("@default", "Show emoticons in panel"), "ShowEmotPanel", false, 0, 0, Expert);
	ConfigDialog::addCheckBox("General", "grid-expert", QT_TRANSLATE_NOOP("@default", "Allow executing commands by parser"), "AllowExecutingFromParser", false, 0, 0, Expert);
	ConfigDialog::addCheckBox("General", "grid-expert", QT_TRANSLATE_NOOP("@default", "Always show anonymous contacts with messages"), "ShowAnonymousWithMsgs", false, 0, 0, Expert);
#ifdef DEBUG_ENABLED
	ConfigDialog::addLineEdit("General", "General", QT_TRANSLATE_NOOP("@default", "Debugging mask"), "DEBUG_MASK",
		QString::null, 0, 0, Expert);
#endif

	ConfigDialog::addVGroupBox("General", "General", "Status");
	ConfigDialog::addComboBox("General", "Status", QT_TRANSLATE_NOOP("@default", "Default status"), 0, "cb_defstatus");
	ConfigDialog::addCheckBox("General", "Status", QT_TRANSLATE_NOOP("@default", "On shutdown, set current description"), "DisconnectWithCurrentDescription");
	ConfigDialog::connectSlot("General", "On shutdown, set current description", SIGNAL(toggled(bool)), kaduslots, SLOT(updateStatus(bool)));
	ConfigDialog::addCheckBox("General", "Status", QT_TRANSLATE_NOOP("@default", "Use KaduParser to parse status"), "ParseStatus", false);

	ConfigDialog::addHBox("General", "Status", "discstatus");
	ConfigDialog::addCheckBox("General", "discstatus", QT_TRANSLATE_NOOP("@default", "On shutdown, set description:"), "DisconnectWithDescription", false);
	ConfigDialog::addLineEdit("General", "discstatus", 0, "DisconnectDescription", QString::null, 0, "e_defaultstatus");
//	ConfigDialog::addSpinBox("General", "Status", QT_TRANSLATE_NOOP("@default", "Number of kept descriptions"), "NumberOfDescriptions", 1, 30, 1, 4, 0, 0, Advanced);
	ConfigDialog::addSpinBox("General", "Status", QT_TRANSLATE_NOOP("@default", "Number of kept descriptions"), "NumberOfDescriptions", 1, 30, 1, 4, 0, 0, Advanced);

	ConfigDialog::registerSlotOnCreateTab("General", kaduslots, SLOT(onCreateTabGeneral()));
	ConfigDialog::registerSlotOnCreateTab("Look", kaduslots, SLOT(onCreateTabLook()));
	ConfigDialog::registerSlotOnApplyTab("Look", kaduslots, SLOT(onApplyTabLook()));
	ConfigDialog::registerSlotOnApplyTab("General", kaduslots, SLOT(onApplyTabGeneral()));

	ConfigDialog::addVGroupBox("ShortCuts", "ShortCuts", QT_TRANSLATE_NOOP("@default", "Define keys"));
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Remove from userlist"), "kadu_deleteuser", "Del");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "View / edit user info"), "kadu_persinfo", "Ins");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Search this user in directory"), "kadu_searchuser", "Ctrl+F");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Show / hide offline users"), "kadu_showoffline", "F9");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Show / hide users without description"), "kadu_showonlydesc", "F10");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Configuration"), "kadu_configure", "F2");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Add user"), "kadu_adduser", "Ctrl+N");

	//zaladowanie wartosci domyslnych (pierwsze uruchomienie)
	config_file.addVariable("General", "UserBoxHeight", 300);
	config_file.addVariable("General", "DescriptionHeight", 60);
	config_file.addVariable("General", "DefaultStatusIndex", 7);

	config_file.addVariable("Look", "UserboxFont", defaultFont);
	config_file.addVariable("Look", "PanelFont", defaultFont);

	ConfigDialog::addComboBox("Look", "Look",
			QT_TRANSLATE_NOOP("@default","Qt Theme"));

	ConfigDialog::addGrid("Look", "Look", "varOpts-beginner", 2, 0, Beginner);
	ConfigDialog::addGrid("Look", "Look", "varOpts-advanced", 2, 0, Advanced);
	ConfigDialog::addGrid("Look", "Look", "varOpts-expert", 2, 0, Expert);

		ConfigDialog::addCheckBox("Look", "varOpts-advanced", QT_TRANSLATE_NOOP("@default", "Show status button"), "ShowStatusButton", true, 0, 0, Advanced);
		ConfigDialog::addCheckBox("Look", "varOpts-expert", QT_TRANSLATE_NOOP("@default", "Display group tabs"), "DisplayGroupTabs", true, 0, 0, Expert);
	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Colors"), 0, Advanced);
		ConfigDialog::addVGroupBox("Look", "Colors", QT_TRANSLATE_NOOP("@default", "Main window"));
			ConfigDialog::addColorButton("Look", "Main window", QT_TRANSLATE_NOOP("@default", "Panel background color"), "InfoPanelBgColor", config_file.readColorEntry("Look","InfoPanelBgColor"), 0, "panel_bg_color");
			ConfigDialog::addColorButton("Look", "Main window", QT_TRANSLATE_NOOP("@default", "Panel font color"), "InfoPanelFgColor", config_file.readColorEntry("Look","InfoPanelFgColor"), 0, "panel_font_color");

	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Previews"), 0, Advanced);

	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Fonts"), 0, Advanced);
		ConfigDialog::addSelectFont("Look", "Fonts", QT_TRANSLATE_NOOP("@default", "Font in panel"), "PanelFont", defaultFont->toString(), 0, "panel_font_box");

	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Information panel"));
		ConfigDialog::addCheckBox("Look", "Information panel", QT_TRANSLATE_NOOP("@default", "Show information panel"), "ShowInfoPanel", true);
		ConfigDialog::addCheckBox("Look", "Information panel", QT_TRANSLATE_NOOP("@default", "Show vertical scrollbar in information panel"), "PanelVerticalScrollbar", true, 0, 0, Expert);
		config_file.writeEntry("Look", "PanelContents", config_file.readEntry("Look", "PanelContents", "<table><tr><td><img width=\"32\" height=\"32\" align=\"left\" valign=\"top\" src=\"@{ManageUsersWindowIcon}\"></td>\n<td> <div align=\"left\"> [<b>%a</b>][ (%u)] [<br>tel.: %m][<br>IP: %i][<br>%oYou are not on the list] </div></td></tr></table> <hr> <b>%s</b> [<br>%d]").replace("You are not on the list", tr("You are not on the list")));
		ConfigDialog::addTextEdit("Look", "Information panel", QT_TRANSLATE_NOOP("@default", "Information panel syntax:"), "PanelContents", "", SyntaxText, 0, Expert);

	ConfigDialog::connectSlot("Look", "Panel background color", SIGNAL(changed(const char *, const QColor&)), kaduslots, SLOT(chooseColor(const char *, const QColor&)), "panel_bg_color");
	ConfigDialog::connectSlot("Look", "Panel font color", SIGNAL(changed(const char *, const QColor&)), kaduslots, SLOT(chooseColor(const char *, const QColor&)), "panel_font_color");

	ConfigDialog::connectSlot("Look", "Font in panel", SIGNAL(changed(const char *, const QFont&)),kaduslots, SLOT(chooseFont(const char *, const QFont&)), "panel_font_box");

	MainLayout = new QVBoxLayout(this);

	TopDockArea = new DockArea(Qt::Horizontal, DockArea::Normal, this,
		"topDockArea", Action::TypeGlobal | Action::TypeUser | Action::TypeUserList);
	connect(TopDockArea, SIGNAL(selectedUsersNeeded(const UserGroup*&)),
		this, SLOT(selectedUsersNeeded(const UserGroup*&)));
	MainLayout->addWidget (TopDockArea);

	QSplitter *split = new QSplitter(Qt::Vertical, this, "splitter");
	MainLayout->addWidget (split);

	QHBox* hbox1 = new QHBox(split, "firstBox");

	// groupbar
	GroupBar = new KaduTabBar(hbox1, "groupbar");
	hbox1->setStretchFactor(GroupBar, 1);

	StatusChangerManager::initModule();
	connect(status_changer_manager, SIGNAL(statusChanged(UserStatus)), this, SLOT(changeStatus(UserStatus)));

	userStatusChanger = new UserStatusChanger();
	splitStatusChanger = new SplitStatusChanger(GG_STATUS_DESCR_MAXSIZE);

	status_changer_manager->registerStatusChanger(userStatusChanger);
	status_changer_manager->registerStatusChanger(splitStatusChanger);

	// gadu, chat, search
	GaduProtocol::initModule();
	ChatManager::initModule();
	SearchDialog::initModule();

	// userbox
	UserBox::initModule();
	Userbox = new UserBox(userlist, hbox1, "userbox");
	hbox1->setStretchFactor(Userbox, 100);
	connect(UserBox::userboxmenu, SIGNAL(popup()), this, SLOT(popupMenu()));
	connect(Userbox, SIGNAL(rightButtonPressed(QListBoxItem *, const QPoint &)),
		UserBox::userboxmenu, SLOT(show(QListBoxItem *)));
	connect(UserBox::management, SIGNAL(aboutToShow()), this, SLOT(popupMenu()));
	connect(Userbox, SIGNAL(doubleClicked(UserListElement)), this, SLOT(sendMessage(UserListElement)));
	connect(Userbox, SIGNAL(returnPressed(UserListElement)), this, SLOT(sendMessage(UserListElement)));
	connect(Userbox, SIGNAL(mouseButtonClicked(int, QListBoxItem *, const QPoint &)),
		this, SLOT(mouseButtonClicked(int, QListBoxItem *)));
	connect(Userbox, SIGNAL(currentChanged(UserListElement)), this, SLOT(currentChanged(UserListElement)));
	UserBox::userboxmenu->addItem("OpenChat", tr("Open chat window") ,this, SLOT(openChat()));
	UserBox::userboxmenu->insertSeparator();
	UserBox::userboxmenu->addItem("CopyDescription", tr("Copy description"), this, SLOT(copyDescription()));
	UserBox::userboxmenu->addItem("OpenDescriptionLink", tr("Open description link in browser"), this, SLOT(openDescriptionLink()));
	UserBox::userboxmenu->addItem("CopyPersonalInfo", tr("Copy personal info"), this, SLOT(copyPersonalInfo()));
	UserBox::userboxmenu->addItem("LookupUserInfo", tr("Search this user in directory"), this, SLOT(lookupInDirectory()),HotKey::shortCutFromFile("ShortCuts", "kadu_searchuser"));
	UserBox::userboxmenu->addItem("EditUserInfo", tr("View / edit user info"), this, SLOT(showUserInfo()),HotKey::shortCutFromFile("ShortCuts", "kadu_persinfo"));
	UserBox::userboxmenu->insertSeparator();

	UserBox::management->addItem(tr("Ignore user"), this, SLOT(ignoreUser()));
	UserBox::management->addItem(tr("Block user"), this, SLOT(blockUser()));
	UserBox::management->addItem(tr("Notify about user"), this, SLOT(notifyUser()));
	UserBox::management->addItem(tr("Offline to user"), this, SLOT(offlineToUser()));
	UserBox::management->addItem(tr("Hide description"), this, SLOT(hideDescription()));
	UserBox::management->insertSeparator();
	UserBox::management->addItem("RemoveFromUserlist", tr("Remove from userlist"), this, SLOT(deleteUsers()),HotKey::shortCutFromFile("ShortCuts", "kadu_deleteuser"));

	UserBox::userboxmenu->insertItem(tr("User management"), UserBox::management);

	groups_manager->setTabBar(GroupBar);
	setDocked(Docked, dontHideOnClose);

	loadGeometry(this, "General", "Geometry", 0, 30, 145, 465);

	readIgnored();

	/* a newbie? */

	if (myUin)
		setCaption(tr("Kadu: %1").arg(myUin));

	pending.loadFromFile();

	Action* inact_users_action = new Action(icons_manager->loadIcon("ShowHideInactiveUsers"),
		tr("Show / hide offline users"), "inactiveUsersAction", Action::TypeUserList);
	inact_users_action->setToggleAction(true);
	inact_users_action->setAllOn(!config_file.readBoolEntry("General", "ShowOffline"));
	connect(inact_users_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(inactiveUsersActionActivated()));
	KaduActions.insert("inactiveUsersAction", inact_users_action);

	Action* desc_users_action = new Action(icons_manager->loadIcon("ShowOnlyDescriptionUsers"),
		tr("Show / hide users without description"), "descriptionUsersAction", Action::TypeUserList);
	desc_users_action->setToggleAction(true);
	desc_users_action->setAllOn(!config_file.readBoolEntry("General", "ShowWithoutDescription"));
	connect(desc_users_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(descriptionUsersActionActivated()));
	KaduActions.insert("descriptionUsersAction", desc_users_action);

	Action* configuration_action = new Action(icons_manager->loadIcon("Configuration"),
		tr("Configuration"), "configurationAction", Action::TypeGlobal);
	connect(configuration_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(configurationActionActivated()));
	KaduActions.insert("configurationAction", configuration_action);

	Action* edit_user_action = new Action(icons_manager->loadIcon("EditUserInfo"),
		tr("View / edit user info"), "editUserAction", Action::TypeUser);
	connect(edit_user_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(editUserActionActivated(const UserGroup*)));
	KaduActions.insert("editUserAction", edit_user_action);

	Action* add_user_action = new Action(icons_manager->loadIcon("AddUser"),
		tr("Add user"), "addUserAction", Action::TypeGlobal);
	connect(add_user_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(addUserActionActivated(const UserGroup*)));
	KaduActions.insert("addUserAction", add_user_action);

	Action* open_search_action = new Action(icons_manager->loadIcon("LookupUserInfo"),
		tr("Search user in directory"), "openSearchAction", Action::TypeGlobal);
	connect(open_search_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(searchInDirectory()));
	KaduActions.insert("openSearchAction", open_search_action);

	KaduActions.addDefaultToolbarAction("Kadu toolbar", "inactiveUsersAction");
	KaduActions.addDefaultToolbarAction("Kadu toolbar", "descriptionUsersAction");
	KaduActions.addDefaultToolbarAction("Kadu toolbar", "configurationAction");
	KaduActions.addDefaultToolbarAction("Kadu toolbar", "editUserAction");
	KaduActions.addDefaultToolbarAction("Kadu toolbar", "openSearchAction");
	KaduActions.addDefaultToolbarAction("Kadu toolbar", "addUserAction");

	/* guess what */
	createMenu();
	createStatusPopupMenu();

	connect(statusMenu, SIGNAL(aboutToHide()), this, SLOT(statusMenuAboutToHide()));
	connect(dockMenu, SIGNAL(aboutToHide()), this, SLOT(dockMenuAboutToHide()));

	dockMenu->insertSeparator();
	dockMenu->insertItem(icons_manager->loadIcon("Exit"), tr("&Exit Kadu"), 9);
	icons_manager->registerMenuItem(dockMenu, tr("&Exit Kadu"), "Exit");

	InfoPanel = new KaduTextBrowser(split, "InfoPanel");
	InfoPanel->setFrameStyle(QFrame::NoFrame);
	InfoPanel->setMinimumHeight(int(1.5 * QFontMetrics(InfoPanel->QTextEdit::font()).height()));
//	InfoPanel->resize(InfoPanel->size().width(), int(1.5 * QFontMetrics(InfoPanel->font()).height()));
	InfoPanel->setTextFormat(Qt::RichText);
	InfoPanel->setAlignment(Qt::AlignVCenter | Qt::WordBreak | Qt::DontClip);
	if (!config_file.readBoolEntry("Look", "PanelVerticalScrollbar"))
		InfoPanel->setVScrollBarMode(QScrollView::AlwaysOff);
	InfoPanel->setPaletteBackgroundColor(config_file.readColorEntry("Look", "InfoPanelBgColor"));
	InfoPanel->setPaletteForegroundColor(config_file.readColorEntry("Look", "InfoPanelFgColor"));
	InfoPanel->QTextEdit::setFont(config_file.readFontEntry("Look", "PanelFont"));
	if((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle")==EMOTS_ANIMATED)
		InfoPanel->setStyleSheet(new AnimStyleSheet(InfoPanel, emoticons->themePath()));
	else
		InfoPanel->setStyleSheet(new StaticStyleSheet(InfoPanel,emoticons->themePath()));

	if (!config_file.readBoolEntry("Look", "ShowInfoPanel"))
		InfoPanel->QWidget::hide();
	connect(&updateInformationPanelTimer, SIGNAL(timeout()), this, SLOT(updateInformationPanel()));

	statusButton = new QPushButton(QIconSet(icons_manager->loadIcon("Offline")), tr("Offline"), this, "statusButton");
	MainLayout->addWidget (statusButton);
	statusButton->setPopup(statusMenu);

	if (!config_file.readBoolEntry("Look", "ShowStatusButton"))
		statusButton->hide();

	QValueList<int> splitsizes;

	splitsizes.append(config_file.readNumEntry("General", "UserBoxHeight"));
	splitsizes.append(config_file.readNumEntry("General", "DescriptionHeight"));

	split->setSizes(splitsizes);

	connect(gadu, SIGNAL(chatMsgReceived2(Protocol *, UserListElements, const QString &, time_t, bool)),
		this, SLOT(chatMsgReceived(Protocol *, UserListElements, const QString &, time_t, bool)));
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

	connect(&(gadu->currentStatus()), SIGNAL(goOnline(const QString &)),
		this, SLOT(wentOnline(const QString &)));
	connect(&(gadu->currentStatus()), SIGNAL(goBusy(const QString &)),
		this, SLOT(wentBusy(const QString &)));
	connect(&(gadu->currentStatus()), SIGNAL(goInvisible(const QString &)),
		this, SLOT(wentInvisible(const QString &)));
	connect(&(gadu->currentStatus()), SIGNAL(goOffline(const QString &)),
		this, SLOT(wentOffline(const QString &)));

	MainLayout->setResizeMode(QLayout::Minimum);
	chat_manager->loadOpenedWindows();

	kdebugf2();
}

QVBoxLayout* Kadu::mainLayout() const
{
	return MainLayout;
}

void Kadu::popupMenu()
{
	kdebugf();
	UserBox *activeUserBox = UserBox::activeUserBox();
	if (activeUserBox == NULL)//to siê zdarza...
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
	QString myGGUIN = QString::number(config_file.readUnsignedNumEntry("General", "UIN"));
	CONST_FOREACH(user, users)
	{
		if (!containsUserWithoutID && !(*user).usesProtocol("Gadu"))
			containsUserWithoutID = true;
		if (!containsMe && (*user).usesProtocol("Gadu") && (*user).ID("Gadu") == myGGUIN)
			containsMe = true;
	}

	int ignoreuseritem = UserBox::management->getItem(tr("Ignore user"));
	int blockuseritem = UserBox::management->getItem(tr("Block user"));
	int notifyuseritem = UserBox::management->getItem(tr("Notify about user"));
	int offlinetouseritem = UserBox::management->getItem(tr("Offline to user"));
	int hidedescriptionitem = UserBox::management->getItem(tr("Hide description"));
	int chatitem = UserBox::userboxmenu->getItem(tr("Open chat window"));

	if (containsUserWithoutID)
	{
		UserBox::userboxmenu->setItemVisible(ignoreuseritem, false);
		UserBox::userboxmenu->setItemVisible(blockuseritem, false);
		UserBox::userboxmenu->setItemVisible(notifyuseritem, false);
		UserBox::userboxmenu->setItemVisible(offlinetouseritem, false);
		UserBox::userboxmenu->setItemVisible(hidedescriptionitem, false);
		UserBox::userboxmenu->setItemVisible(chatitem, false);
	}
	else
	{
		bool on;
		UserListElements selectedUsers = activeUserBox->selectedUsers();
		if (isIgnored(selectedUsers))
			UserBox::userboxmenu->setItemChecked(ignoreuseritem, true);

		on = true;
		CONST_FOREACH(user, users)
			if (!(*user).usesProtocol("Gadu") || !(*user).protocolData("Gadu", "Blocking").toBool())
			{
				on = false;
				break;
			}
		UserBox::userboxmenu->setItemChecked(blockuseritem, on);

		on = true;
		CONST_FOREACH(user, users)
			if (!(*user).usesProtocol("Gadu") || !(*user).protocolData("Gadu", "OfflineTo").toBool())
			{
				on = false;
				break;
			}
		UserBox::userboxmenu->setItemVisible(offlinetouseritem, config_file.readBoolEntry("General", "PrivateStatus"));
		UserBox::userboxmenu->setItemChecked(offlinetouseritem, on);

		on = false;
		CONST_FOREACH(user, users)
			if ((*user).data("HideDescription").toString() == "true")
			{
				on = true;
				break;
			}
		UserBox::userboxmenu->setItemChecked(hidedescriptionitem, on);

		on = true;
		CONST_FOREACH(user, users)
			if (!(*user).notify())
			{
				on = false;
				break;
			}
		UserBox::userboxmenu->setItemVisible(notifyuseritem, !config_file.readBoolEntry("Notify", "NotifyAboutAll"));
		UserBox::userboxmenu->setItemChecked(notifyuseritem, on);

		if (containsMe)
		{
			UserBox::userboxmenu->setItemVisible(ignoreuseritem, false);
			UserBox::userboxmenu->setItemVisible(blockuseritem, false);
			UserBox::userboxmenu->setItemVisible(offlinetouseritem, false);
			UserBox::userboxmenu->setItemVisible(chatitem, false);
		}
	}

	if (users.count() != 1)
		UserBox::userboxmenu->setItemVisible(UserBox::userboxmenu->getItem(tr("View / edit user info")), false);
	if ((users.count() != 1) || !firstUser.usesProtocol("Gadu"))
		UserBox::userboxmenu->setItemVisible(UserBox::userboxmenu->getItem(tr("Search this user in directory")), false);
	if ((users.count() != 1) || !firstUser.usesProtocol("Gadu") || firstUser.status("Gadu").description().isEmpty())
		UserBox::userboxmenu->setItemVisible(UserBox::userboxmenu->getItem(tr("Copy description")), false);
	if ((users.count() != 1) || !firstUser.usesProtocol("Gadu") || firstUser.status("Gadu").description().isEmpty() ||
		firstUser.status("Gadu").description().find(HtmlDocument::urlRegExp()) == -1)
		UserBox::userboxmenu->setItemVisible(UserBox::userboxmenu->getItem(tr("Open description link in browser")), false);
	kdebugf2();
}

void Kadu::configure()
{
	configurationActionActivated();
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
	QString status;
	if (user.usesProtocol("Gadu"))
		status = user.status("Gadu").description();
	if (!status.isEmpty())
	{
		int idx_start = status.find(HtmlDocument::urlRegExp());
		if (idx_start >= 0)
		{
			int idx_stop = status.find(QRegExp("\\s"), idx_start);
			if (idx_stop <= idx_start)
				idx_stop = status.length();
			openWebBrowser(status.mid(idx_start, (idx_stop - idx_start)));
		}
	}
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

void Kadu::lookupInDirectory()
{
	kdebugf();
	SearchDialog *sd;
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
			sd = new SearchDialog(0, "User info", user.ID("Gadu").toUInt());
			sd->show();
			sd->firstSearch();
		}
	}
	else
	{
		sd = new SearchDialog();
		sd->show();
	}
	kdebugf2();
}

void Kadu::selectedUsersNeeded(const UserGroup*& users)
{
	kdebugf();
	UserBox* activeUserBox = UserBox::activeUserBox();
	if (activeUserBox==NULL)
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

void Kadu::inactiveUsersActionActivated()
{
	groups_manager->changeDisplayingOffline();
}

void Kadu::descriptionUsersActionActivated()
{
	groups_manager->changeDisplayingWithoutDescription();
}

void Kadu::configurationActionActivated()
{
	ConfigDialog::showConfigDialog(qApp);
}

void Kadu::editUserActionActivated(const UserGroup* users)
{
	kdebugf();
	if (users->count() == 1)
		(new UserInfo(*users->begin(), 0, "user info"))->show();
	kdebugf2();
}

void Kadu::addUserActionActivated(const UserGroup* users)
{
	kdebugf();
	(new UserInfo(UserListElement(), 0, "add user"))->show();
	kdebugf2();
}

void Kadu::showUserInfo()
{
	kdebugf();
	UserBox *activeUserBox=UserBox::activeUserBox();
	if (activeUserBox==NULL)
	{
		kdebugf2();
		return;
	}
	UserGroup users(activeUserBox->selectedUsers());
	editUserActionActivated(&users);
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
	(new PersonalInfoDialog())->show();
}

void Kadu::addUserAction()
{
	const UserGroup* users;
	selectedUsersNeeded(users);
	addUserActionActivated(users);
}

void Kadu::manageIgnored()
{
	(new Ignored(0, "ignored"))->show();
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
	chat_manager->openPendingMsgs(activeUserBox->selectedUsers());
	kdebugf2();
}

void Kadu::searchInDirectory()
{
	(new SearchDialog())->show();
}

void Kadu::help()
{
	openWebBrowser("http://www.kadu.net/doc.php");
}

void Kadu::about()
{
	(new About(this, "about_window"))->show();
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
	(new UserlistImportExport(this, "userlist_import_export"))->show();
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
	if (isIgnored(users))
		delIgnored(users);
	else
		addIgnored(users);
	writeIgnored();
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
	GroupBar->setFont(QFont(config_file.readFontEntry("Look", "UserboxFont").family(), config_file.readFontEntry("Look", "UserboxFont").pointSize(),75));

	InfoPanel->setPaletteBackgroundColor(config_file.readColorEntry("Look", "InfoPanelBgColor"));
	InfoPanel->setPaletteForegroundColor(config_file.readColorEntry("Look", "InfoPanelFgColor"));
	InfoPanel->QTextEdit::setFont(config_file.readFontEntry("Look", "PanelFont"));

	if (config_file.readBoolEntry("Look", "PanelVerticalScrollbar"))
		InfoPanel->setVScrollBarMode(QScrollView::Auto);
	else
		InfoPanel->setVScrollBarMode(QScrollView::AlwaysOff);

	const UserStatus &stat = gadu->currentStatus();
	QPixmap pix = stat.pixmap();
	statusButton->setIconSet(QIconSet(pix));
	emit statusPixmapChanged(pix, stat.toString());
	kdebugf2();
}

void Kadu::removeUsers(UserListElements users)
{
	kdebugf();
	if (QMessageBox::warning(kadu, "Kadu",
		tr("Selected users:\n%0\nwill be deleted. Are you sure?").arg(users.altNicks().join(", ")),
		QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes)
	{
		return;
	}
	emit removingUsers(users);
	userlist->removeUsers(users);
	userlist->writeToConfig();
	kdebugf2();
}

void Kadu::blink()
{
	QPixmap pix;

	kdebugf();

	if (!DoBlink && !gadu->currentStatus().isOffline())
		return;
	else if (!DoBlink && gadu->currentStatus().isOffline())
	{
		pix = gadu->status().pixmap(Offline, false);
		statusButton->setIconSet(QIconSet(pix));
		emit statusPixmapChanged(pix, "Offline");
		return;
	}

	QString iconName;
	if (BlinkOn)
	{
		pix = gadu->status().pixmap(Offline, false);
		iconName = "Offline";
	}
	else
	{
		const UserStatus &stat = gadu->status();
		pix = stat.pixmap(status);
		iconName = stat.toString();
	}

	statusButton->setIconSet(QIconSet(pix));
	emit statusPixmapChanged(pix, iconName);

	BlinkOn=!BlinkOn;

	blinktimer->start(1000, TRUE);
}

void Kadu::mouseButtonClicked(int button, QListBoxItem *item)
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

	if (elem.usesProtocol("Gadu")) //TODO: elem.hasFeature("SendingMessages")
	{
		UserListElements users = activeUserBox->selectedUsers();
		if (!users.isEmpty())
			if (!users.contains(userlist->byID("Gadu", config_file.readEntry("General", "UIN"))))
				chat_manager->sendMessage(elem, users);
	}
	kdebugf2();
}

/* when we want to change the status */
void Kadu::slotHandleState(int command)
{
	kdebugf();
	ChooseDescription *cd;
	QString desc;
	bool accepted = true;
	UserListElement ule = userlist->byID("Gadu", config_file.readEntry("General", "UIN"));
	bool parse = config_file.readBoolEntry("General", "ParseStatus", false);

	status.setStatus(gadu->status());
	switch (command)
	{
		case 0:
			status.setOnline();
			break;
		case 1:
			cd = new ChooseDescription(1, &lastPositionBeforeStatusMenuHide);
			accepted = cd->exec() == QDialog::Accepted;
			if (accepted)
			{
				cd->getDescription(desc);
				if (parse)
					desc = KaduParser::parse(desc, ule, true);
				status.setOnline(desc);
			}
			delete cd;
			break;
		case 2:
			status.setBusy();
			break;
		case 3:
			cd = new ChooseDescription(3, &lastPositionBeforeStatusMenuHide);
			accepted = cd->exec() == QDialog::Accepted;
			if (accepted)
			{
				cd->getDescription(desc);
				if (parse)
					desc = KaduParser::parse(desc, ule, true);
				status.setBusy(desc);
			}
			delete cd;
			break;
		case 4:
			status.setInvisible();
			break;
		case 5:
			cd = new ChooseDescription(5, &lastPositionBeforeStatusMenuHide);
			accepted = cd->exec() == QDialog::Accepted;
			if (accepted)
			{
				cd->getDescription(desc);
				if (parse)
					desc = KaduParser::parse(desc, ule, true);
				status.setInvisible(desc);
			}
			delete cd;
			break;
		case 6:
			status.setOffline();
			break;
		case 7:
			cd = new ChooseDescription(7, &lastPositionBeforeStatusMenuHide);
			accepted = cd->exec() == QDialog::Accepted;
			if (accepted)
			{
				cd->getDescription(desc);
				if (parse)
					desc = KaduParser::parse(desc, ule, true);
				status.setOffline(desc);
			}
			delete cd;
			break;
		case 8:
			statusMenu->setItemChecked(8, !statusMenu->isItemChecked(8));
			dockMenu->setItemChecked(8, !dockMenu->isItemChecked(8));
			config_file.writeEntry("General", "PrivateStatus",statusMenu->isItemChecked(8));
			status.setFriendsOnly(statusMenu->isItemChecked(8));
			break;
	}
	if (!accepted)
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: not accepted\n");
		return;
	}

	userStatusChanger->userStatusSet(status);

	kdebugf2();
}

void Kadu::changeStatus(UserStatus newStatus)
{
	status.setStatus(newStatus);
	gadu->writeableStatus().setStatus(status);

	if (status.isOffline())
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

void Kadu::chatMsgReceived(Protocol *protocol, UserListElements senders, const QString &msg, time_t time, bool grabbed)
{
	kdebugf();
	if (grabbed)
	{
		kdebugf2();
		return;
	}

	pending.addMsg(protocol->protocolID(), senders, msg, GG_CLASS_CHAT, time);

	if (config_file.readBoolEntry("General","AutoRaise"))
	{
		kadu->showNormal();
		kadu->setFocus();
	}

	if (config_file.readBoolEntry("Chat", "OpenChatOnMessage"))
		pending.openMessages();

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

void Kadu::systemMessageReceived(const QString &msg)
{
	MessageBox::msg(msg);
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
		xml_config_file->makeBackup();
		ConfigDialog::closeDialog();
		ModulesManager::closeModule();

		Updates::closeModule();
		delete defaultFontInfo;
		delete defaultFont;

		if (config_file.readBoolEntry("Look", "ShowInfoPanel"))
		{
			config_file.writeEntry("General", "UserBoxHeight", Userbox->size().height());
			config_file.writeEntry("General", "DescriptionHeight", InfoPanel->size().height());
		}
		if (config_file.readBoolEntry("Look", "ShowStatusButton"))
			config_file.writeEntry("General", "UserBoxHeight", Userbox->size().height());
		saveGeometry(this, "General", "Geometry");

		config_file.writeEntry("General", "DefaultDescription", defaultdescriptions.join("<-->"));

		if (config_file.readNumEntry("General", "DefaultStatusIndex") == 7 || config_file.readNumEntry("General", "DefaultStatusIndex") == 8)
		{
			config_file.writeEntry("General", "LastStatusIndex", gadu->status().index());
			config_file.writeEntry("General", "LastStatusDescription", gadu->status().description());
		}

		pending.writeToFile();
		writeIgnored();
		if (!gadu->status().isOffline())
		{
			if (config_file.readBoolEntry("General", "DisconnectWithCurrentDescription"))
			{
				kdebugmf(KDEBUG_INFO, "Set status NOT_AVAIL_DESCR with current description(%s)\n", gadu->status().description().data());

				setOffline(gadu->status().description());
			}
			else if (config_file.readBoolEntry("General", "DisconnectWithDescription"))
			{
				kdebugmf(KDEBUG_INFO, "Set status NOT_AVAIL_DESCR with disconnect description(%s)\n", config_file.readEntry("General", "DisconnectDescription").local8Bit().data());

				setOffline(config_file.readEntry("General", "DisconnectDescription"));
			}
		}
		disconnect(gadu, SIGNAL(chatMsgReceived2(Protocol *, UserListElements, const QString &, time_t, bool)),
				this, SLOT(chatMsgReceived(Protocol *, UserListElements, const QString &, time_t, bool)));
		disconnect(gadu, SIGNAL(connecting()), this, SLOT(connecting()));
		disconnect(gadu, SIGNAL(connected()), this, SLOT(connected()));
		disconnect(gadu, SIGNAL(disconnected()), this, SLOT(disconnected()));
		disconnect(gadu, SIGNAL(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)),
				this, SLOT(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)));
		disconnect(gadu, SIGNAL(needTokenValue(QPixmap, QString &)),
				this, SLOT(readTokenValue(QPixmap, QString &)));
		disconnect(gadu, SIGNAL(systemMessageReceived(const QString &)), this, SLOT(systemMessageReceived(const QString &)));

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
		disconnect(Userbox, SIGNAL(rightButtonPressed(QListBoxItem *, const QPoint &)),
					UserBox::userboxmenu, SLOT(show(QListBoxItem *)));
		disconnect(Userbox, SIGNAL(doubleClicked(UserListElement)), this, SLOT(sendMessage(UserListElement)));
		disconnect(Userbox, SIGNAL(returnPressed(UserListElement)), this, SLOT(sendMessage(UserListElement)));
		disconnect(Userbox, SIGNAL(mouseButtonClicked(int, QListBoxItem *, const QPoint &)),
				this, SLOT(mouseButtonClicked(int, QListBoxItem *)));
		disconnect(Userbox, SIGNAL(currentChanged(UserListElement)), this, SLOT(currentChanged(UserListElement)));

		status_changer_manager->unregisterStatusChanger(splitStatusChanger);
		status_changer_manager->unregisterStatusChanger(userStatusChanger);

		delete splitStatusChanger;
		splitStatusChanger = 0;

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
		delete emoticons;
		IconsManager::closeModule();

		ConfigDialog::disconnectSlot("Look", "Panel background color", SIGNAL(changed(const char *, const QColor&)), kaduslots, SLOT(chooseColor(const char *, const QColor&)), "panel_bg_color");
		ConfigDialog::disconnectSlot("Look", "Panel font color", SIGNAL(changed(const char *, const QColor&)), kaduslots, SLOT(chooseColor(const char *, const QColor&)), "panel_font_color");
		ConfigDialog::disconnectSlot("Look", "Font in panel", SIGNAL(changed(const char *, const QFont&)),kaduslots, SLOT(chooseFont(const char *, const QFont&)), "panel_font_box");

			ConfigDialog::removeControl("Look", "Information panel syntax:");
			ConfigDialog::removeControl("Look", "Show vertical scrollbar in information panel");
			ConfigDialog::removeControl("Look", "Show information panel");
		ConfigDialog::removeControl("Look", "Information panel");

			ConfigDialog::removeControl("Look", "Font in panel", "panel_font_box");
		ConfigDialog::removeControl("Look", "Fonts");

				ConfigDialog::removeControl("Look", "Panel font color", "panel_font_color");
				ConfigDialog::removeControl("Look", "Panel background color", "panel_bg_color");
			ConfigDialog::removeControl("Look", "Main window");
		ConfigDialog::removeControl("Look", "Colors");
			ConfigDialog::removeControl("Look", "Display group tabs");
			ConfigDialog::removeControl("Look", "Show status button");

		ConfigDialog::removeControl("Look", "varOpts-expert");
		ConfigDialog::removeControl("Look", "varOpts-advanced");
		ConfigDialog::removeControl("Look", "varOpts-beginner");

		ConfigDialog::removeControl("Look", "Qt Theme");

		ConfigDialog::removeControl("ShortCuts", "Add user");
		ConfigDialog::removeControl("ShortCuts", "Configuration");
		ConfigDialog::removeControl("ShortCuts", "Show / hide users without description");
		ConfigDialog::removeControl("ShortCuts", "Show / hide offline users");
		ConfigDialog::removeControl("ShortCuts", "Search this user in directory");
		ConfigDialog::removeControl("ShortCuts", "View / edit user info");
		ConfigDialog::removeControl("ShortCuts", "Remove from userlist");
		ConfigDialog::removeControl("ShortCuts", "Define keys");

		ConfigDialog::unregisterSlotOnCreateTab("General", kaduslots, SLOT(onCreateTabGeneral()));
		ConfigDialog::unregisterSlotOnCreateTab("Look", kaduslots, SLOT(onCreateTabLook()));
		ConfigDialog::unregisterSlotOnApplyTab("Look", kaduslots, SLOT(onApplyTabLook()));
		ConfigDialog::unregisterSlotOnApplyTab("General", kaduslots, SLOT(onApplyTabGeneral()));

		ConfigDialog::removeControl("General", "Number of kept descriptions");
		ConfigDialog::removeControl("General", 0, "e_defaultstatus");
		ConfigDialog::removeControl("General", "On shutdown, set description:");
		ConfigDialog::removeControl("General", "discstatus");

		ConfigDialog::removeControl("General", "Use KaduParser to parse status");
		ConfigDialog::disconnectSlot("General", "On shutdown, set current description", SIGNAL(toggled(bool)), kaduslots, SLOT(updateStatus(bool)));
		ConfigDialog::removeControl("General", "On shutdown, set current description");
		ConfigDialog::removeControl("General", "Default status", "cb_defstatus");
		ConfigDialog::removeControl("General", "Status");

#ifdef DEBUG_ENABLED
		ConfigDialog::removeControl("General", "Debugging mask");
#endif
		ConfigDialog::removeControl("General", "Always show anonymous contacts with messages");
		ConfigDialog::removeControl("General", "Allow executing commands by parser");
		ConfigDialog::removeControl("General", "Show emoticons in panel");
		ConfigDialog::removeControl("General", "Check for updates");

		ConfigDialog::removeControl("General", "Private status");

		ConfigDialog::removeControl("General", "grid-expert");
		ConfigDialog::removeControl("General", "grid-advanced");
		ConfigDialog::removeControl("General", "grid-beginner");
		ConfigDialog::removeControl("General", "Set language:");
		ConfigDialog::removeControl("General", "Nick");
		ConfigDialog::removeControl("General", "Password");
		ConfigDialog::removeControl("General", "Uin");
		ConfigDialog::removeControl("General", "User data");

		ConfigDialog::removeTab("Network");
		ConfigDialog::removeTab("Look");
		ConfigDialog::removeTab("Chat");
		ConfigDialog::removeTab("ShortCuts");
		ConfigDialog::removeTab("General");

#ifdef Q_OS_MACX
		//na koniec przywracamy domy¶ln± ikonê, je¿eli tego nie zrobimy, to pozostanie bie¿±cy status
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

void Kadu::quitApplication()
{
	kdebugf();
	close(true);
}

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
		Action *a = KaduActions[*act];
		KaduActions.remove(*act);
		delete a;
	}
#endif
	delete selectedUsers;

	kdebugf2();
}

void Kadu::createMenu()
{
	kdebugf();
	menuBox = new QVBox(this, "menubarvbox");
	MenuBar = new QMenuBar(menuBox, "MenuBar");

	MainMenu = new QPopupMenu(MenuBar, "MainMenu");
	MainMenu->insertItem(icons_manager->loadIcon("ManageIgnored"), tr("Manage &ignored"), this, SLOT(manageIgnored()));
	MainMenu->insertItem(icons_manager->loadIcon("Configuration"), tr("&Configuration"), this, SLOT(configure()),HotKey::shortCutFromFile("ShortCuts", "kadu_configure"));
	MainMenu->insertSeparator();

	personalInfoMenuId=MainMenu->insertItem(icons_manager->loadIcon("PersonalInfo"), tr("Personal information"), this,SLOT(personalInfo()));
	MainMenu->insertSeparator();
	MainMenu->insertItem(icons_manager->loadIcon("LookupUserInfo"), tr("&Search user in directory"), this, SLOT(searchInDirectory()));
	MainMenu->insertItem(icons_manager->loadIcon("ImportExport"), tr("I&mport / Export userlist"), this, SLOT(importExportUserlist()));
	MainMenu->insertItem(icons_manager->loadIcon("AddUser"), tr("&Add user"), this, SLOT(addUserAction()),HotKey::shortCutFromFile("ShortCuts", "kadu_adduser"));
	MainMenu->insertSeparator();
	MainMenu->insertItem(icons_manager->loadIcon("HelpMenuItem"), tr("H&elp"), this, SLOT(help()));
	MainMenu->insertItem(icons_manager->loadIcon("AboutMenuItem"), tr("A&bout..."), this, SLOT(about()));
	MainMenu->insertSeparator();
	MainMenu->insertItem(icons_manager->loadIcon("HideKadu"), tr("&Hide Kadu"), this, SLOT(hideKadu()));
	MainMenu->insertItem(icons_manager->loadIcon("Exit"), tr("&Exit Kadu"), this, SLOT(quit()));

	MenuBar->insertItem(tr("&Kadu"), MainMenu);
	MainLayout->insertWidget(0, menuBox);

	icons_manager->registerMenu(MainMenu);
	icons_manager->registerMenuItem(MainMenu, tr("Manage &ignored"), "ManageIgnored");
	icons_manager->registerMenuItem(MainMenu, tr("&Configuration"), "Configuration");
	icons_manager->registerMenuItem(MainMenu, tr("Personal information"), "PersonalInfo");
	icons_manager->registerMenuItem(MainMenu, tr("&Search for users"), "LookupUserInfo");
	icons_manager->registerMenuItem(MainMenu, tr("I&mport / Export userlist"), "ImportExport");
	icons_manager->registerMenuItem(MainMenu, tr("&Add user"), "AddUser");
	icons_manager->registerMenuItem(MainMenu, tr("H&elp"), "HelpMenuItem");
	icons_manager->registerMenuItem(MainMenu, tr("A&bout..."), "AboutMenuItem");
	icons_manager->registerMenuItem(MainMenu, tr("&Hide Kadu"), "HideKadu");
	icons_manager->registerMenuItem(MainMenu, tr("&Exit Kadu"), "Exit");
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
	QIconSet icon;

	statusMenu = new QPopupMenu(this, "statusMenu");
	dockMenu = new QPopupMenu(this, "dockMenu");

	icons_manager->registerMenu(statusMenu);
	icons_manager->registerMenu(dockMenu);

	UserStatus *s = new GaduStatus();
	for (int i=0; i<8; ++i)
	{
		// je¿eli wywo³anie mia³o by postaæ setIndex(i)
		// to po sprawdzeniu, czy opis jest równy "" nast±pi³oby automatyczne
		// przyjêcie, ¿e status jest jednak bez opisu
		// co przyczyni³oby siê do z³ej ikonki
		s->setIndex(i, ".");
		pix = s->pixmap();
		icon = QIconSet(pix);
		QString statusName = qApp->translate("@default", UserStatus::name(i).ascii());
		statusMenu->insertItem(icon, statusName, i);
		dockMenu->insertItem(icon, statusName, i);

		icons_manager->registerMenuItem(statusMenu, statusName, UserStatus::toString(s->status(), s->hasDescription()));
		icons_manager->registerMenuItem(dockMenu, statusName, UserStatus::toString(s->status(), s->hasDescription()));
	}
	delete s;

	bool privateStatus=config_file.readBoolEntry("General", "PrivateStatus");
	statusMenu->insertSeparator();
	dockMenu->insertSeparator();
	statusMenu->insertItem(tr("Private"), 8);
	statusMenu->setItemChecked(8, privateStatus);
	dockMenu->insertItem(tr("Private"), 8);
	dockMenu->setItemChecked(8, privateStatus);

	statusMenu->setCheckable(true);
	dockMenu->setCheckable(true);
	statusMenu->setItemChecked(6, true);
	dockMenu->setItemChecked(6, true);

	statusMenu->setItemEnabled(7, false);
	dockMenu->setItemEnabled(7, false);

	connect(statusMenu, SIGNAL(activated(int)), this, SLOT(slotHandleState(int)));
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
		doc.parseHtml(KaduParser::parse(config_file.readEntry("Look", "PanelContents"), user));
		doc.convertUrlsToHtml();
		if((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle") != EMOTS_NONE && config_file.readBoolEntry("General", "ShowEmotPanel"))
		{
			InfoPanel->mimeSourceFactory()->addFilePath(emoticons->themePath());
			emoticons->expandEmoticons(doc, config_file.readColorEntry("Look", "InfoPanelBgColor"));
		}
		InfoPanel->setText(doc.generateHtml());
		if (config_file.readBoolEntry("General", "ShowEmotPanel"))
			InfoPanel->scrollToBottom();
		kdebugf2();
	}
}

void Kadu::currentChanged(UserListElement user)
{
	updateInformationPanel(user);
}

QMenuBar* Kadu::menuBar() const
{
	return MenuBar;
}

QPopupMenu* Kadu::mainMenu() const
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

void Kadu::setDocked(bool docked, bool dontHideOnClose1)
{
	Docked = docked;
	dontHideOnClose = dontHideOnClose1;
	if (config_file.readBoolEntry("General", "ShowAnonymousWithMsgs") || !Docked || dontHideOnClose)
	{
		Userbox->removeNegativeFilter(anonymousUsers);
		Userbox->applyNegativeFilter(anonymousUsersWithoutMessages);
	}
	else
	{
		Userbox->removeNegativeFilter(anonymousUsersWithoutMessages);
		Userbox->applyNegativeFilter(anonymousUsers);
	}
}

bool Kadu::docked() const
{
	return Docked;
}

void Kadu::show()
{
	QWidget::show();
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

	UserStatus status = gadu->status();
	status.setFriendsOnly(privateStatus);
	userStatusChanger->userStatusSet(status);

	statusMenu->setItemChecked(8, privateStatus);
}

void KaduSlots::onCreateTabGeneral()
{
	kdebugf();
	QLineEdit *e_password=ConfigDialog::getLineEdit("General", "Password");
	e_password->setEchoMode(QLineEdit::Password);
	e_password->setText(pwHash(config_file.readEntry("General", "Password")));

	QComboBox *cb_language= ConfigDialog::getComboBox("General", "Set language:");

	QDir locale(dataPath("kadu/translations/"), "kadu_*.qm");
	QStringList files=locale.entryList();

	FOREACH(file, files)
		*file = translateLanguage(qApp, (*file).mid(5, (*file).length()-8), true);
	cb_language->insertStringList(files);
	cb_language->setCurrentText(translateLanguage(qApp,
	config_file.readEntry("General", "Language", QTextCodec::locale()),true));

	QCheckBox *b_disconnectdesc= ConfigDialog::getCheckBox("General", "On shutdown, set description:");
	QLineEdit *e_disconnectdesc= ConfigDialog::getLineEdit("General", 0, "e_defaultstatus");
	e_disconnectdesc->setMaxLength(GG_STATUS_DESCR_MAXSIZE);
	e_disconnectdesc->setEnabled(b_disconnectdesc->isChecked());
	connect(b_disconnectdesc, SIGNAL(toggled(bool)), e_disconnectdesc, SLOT(setEnabled(bool)));

	int statusIndex = config_file.readNumEntry("General", "DefaultStatusIndex");

	int max = UserStatus::initCount();
	QComboBox* cb_defstatus = ConfigDialog::getComboBox("General", "Default status", "cb_defstatus");
	cb_defstatus->clear();
	for (int i = 0; i < max; ++i)
		cb_defstatus->insertItem(qApp->translate("@default", UserStatus::name(i).ascii()));
	cb_defstatus->insertItem(qApp->translate("@default", "Restore last status"));
	cb_defstatus->insertItem(qApp->translate("@default", "Restore last status (change Offline to Invisible)"));
	cb_defstatus->setCurrentItem(statusIndex);


	updateStatus(config_file.readBoolEntry("General", "DisconnectWithCurrentDescription"));
	kdebugf2();
}

void KaduSlots::onCreateTabLook()
{
	kdebugf();

	QComboBox *cb_qttheme=ConfigDialog::getComboBox("Look", "Qt Theme");
	static QStringList sl_themes = QStyleFactory::keys();//to jest dosyæ kosztowna czasowo operacja
	cb_qttheme->insertStringList(sl_themes);
	if(!sl_themes.contains(QApplication::style().name()))
		cb_qttheme->setCurrentText(tr("Unknown"));
	else
		cb_qttheme->setCurrentText(QApplication::style().name());

	updatePreview();
	kdebugf2();
}

void KaduSlots::onApplyTabLook()
{
	kdebugf();
	QString new_style = ConfigDialog::getComboBox("Look", "Qt Theme")->currentText();
	if (new_style != tr("Unknown") && new_style != QApplication::style().name())
	{
		QApplication::setStyle(new_style);
		config_file.writeEntry("Look", "QtStyle", new_style);
	}
	kadu->showdesc(config_file.readBoolEntry("Look", "ShowInfoPanel"));

	if (config_file.readBoolEntry("Look", "ShowStatusButton"))
		kadu->statusButton->show();
	else
		kadu->statusButton->hide();

	/* I od¶wie¿ okno Kadu */
	groups_manager->refreshTabBar();
	kadu->changeAppearance();
	UserBox::setColorsOrBackgrounds();
	chat_manager->changeAppearance();
	kdebugf2();
}

void KaduSlots::onApplyTabGeneral()
{
	kdebugf();
	QLineEdit *password = ConfigDialog::getLineEdit("General", "Password");
	config_file.writeEntry("General", "Password", pwHash(password->text()));

	gadu->changeID(ConfigDialog::getLineEdit("General", "Uin")->text());

	config_file.writeEntry("General", "DefaultStatusIndex",
		ConfigDialog::getComboBox("General", "Default status", "cb_defstatus")->currentItem());

	kadu->refreshPrivateStatusFromConfigFile();
	kadu->setCaption(tr("Kadu: %1").arg((UinType)config_file.readUnsignedNumEntry("General", "UIN")));

	QComboBox *cb_language= ConfigDialog::getComboBox("General", "Set language:");
	config_file.writeEntry("General", "Language", translateLanguage(qApp, cb_language->currentText(),false));

	//refresh
	kadu->setDocked(kadu->Docked, kadu->dontHideOnClose);

#ifdef DEBUG_ENABLED
	debug_mask=config_file.readNumEntry("General", "DEBUG_MASK");
	gg_debug_level=debug_mask | ~255;
#endif

	kdebugf2();
}

void KaduSlots::chooseColor(const char *name, const QColor& color)
{
	kdebugf();
	QLabel *preview= ConfigDialog::getLabel("Look", "<b>Text</b> preview", "preview_panel");
	if (QString(name)=="panel_bg_color")
		preview->setPaletteBackgroundColor(color);
	else if (QString(name)=="panel_font_color")
		preview->setPaletteBackgroundColor(color);
	else
		kdebugmf(KDEBUG_ERROR, "label '%s' not known\n", name);
	kdebugf2();
}

void KaduSlots::chooseFont(const char *name, const QFont& font)
{
	kdebugf();
	QLabel *preview= ConfigDialog::getLabel("Look", "<b>Text</b> preview", "preview_panel");
	if (QString(name)=="panel_font_box")
		preview->setFont(font);
	kdebugf2();
}

void KaduSlots::updatePreview()
{
	kdebugf();
	QLabel *preview= ConfigDialog::getLabel("Look", "<b>Text</b> preview", "preview_panel");
	preview->setFont(config_file.readFontEntry("Look", "PanelFont"));
	preview->setPaletteForegroundColor(config_file.readColorEntry("Look", "InfoPanelFgColor"));
	preview->setPaletteBackgroundColor(config_file.readColorEntry("Look", "InfoPanelBgColor"));
	preview->setAlignment(Qt::AlignLeft);
	kdebugf2();
}

void KaduSlots::updateStatus(bool current)
{
	kdebugf();
 	QCheckBox *cb_setdesc = ConfigDialog::getCheckBox("General", "On shutdown, set description:");
 	QLineEdit *e_defaultstatus = ConfigDialog::getLineEdit("General", 0, "e_defaultstatus");

	cb_setdesc->setEnabled(!current);
	e_defaultstatus->setEnabled(!current && cb_setdesc->isChecked());
	kdebugf2();
}

KaduSlots::KaduSlots(QObject *parent, const char *name) : QObject(parent, name)
{
}

void Kadu::resizeEvent(QResizeEvent *e)
{
//	kdebugm(KDEBUG_WARNING, "MenuBar->h4w(%d):%d, MenuBar->height():%d, MenuBar->sizeHint().height():%d\n", width(), MenuBar->heightForWidth(width()), MenuBar->height(), MenuBar->sizeHint().height());
	menuBox->setMaximumHeight(MenuBar->heightForWidth(width()));
	QWidget::resizeEvent(e);
}

/*void Kadu::moveEvent(QMoveEvent *e)
{
//	kdebugmf(KDEBUG_INFO, "%d %d %d %d\n", x(), y(), width(), height());
	QWidget::moveEvent(e);
}*/

void Kadu::setDefaultStatus()
{
	kdebugf();
	QString descr = defaultdescriptions.first();
	int statusIndex = config_file.readNumEntry("General", "DefaultStatusIndex");
	if (statusIndex == 7 || statusIndex == 8) //restore status
	{
		int lastStatusIndex = config_file.readNumEntry("General", "LastStatusIndex", UserStatus::index(Offline, false));
		QString lastStatusDescription = config_file.readEntry("General", "LastStatusDescription");
		if (statusIndex == 8 && UserStatus::isOffline(lastStatusIndex))
			lastStatusIndex = UserStatus::index(Invisible, !lastStatusDescription.isEmpty());
		status.setIndex(lastStatusIndex, lastStatusDescription);
	}
	else
		status.setIndex(statusIndex, descr);

	status.setFriendsOnly(config_file.readBoolEntry("General", "PrivateStatus"));
	userStatusChanger->userStatusSet(status);

	kdebugf2();
}

void Kadu::startupProcedure()
{
	kdebugf();

	// create toolbars in startupProcedure() to include actions from modules
	if (!TopDockArea->loadFromConfig(this))
	{
		ToolBar* toolbar = new ToolBar(this, "Kadu toolbar");
		TopDockArea->moveDockWindow(toolbar);
		TopDockArea->setAcceptDockWindow(toolbar, true);
		KaduActions.addDefaultActionsToToolbar(toolbar);
	}

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
	for(int i = 0; i < 8; ++i)
	{
		statusMenu->setItemChecked(i, false);
		dockMenu->setItemChecked(i, false);
	}
	statusMenu->setItemChecked(statusNr, true);
	dockMenu->setItemChecked(statusNr, true);
	statusMenu->setItemChecked(8, gadu->status().isFriendsOnly());
	dockMenu->setItemChecked(8, gadu->status().isFriendsOnly());

	statusButton->setText(qApp->translate("@default", gadu->status().name().ascii()));
	statusMenu->setItemEnabled(7, statusNr != 6);
	dockMenu->setItemEnabled(7, statusNr != 6);
	QPixmap pix = gadu->status().pixmap();
	QString iconName = gadu->status().toString();
	statusButton->setIconSet(QIconSet(pix));
	setMainWindowIcon(pix);

	emit statusPixmapChanged(pix, iconName);
}

void Kadu::readTokenValue(QPixmap tokenImage, QString &tokenValue)
{
	TokenDialog *td = new TokenDialog(tokenImage, 0, "token_dialog");

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
		setIcon(icon);
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
		QString id = QString::number(ev->number());
		if (ev->number() > 0 && gadu->ID() != id)
			chat_manager->openChat(gadu, userlist->byID("Gadu", id));
	}
	else
		QWidget::customEvent(e);
}

void Kadu::setOnline(const QString &description)
{
	UserStatus status;

	status.setStatus(gadu->status());
	status.setOnline(description);

	userStatusChanger->userStatusSet(status);
}

void Kadu::setBusy(const QString &description)
{
	UserStatus status;

	status.setStatus(gadu->status());
	status.setBusy(description);

	userStatusChanger->userStatusSet(status);
}

void Kadu::setInvisible(const QString &description)
{
	UserStatus status;

	status.setStatus(gadu->status());
	status.setInvisible(description);

	userStatusChanger->userStatusSet(status);
}

void Kadu::setOffline(const QString &description)
{
	UserStatus status;

	status.setStatus(gadu->status());
	status.setOffline(description);

	userStatusChanger->userStatusSet(status);
}
