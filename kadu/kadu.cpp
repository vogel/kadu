/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qwidget.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qstring.h>
#include <qtextstream.h>
#include <qtextcodec.h>
#include <qpoint.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qhbox.h>
#include <qfont.h>
#include <qcursor.h>
#include <qmessagebox.h>
#include <qprocess.h>
#include <qregexp.h>
#include <qtimer.h>
#include <qmenubar.h>
#include <qsplitter.h>
#include <qtoolbar.h>
#include <qtranslator.h>

#include <sys/file.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <stdlib.h>
#include <sys/stat.h>
//
#include "config_dialog.h"
#include "config_file.h"
#include "kadu.h"
#include "events.h"
#include "chat.h"
#include "search.h"
#include "dcc.h"
#include "expimp.h"
#include "userinfo.h"
#include "personal_info.h"
#include "register.h"
#include "about.h"
#include "ignore.h"
#include "hints.h"
#include "emoticons.h"
#include "history.h"
#include "pending_msgs.h"
#include "updates.h"
#include "password.h"
#include "tabbar.h"
#include "debug.h"
#include "gadu.h"
#include "modules.h"
#include "message_box.h"

#include "kadu-config.h"

#define GG_USER_OFFLINE	0x01
#define	GG_USER_NORMAL	0x03
#define GG_USER_BLOCKED	0x04

int muteitem;
struct gg_event* e;

QTime closestatusppmtime;
QTimer* blinktimer;
QPopupMenu* statusppm;
QPopupMenu* dockppm;
QPushButton* statusbutton;

UpdatesClass* uc;
int lockFileHandle;
QFile *lockFile;

QValueList<ToolBar::ToolButton> ToolBar::RegisteredToolButtons;
ToolBar* ToolBar::instance=NULL;

ToolBar::ToolBar(QMainWindow* parent) : QToolBar(parent, "main toolbar")
{
	setCloseMode(QDockWindow::Undocked);
	setLabel(tr("Main toolbar"));

	config_file.addVariable("General", "ToolBarHidden", false);
	if (config_file.readBoolEntry("General", "ToolBarHidden"))
		hide();

	setVerticallyStretchable(true);
	setHorizontallyStretchable(true);

	createControls();	
	instance=this;
}

ToolBar::~ToolBar()
{
	config_file.writeEntry("General", "ToolBarHidden", isHidden());
	instance=NULL;
}

void ToolBar::createControls()
{
	for(QValueList<ToolButton>::iterator j=RegisteredToolButtons.begin(); j!=RegisteredToolButtons.end(); j++)
		if ((*j).caption== "--separator--")
			addSeparator();
		else
			(*j).button = new QToolButton((*j).iconfile, (*j).caption,
				QString::null, (*j).receiver, (*j).slot, this, (*j).name);

	setStretchableWidget(new QWidget(this));
}

void ToolBar::registerSeparator(int position)
{
	if(instance!=NULL)
		instance->clear();

	ToolButton RToolButton;
	RToolButton.caption="--separator--";

	if ((RegisteredToolButtons.count()<(uint)(position+1)) || (position == -1))
		RegisteredToolButtons.append(RToolButton);
	else
		RegisteredToolButtons.insert(RegisteredToolButtons.at(position), RToolButton);

	if(instance!=NULL)
		instance->createControls();	
}

void ToolBar::registerButton(const QIconSet& iconfile, const QString& caption, 
			QObject* receiver, const char* slot, int position, const char* name)
{
	if(instance!=NULL)
		instance->clear();

	ToolButton RToolButton;

	RToolButton.iconfile= iconfile;
	RToolButton.caption= caption;
	RToolButton.receiver= receiver;
	RToolButton.slot= slot;
	RToolButton.position= position;
	RToolButton.name= name;

	if ((RegisteredToolButtons.count()<(uint)(position+1)) || (position == -1))
		RegisteredToolButtons.append(RToolButton);
	else
		RegisteredToolButtons.insert(RegisteredToolButtons.at(position), RToolButton);

	if(instance!=NULL)
		instance->createControls();
}

void ToolBar::unregisterButton(const char* name)
{
	if(instance!=NULL)
		instance->clear();

	for(QValueList<ToolButton>::iterator j=RegisteredToolButtons.begin(); j!=RegisteredToolButtons.end(); j++)
		if ((*j).name == name)
		{
			RegisteredToolButtons.remove(j);
			break;
		}

	if(instance!=NULL)
		instance->createControls();		
}

QToolButton* ToolBar::getButton(const char* name)
{
	for(QValueList<ToolButton>::iterator j=RegisteredToolButtons.begin(); j!=RegisteredToolButtons.end(); j++)
		if ((*j).name == name)
			return (*j).button;
	kdebugm(KDEBUG_WARNING, "return NULL\n");
	return NULL;
}

void Kadu::gotUpdatesInfo(const QByteArray &data, QNetworkOperation *op) {
	char buf[32];
//	int i;
	QString newestversion;

	if (config_file.readBoolEntry("General", "CheckUpdates"))
	{
		if (data.size() > 31) {
			kdebugm(KDEBUG_WARNING, "Kadu::gotUpdatesInfo(): cannot obtain update info\n");
			delete uc;
			return;
		}
		for (unsigned i = 0; i < data.size(); i++)
			buf[i] = data[i];
		buf[data.size()] = 0;
		newestversion = buf;

		kdebugm(KDEBUG_INFO, "Kadu::gotUpdatesInfo(): %s\n", buf);

		if (uc->ifNewerVersion(newestversion))
			QMessageBox::information(this, tr("Update information"),
				tr("The newest Kadu version is %1").arg(newestversion), QMessageBox::Ok);
	}
	UpdateChecked=true;
	delete uc;
}

void Kadu::keyPressEvent(QKeyEvent *e) {
	if (e->key() == Key_Escape && Docked) {
		kdebugm(KDEBUG_INFO, "Kadu::keyPressEvent(Key_Escape): Kadu hide\n");
		hide();
	}
	else if (HotKey::shortCut(e,"ShortCuts", "kadu_deleteuser"))
	{
		if (Userbox->getSelectedAltNicks().count())
			deleteUsers();
	}
	else if (HotKey::shortCut(e,"ShortCuts", "kadu_persinfo"))
	{
		if (Userbox->getSelectedAltNicks().count() == 1)
			showUserInfo();
	}
	else if (HotKey::shortCut(e,"ShortCuts", "kadu_viewhistory"))
		viewHistory();
	else if (HotKey::shortCut(e,"ShortCuts", "kadu_searchuser"))
		lookupInDirectory();
	else if (HotKey::shortCut(e,"ShortCuts", "kadu_showinactive"))
		Userbox->showHideInactive();
	else if (HotKey::shortCut(e,"ShortCuts", "kadu_sendfile"))
		sendFile();
	else if (HotKey::shortCut(e,"ShortCuts", "kadu_configure"))
		configure();
	else if (HotKey::shortCut(e,"ShortCuts", "kadu_modulesmanager"))
		modules_manager->showDialog();

	emit keyPressed(e);

	QWidget::keyPressEvent(e);
}

/* a monstrous constructor so Kadu would take longer to start up */
Kadu::Kadu(QWidget *parent, const char *name) : QMainWindow(parent, name)
{
	UpdateChecked = false;
	Docked = false;
	Autohammer = false;
	ShowMainWindowOnStart = true;

	KaduSlots *kaduslots=new KaduSlots();
	UinType myUin=config_file.readNumEntry("General", "UIN");

	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "General"));
	ConfigDialog::addHGroupBox("General", "General", QT_TRANSLATE_NOOP("@default", "User data"));
	ConfigDialog::addLineEdit("General", "User data", QT_TRANSLATE_NOOP("@default", "Uin"), "UIN", "0");
	ConfigDialog::addLineEdit("General", "User data", QT_TRANSLATE_NOOP("@default", "Password"), "Password", "");
	ConfigDialog::addLineEdit("General", "User data", QT_TRANSLATE_NOOP("@default", "Nick"), "Nick", tr("Me"));
	ConfigDialog::addComboBox("General", "General", QT_TRANSLATE_NOOP("@default", "Set language:"));
	ConfigDialog::addGrid("General", "General", "grid", 2);
	ConfigDialog::addCheckBox("General", "grid", QT_TRANSLATE_NOOP("@default", "Log messages"), "Logging", true);
	ConfigDialog::addCheckBox("General", "grid", QT_TRANSLATE_NOOP("@default", "Restore window geometry"), "SaveGeometry", true, QT_TRANSLATE_NOOP("@default", "Remember window size and position between startups"));
	ConfigDialog::addCheckBox("General", "grid", QT_TRANSLATE_NOOP("@default", "Check for updates"), "CheckUpdates", true, QT_TRANSLATE_NOOP("@default", "Automatically checks whether a new version is available"));

	ConfigDialog::addCheckBox("General", "grid", QT_TRANSLATE_NOOP("@default", "Private status"), "PrivateStatus", false, QT_TRANSLATE_NOOP("@default", "When enabled, you're visible only to users on your list"));

	ConfigDialog::addCheckBox("General", "grid", QT_TRANSLATE_NOOP("@default", "Show emoticons in panel"), "ShowEmotPanel", false);
	ConfigDialog::addCheckBox("General", "grid", QT_TRANSLATE_NOOP("@default", "Show emoticons in history"), "ShowEmotHist", false);

	ConfigDialog::addVGroupBox("General", "General", "Status");
	ConfigDialog::addComboBox("General", "Status", QT_TRANSLATE_NOOP("@default", "Default status"), "", "cb_defstatus");
	ConfigDialog::addHBox("General", "Status", "discstatus");
	ConfigDialog::addCheckBox("General", "discstatus", QT_TRANSLATE_NOOP("@default", "On shutdown, set description:"), "DisconnectWithDescription", false);
	ConfigDialog::addLineEdit("General", "discstatus", "", "DisconnectDescription", "", "", "e_defaultstatus");

	ConfigDialog::registerSlotOnCreate(kaduslots, SLOT(onCreateConfigDialog()));
	ConfigDialog::registerSlotOnApply(kaduslots, SLOT(onDestroyConfigDialog()));

	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "ShortCuts"));
	ConfigDialog::addVGroupBox("ShortCuts", "ShortCuts", QT_TRANSLATE_NOOP("@default", "Define keys"));
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Remove from userlist"), "kadu_deleteuser", "Del");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "View/edit user info"), "kadu_persinfo", "Ins");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "View history"), "kadu_viewhistory", "Ctrl+H");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Lookup in directory"), "kadu_searchuser", "Ctrl+F");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Show / hide inactive users"), "kadu_showinactive", "F9");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Send file"), "kadu_sendfile", "F8");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Configuration"), "kadu_configure", "F2");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Add user"), "kadu_adduser", "Ctrl+N");

	Chat::initModule();
	UserBox::initModule();
	History::initModule();
	HintManager::initModule();
	EventConfigSlots::initModule();
	GaduProtocol::initModule();

	//zaladowanie wartosci domyslnych (pierwsze uruchomienie)
	config_file.addVariable("General", "UserBoxHeight", 300);
	config_file.addVariable("General", "DescriptionHeight", 60);

	QFontInfo info(qApp->font());
	QFont def_font(info.family(),info.pointSize());
	config_file.addVariable("Look", "UserboxFont", &def_font);
	config_file.addVariable("Look", "UserboxDescFont", &def_font);

	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "Look"));

	ConfigDialog::addCheckBox("Look", "varOpts", QT_TRANSLATE_NOOP("@default", "Show vertical scrollbar in information panel"), "PanelVerticalScrollbar", true);
	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Colors"));
		ConfigDialog::addVGroupBox("Look", "Colors", QT_TRANSLATE_NOOP("@default", "Main window"));
			ConfigDialog::addHBox("Look", "Main window", "panel_bg_color_box");
				ConfigDialog::addLabel("Look", "panel_bg_color_box", QT_TRANSLATE_NOOP("@default", "Panel background color"));
				ConfigDialog::addColorButton("Look", "panel_bg_color_box", "", "InfoPanelBgColor", config_file.readColorEntry("Look","InfoPanelBgColor"), "", "panel_bg_color");
			ConfigDialog::addHBox("Look", "Main window", "panel_font_color_box");
				ConfigDialog::addLabel("Look", "panel_font_color_box", QT_TRANSLATE_NOOP("@default", "Panel font color"));
				ConfigDialog::addColorButton("Look", "panel_font_color_box", "", "InfoPanelFgColor", config_file.readColorEntry("Look","InfoPanelFgColor"), "", "panel_font_color");

	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Fonts"));
		ConfigDialog::addSelectFont("Look", "Fonts", QT_TRANSLATE_NOOP("@default", "Font in panel"), "PanelFont", def_font.toString(), "", "panel_font_box");

	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Information panel"));
		ConfigDialog::addCheckBox("Look", "Information panel", QT_TRANSLATE_NOOP("@default", "Show information panel"), "ShowInfoPanel", true);
		ConfigDialog::addTextEdit("Look", "Information panel", QT_TRANSLATE_NOOP("@default", "Information panel syntax:"), "PanelContents", "[#%u][, %f] %r [- %d] [ (%i)]", QT_TRANSLATE_NOOP("@default", "Syntax: %s - status, %d - description, %i - ip, %n - nick, %a - altnick, %f - frist name\n%r - surname, %m - mobile, %u - uin, %g - group, %o - return _space_ if user doesn't have us in userlist\n%v - revDNS, %p - port %e - email\nIf you leave blank, default settings will be used"));

	ConfigDialog::connectSlot("Look", "", SIGNAL(changed(const char *, const QColor&)), kaduslots, SLOT(chooseColor(const char *, const QColor&)), "panel_bg_color");
	ConfigDialog::connectSlot("Look", "", SIGNAL(changed(const char *, const QColor&)), kaduslots, SLOT(chooseColor(const char *, const QColor&)), "panel_font_color");

	ConfigDialog::connectSlot("Look", "Font in panel", SIGNAL(changed(const char *, const QFont&)),kaduslots, SLOT(chooseFont(const char *, const QFont&)), "panel_font_box");


	closestatusppmtime.start();

	// blinktimer = NULL; zamieniamy na(powod: patrz plik events.cpp)
	blinktimer = NULL;

	/* blinker */
	BlinkOn = false;
	DoBlink = false;

	/* another API change, another hack */
	memset(&loginparams, 0, sizeof(loginparams));
	loginparams.async = 1;

	loadGeometry(this, "General", "Geometry", 0, 0, 145, 465);

	if (config_file.readBoolEntry("Hints", "Hints"))
		hintmanager = new HintManager();

	/* read the userlist */
	userlist.readFromFile();

	readIgnored();

	/* a newbie? */

	if (myUin)
		setCaption(tr("Kadu: %1").arg(myUin));

	pending.loadFromFile();
	
	QVBox *vbox=new QVBox(this);
	setCentralWidget(vbox);

	QSplitter *split = new QSplitter(Qt::Vertical, vbox);

	QHBox *hbox1 = new QHBox(split);

	/* initialize group tabbar */
	GroupBar = new KaduTabBar(hbox1, "groupbar");
	GroupBar->setShape(QTabBar::RoundedBelow);
	GroupBar->addTab(new QTab(tr("All")));
	GroupBar->setFont(QFont(config_file.readFontEntry("Look", "UserboxFont").family(), config_file.readFontEntry("Look", "UserboxFont").pointSize(),75));
	connect(GroupBar, SIGNAL(selected(int)), this, SLOT(groupTabSelected(int)));

	/* connect userlist signals */
	connect(&userlist, SIGNAL(modified()), this, SLOT(userListModified()));
	connect(&userlist, SIGNAL(statusModified(UserListElement *)), this, SLOT(userListStatusModified(UserListElement *)));
	connect(&userlist, SIGNAL(userAdded(const UserListElement&)),this,SLOT(userListUserAdded(const UserListElement&)));

	/* initialize and configure userbox */
	Userbox = new UserBox(hbox1, "userbox");
	if (config_file.readBoolEntry("Look", "MultiColumnUserbox"))
		Userbox->setColumnMode(QListBox::FitToWidth);
	Userbox->setPaletteBackgroundColor(config_file.readColorEntry("Look", "UserboxBgColor"));
	Userbox->setPaletteForegroundColor(config_file.readColorEntry("Look", "UserboxFgColor"));
	Userbox->QListBox::setFont(config_file.readFontEntry("Look", "UserboxFont"));
	Userbox->setMinimumWidth(20);

	hbox1->setStretchFactor(GroupBar, 1);
	hbox1->setStretchFactor(Userbox, 100);

	/* add all users to userbox */
	setActiveGroup("");

	// dodanie przyciskow do paska narzedzi
	ToolBar::registerButton(icons_manager.loadIcon("ShowHideInactiveUsers"), tr("Show / hide inactive users"), Userbox, SLOT(showHideInactive()));
	ToolBar::registerButton(icons_manager.loadIcon("Configuration"), tr("Configuration"), this, SLOT(configure()));
	ToolBar::registerSeparator();
	ToolBar::registerButton(icons_manager.loadIcon("History"), tr("View history"), this, SLOT(viewHistory()));
	ToolBar::registerButton(icons_manager.loadIcon("EditUserInfo"), tr("View/edit user info"), this, SLOT(showUserInfo()));
	ToolBar::registerButton(icons_manager.loadIcon("LookupUserInfo"), tr("Lookup in directory"), this, SLOT(lookupInDirectory()));
	ToolBar::registerSeparator();
	ToolBar::registerButton(icons_manager.loadIcon("AddUser"), tr("Add user"), this, SLOT(addUserAction()));

	// popupmenu
	UserBox::userboxmenu->addItem(tr("Open chat window") ,this, SLOT(openChat()));

	UserBox::userboxmenu->addItem("SendFile", tr("Send file"), this,
		SLOT(sendFile()), HotKey::shortCutFromFile("ShortCuts", "kadu_sendfile"));

	UserBox::userboxmenu->insertSeparator();
	UserBox::userboxmenu->addItem(tr("Ignore user"), this, SLOT(ignoreUser()));
	UserBox::userboxmenu->addItem(tr("Block user"), this, SLOT(blockUser()));
	UserBox::userboxmenu->addItem(tr("Notify about user"), this, SLOT(notifyUser()));
	UserBox::userboxmenu->addItem(tr("Offline to user"), this, SLOT(offlineToUser()));

	UserBox::userboxmenu->insertSeparator();
	UserBox::userboxmenu->addItem("RemoveFromUserlist", tr("Remove from userlist"), this, SLOT(deleteUsers()),HotKey::shortCutFromFile("ShortCuts", "kadu_deleteuser"));
	UserBox::userboxmenu->addItem("ClearHistory", tr("Clear history"), this, SLOT(deleteHistory()));
	UserBox::userboxmenu->addItem("History", tr("View history"),this,SLOT(viewHistory()),HotKey::shortCutFromFile("ShortCuts", "kadu_viewhistory"));
	UserBox::userboxmenu->addItem("EditUserInfo", tr("View/edit user info"), this, SLOT(showUserInfo()),HotKey::shortCutFromFile("ShortCuts", "kadu_persinfo"));
	UserBox::userboxmenu->addItem("LookupUserInfo", tr("Lookup in directory"), this, SLOT(lookupInDirectory()),HotKey::shortCutFromFile("ShortCuts", "kadu_searchuser"));
	UserBox::userboxmenu->insertSeparator();
	UserBox::userboxmenu->addItem(tr("About..."), this, SLOT(about()));

	connect(UserBox::userboxmenu, SIGNAL(popup()), this, SLOT(popupMenu()));
	connect(Userbox, SIGNAL(rightButtonClicked(QListBoxItem *, const QPoint &)),
		UserBox::userboxmenu, SLOT(show(QListBoxItem *)));
	//

	connect(Userbox, SIGNAL(doubleClicked(QListBoxItem *)), this, SLOT(sendMessage(QListBoxItem *)));
	connect(Userbox, SIGNAL(returnPressed(QListBoxItem *)), this, SLOT(sendMessage(QListBoxItem *)));

	connect(Userbox, SIGNAL(mouseButtonClicked(int, QListBoxItem *, const QPoint &)),
		this, SLOT(mouseButtonClicked(int, QListBoxItem *)));
	connect(Userbox, SIGNAL(currentChanged(QListBoxItem *)), this, SLOT(currentChanged(QListBoxItem *)));

	/* guess what */
	createMenu();
	createStatusPopupMenu();

	connect(statusppm, SIGNAL(aboutToHide()), this, SLOT(statusMenuAboutToHide()));

	dockppm->insertSeparator();
	dockppm->insertItem(icons_manager.loadIcon("Exit"), tr("&Exit Kadu"), 9);

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
	if (!config_file.readBoolEntry("Look", "ShowInfoPanel"))
		InfoPanel->QWidget::hide();
	QObject::connect(&userlist, SIGNAL(dnsNameReady(UinType)), this, SLOT(infopanelUpdate(UinType)));

	statusbutton = new QPushButton(QIconSet(icons_manager.loadIcon("Offline")), tr("Offline"), vbox, "statusbutton");
	statusbutton->setPopup(statusppm);
	
	if (!config_file.readBoolEntry("Look", "ShowStatusButton"))
		statusbutton->hide();

	QValueList<int> splitsizes;

	splitsizes.append(config_file.readNumEntry("General", "UserBoxHeight"));
	splitsizes.append(config_file.readNumEntry("General", "DescriptionHeight"));

	split->setSizes(splitsizes);

//	tworzymy pasek narzedziowy
	createToolBar();
	if (config_file.readEntry("General", "DockWindows") != QString::null) {
		QString dockwindows=config_file.readEntry("General", "DockWindows").replace(QRegExp("\\\\n"), "\n");
		QTextStream stream(&dockwindows, IO_ReadOnly);
		stream >> *this;
		}

	refreshGroupTabBar();
	int configTab = config_file.readNumEntry( "Look", "CurrentGroupTab" );
	if ( configTab >= 0 && configTab < GroupBar -> count() )
		((QTabBar*) GroupBar) -> setCurrentTab( configTab );

	connect(gadu, SIGNAL(connecting()), this, SLOT(connecting()));
	connect(gadu, SIGNAL(connected()), this, SLOT(connected()));
	connect(gadu, SIGNAL(dccSetupFailed()), this, SLOT(dccSetupFailed()));
	connect(gadu, SIGNAL(disconnected()), this, SLOT(disconnected()));
	connect(gadu, SIGNAL(error(GaduError)), this, SLOT(error(GaduError)));
	connect(gadu, SIGNAL(statusChanged(int)), this, SLOT(setCurrentStatus(int)));

	dccsock = NULL;
}

void Kadu::createToolBar()
{
	new ToolBar(this);
	setRightJustification(true);
//	setDockEnabled(Qt::DockBottom, false);
	setAppropriate(ToolBar::instance, true);
}

void Kadu::popupMenu()
{
	UserList users;
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)//to siê zdarza...
		return;
	users = activeUserBox->getSelectedUsers();
	UserListElement user = (*users.begin());

	bool isOurUin=users.containsUin(config_file.readNumEntry("General", "UIN"));
	
	int sendfile = UserBox::userboxmenu->getItem(tr("Send file"));

	if (dccSocketClass::count >= 8 && users.count() != 1) {
		UserBox::userboxmenu->setItemEnabled(sendfile, false);
		}
	if (users.count() == 1 && (config_file.readBoolEntry("Network", "AllowDCC") &&
		(user.status == GG_STATUS_AVAIL || user.status == GG_STATUS_AVAIL_DESCR ||
		user.status == GG_STATUS_BUSY || user.status == GG_STATUS_BUSY_DESCR)) && !isOurUin) {
			UserBox::userboxmenu->setItemEnabled(sendfile, true);
			}
		else {
			UserBox::userboxmenu->setItemEnabled(sendfile, false);
			}

	int ignoreuseritem= UserBox::userboxmenu->getItem(tr("Ignore user"));
	int blockuseritem= UserBox::userboxmenu->getItem(tr("Block user"));
	int notifyuseritem= UserBox::userboxmenu->getItem(tr("Notify about user"));
	int offlinetouseritem= UserBox::userboxmenu->getItem(tr("Offline to user"));

	if (!user.uin || isOurUin) {
		UserBox::userboxmenu->setItemEnabled(ignoreuseritem, false);
		UserBox::userboxmenu->setItemEnabled(blockuseritem, false);
		UserBox::userboxmenu->setItemEnabled(notifyuseritem, false);
		UserBox::userboxmenu->setItemEnabled(offlinetouseritem, false);
		}
	else {
		UinsList uins;
		uins = activeUserBox->getSelectedUins();
		if (isIgnored(uins))
			UserBox::userboxmenu->setItemChecked(ignoreuseritem, true);
		if (user.blocking)
			UserBox::userboxmenu->setItemChecked(blockuseritem, true);
		UserBox::userboxmenu->setItemEnabled(offlinetouseritem, config_file.readBoolEntry("General", "PrivateStatus"));
		if (user.offline_to_user)
			UserBox::userboxmenu->setItemChecked(offlinetouseritem, true);
		UserBox::userboxmenu->setItemEnabled(notifyuseritem, config_file.readBoolEntry("Notify", "NotifyStatusChange") && !config_file.readBoolEntry("Notify", "NotifyAboutAll"));
		if (user.notify)
			UserBox::userboxmenu->setItemChecked(notifyuseritem, true);
		}

	int deletehistoryitem = UserBox::userboxmenu->getItem(tr("Clear history"));
	int historyitem = UserBox::userboxmenu->getItem(tr("View history"));
	int searchuser = UserBox::userboxmenu->getItem(tr("Lookup in directory"));
	if (!user.uin || isOurUin) {
		UserBox::userboxmenu->setItemEnabled(deletehistoryitem, false);
		UserBox::userboxmenu->setItemEnabled(historyitem, false);
		}
	if (users.count() != 1 || !user.uin)
		UserBox::userboxmenu->setItemEnabled(searchuser, false);
	if (users.count() != 1)
		UserBox::userboxmenu->setItemEnabled(UserBox::userboxmenu->getItem(tr("View/edit user info")), false);
	if (!user.uin || isOurUin)
		UserBox::userboxmenu->setItemEnabled(UserBox::userboxmenu->getItem(tr("Open chat window")), false);
}


void Kadu::configure()
{
	ConfigDialog::showConfigDialog(qApp);
}

void Kadu::viewHistory() {
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)
		return;
	UinsList uins= activeUserBox->getSelectedUins();
	if (uins.count()) {
		History *hb = new History(uins);
		hb->show();
	}
}

void Kadu::sendFile()
{
	if (config_file.readBoolEntry("Network", "AllowDCC"))
		if (config_dccip.isIp4Addr()) {
			struct gg_dcc *dcc_new;
			UserBox *activeUserBox=UserBox::getActiveUserBox();
			UserList users;
			if (activeUserBox==NULL)
				return;
			users= activeUserBox->getSelectedUsers();
			if (users.count() != 1)
				return;
			UserListElement user = (*users.begin());
			if (user.port >= 10) {
				if ((dcc_new = gg_dcc_send_file(htonl(user.ip.ip4Addr()), user.port,
					config_file.readNumEntry("General", "UIN"), user.uin)) != NULL) {
					dccSocketClass *dcc = new dccSocketClass(dcc_new);
					connect(dcc, SIGNAL(dccFinished(dccSocketClass *)), this,
						SLOT(dccFinished(dccSocketClass *)));
					dcc->initializeNotifiers();
					}
				}
			else
				gg_dcc_request(sess, user.uin);
			}
}

void Kadu::lookupInDirectory() {
	SearchDialog *sd;
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	UserList users;
	if (activeUserBox==NULL)
		return;
	users = activeUserBox->getSelectedUsers();
	if (users.count() == 1) {
		sd = new SearchDialog(0, tr("User info"), userlist.byAltNick((*users.begin()).altnick).uin);
		sd->show();
		sd->firstSearch();
	}
	else {
		sd = new SearchDialog();
		sd->show();
	}
}

void Kadu::showUserInfo() {
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	UserList users;
	if (activeUserBox==NULL)
		return;
	users = activeUserBox->getSelectedUsers();
	if (users.count() == 1)
	{
		UserInfo *ui = new UserInfo("user info", 0, (*users.begin()).altnick);
		ui->show();
	}
}

void Kadu::deleteUsers()
{
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)
		return;
	QStringList  users = activeUserBox->getSelectedAltNicks();
	removeUser(users, false);
	if (!Userbox->isSelected(Userbox->currentItem()))
		InfoPanel->setText("");
}

void Kadu::personalInfo()
{
	PersonalInfoDialog *pid = new PersonalInfoDialog();
	pid->show();
}

void Kadu::addUserAction() {
	UserInfo *ui = new UserInfo("add user", 0, QString::null, true);
	ui->show();
}

void Kadu::deleteHistory()
{
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)
		return;
	history.removeHistory(activeUserBox->getSelectedUins());
}

void Kadu::manageIgnored()
{
	Ignored *ign = new Ignored(0, "ignored");
	ign->show();
}

void Kadu::openChat()
{
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)
		return;
	chat_manager->openPendingMsgs(activeUserBox->getSelectedUins());
}

void Kadu::searchInDirectory()
{
	SearchDialog *sd = new SearchDialog();
	sd->show();
}

void Kadu::help()
{
	QString link;
/*	if (QFile::exists(QString(DOCDIR) + "/index_doc.html"))
		link = QString(DOCDIR) + "/index_doc.html";
	else*/
		link = "http://kadu.net/doc.php";
	openWebBrowser(link);
}

void Kadu::about()
{
	(new About(this, "about_window"))->show();
}

void Kadu::remindPassword()
{
	(new RemindPassword())->start();
}

void Kadu::changePassword()
{
	(new ChangePassword())->show();
}

void Kadu::registerUser()
{
	(new Register())->show();
}

void Kadu::unregisterUser()
{
	(new Unregister())->show();
}

void Kadu::quit()
{
	kdebugf();
	close(true);
}

void Kadu::importExportUserlist()
{
	(new UserlistImportExport(this, "userlist_import_export"))->show();
}


void Kadu::hideKadu()
{
	if (Docked)
		close();
}

void Kadu::ignoreUser()
{
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)
		return;
	UinsList uins = activeUserBox->getSelectedUins();
	if (isIgnored(uins))
		delIgnored(uins);
	else
		addIgnored(uins);
	writeIgnored();
}

void Kadu::blockUser()
{
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)
		return;
	UserListElement *puser = &userlist.byAltNick((*activeUserBox->getSelectedUsers().begin()).altnick);
	puser->blocking = !puser->blocking;
	gg_remove_notify_ex(sess, puser->uin, puser->blocking ? GG_USER_NORMAL : GG_USER_BLOCKED);
	gg_add_notify_ex(sess, puser->uin, puser->blocking ? GG_USER_BLOCKED : GG_USER_NORMAL);
	userlist.writeToFile();
}

void Kadu::notifyUser()
{
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)
		return;
	UserListElement *puser = &userlist.byAltNick((*activeUserBox->getSelectedUsers().begin()).altnick);
	puser->notify = !puser->notify;
	userlist.writeToFile();
}

void Kadu::offlineToUser()
{
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)
		return;
	UserListElement *puser = &userlist.byAltNick((*activeUserBox->getSelectedUsers().begin()).altnick);
	puser->offline_to_user = !puser->offline_to_user;
	gg_remove_notify_ex(sess, puser->uin, puser->offline_to_user ? GG_USER_NORMAL : GG_USER_OFFLINE);
	gg_add_notify_ex(sess, puser->uin, puser->offline_to_user ? GG_USER_OFFLINE : GG_USER_NORMAL);
	userlist.writeToFile();
}

void Kadu::changeAppearance() {
	kdebugf();

	Userbox->setPaletteBackgroundColor(config_file.readColorEntry("Look", "UserboxBgColor"));
	Userbox->setPaletteForegroundColor(config_file.readColorEntry("Look", "UserboxFgColor"));
	Userbox->QListBox::setFont(config_file.readFontEntry("Look", "UserboxFont"));

	GroupBar->setFont(QFont(config_file.readFontEntry("Look", "UserboxFont").family(), config_file.readFontEntry("Look", "UserboxFont").pointSize(),75));

	InfoPanel->setPaletteBackgroundColor(config_file.readColorEntry("Look", "InfoPanelBgColor"));
	InfoPanel->setPaletteForegroundColor(config_file.readColorEntry("Look", "InfoPanelFgColor"));
	InfoPanel->QTextEdit::setFont(config_file.readFontEntry("Look", "PanelFont"));

	if (config_file.readBoolEntry("Look", "PanelVerticalScrollbar"))
		InfoPanel->setVScrollBarMode(QScrollView::Auto);
	else
		InfoPanel->setVScrollBarMode(QScrollView::AlwaysOff);
}

void Kadu::currentChanged(QListBoxItem *item) {
	if (!item || !item->isSelected())
		return;

	kdebugm(KDEBUG_INFO, "Kadu::currentChanged(): %s\n", (const char *)item->text().local8Bit());

	if (config_file.readBoolEntry("Look", "ShowInfoPanel"))
	{
		HtmlDocument doc;
		doc.parseHtml(parse(config_file.readEntry("Look", "PanelContents"), userlist.byAltNick(item->text())));
		doc.convertUrlsToHtml();

		if((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle")!=EMOTS_NONE && config_file.readBoolEntry("General", "ShowEmotPanel"))
		{
			InfoPanel->mimeSourceFactory()->addFilePath(emoticons->themePath());
			emoticons->expandEmoticons(doc, config_file.readColorEntry("Look", "InfoPanelBgColor"));
		}
		InfoPanel->setText(doc.generateHtml());
		if (config_file.readBoolEntry("General", "ShowEmotPanel"))
			InfoPanel->scrollToBottom();
	}
}

void Kadu::refreshGroupTabBar()
{
	if (!config_file.readBoolEntry("Look", "DisplayGroupTabs"))
	{
		GroupBar->hide();
		return;
	}
	/* budujemy listê grup */
	QValueList<QString> group_list;
	for (UserList::ConstIterator i = userlist.begin(); i != userlist.end(); i++)
	{
		QString groups = (*i).group();
		QString group;
		for (int g = 0; (group = groups.section(',' ,g ,g)) != ""; g++)
			if(!group_list.contains(group))
				group_list.append(group);
	}
	kdebugm(KDEBUG_INFO, "%i groups found\n",group_list.count());
	//
	if (group_list.count() == 0)
	{
		GroupBar->hide();
		setActiveGroup("");
		return;
	}
	/* usuwamy wszystkie niepotrzebne zakladki - od tylu,
	   bo indeksy sie przesuwaja po usunieciu */
	for (int i = GroupBar->count() - 1; i >= 1; i--)
		if(!group_list.contains(GroupBar->tabAt(i)->text()))
			GroupBar->removeTab(GroupBar->tabAt(i));
	/* dodajemy nowe zakladki */
	for (unsigned int i = 0; i < group_list.count(); i++)
		{
		bool createNewTab = true;
		for (int j = 0; j < GroupBar->count(); j++)
			if (GroupBar->tabAt(j)->text() == group_list[i])
				createNewTab = false;
		if(createNewTab)
			GroupBar->addTab(new QTab(group_list[i]));
		}
	kdebugm(KDEBUG_INFO, "%i group tabs\n", GroupBar->count());
	GroupBar->show();
	/* odswiezamy - dziala tylko jesli jest widoczny */
	GroupBar->update();
}

void Kadu::setActiveGroup(const QString& group)
{
	Userbox->clearUsers();
	for (UserList::ConstIterator i = userlist.begin(); i != userlist.end(); i++)
	{
		bool belongsToGroup;
		if (group == "")
			belongsToGroup = true;
		else
		{
			belongsToGroup = false;
			QString user_groups = (*i).group();
			QString user_group;
			for (int g = 0; (user_group = user_groups.section(',',g,g)) != ""; g++)
				if (user_group == group)
					belongsToGroup = true;
		}
		if (belongsToGroup && (!(*i).anonymous || !Docked))
			Userbox->addUser((*i).altnick);
	}
	UserBox::all_refresh();
}

void Kadu::groupTabSelected(int id)
{
	if (id == 0)
		setActiveGroup("");
	else
		setActiveGroup(GroupBar->tab(id)->text());
}

void Kadu::userListModified()
{
	refreshGroupTabBar();
}

void Kadu::userListStatusModified(UserListElement *user)
{
	kdebugm(KDEBUG_FUNCTION_START, "Kadu::userListStatusModified(): %d\n", user->uin);
	if ((user->status == GG_STATUS_NOT_AVAIL)
		|| (user->status == GG_STATUS_NOT_AVAIL_DESCR))
		InfoPanel->setText("");
	chat_manager->refreshTitlesForUin(user->uin);
}

void Kadu::removeUser(QStringList &users, bool permanently = false)
{
	if(QMessageBox::warning(kadu, "Kadu",
		tr("Selected users will be deleted. Are you sure?"),
		tr("&Yes"),tr("&No"))!=0)
		return;

	unsigned int i;

	for (i = 0; i < users.count(); i++)
		UserBox::all_removeUser(users[i]);
	UserBox::all_refresh();

	for (i = 0; i < users.count(); i++) {
		UserListElement user = userlist.byAltNick(users[i]);
		if (!user.anonymous && user.uin)
			gg_remove_notify(sess, user.uin);
		userlist.removeUser(user.altnick);
		}

	userlist.writeToFile();
	refreshGroupTabBar();
}

void Kadu::blink() {
	int i;
	QPixmap pix;

	if (!DoBlink && socket_active)
	{
		setCurrentStatus(loginparams.status & (~GG_STATUS_FRIENDS_MASK));
		return;
	}
	else if (!DoBlink && !socket_active)
	{
		pix = icons_manager.loadIcon("Offline");
		statusbutton->setIconSet(QIconSet(pix));
		emit connectingBlinkShowOffline();
		return;
	}

	if (BlinkOn)
	{
		pix = icons_manager.loadIcon("Offline");
		statusbutton->setIconSet(QIconSet(pix));
		emit connectingBlinkShowOffline();
	}
	else
	{
		i = statusGGToStatusNr(loginparams.status & (~GG_STATUS_FRIENDS_MASK));
		pix = icons_manager.loadIcon(gg_icons[i]);
		statusbutton->setIconSet(QIconSet(pix));
		emit connectingBlinkShowStatus(loginparams.status & (~GG_STATUS_FRIENDS_MASK));
	}
	BlinkOn=!BlinkOn;

	blinktimer->start(1000, TRUE);
}

void Kadu::dccSetupFailed()
{
	QMessageBox::warning(kadu, "",
		tr("Couldn't create DCC socket.\nDirect connections disabled."));
}

void Kadu::userListUserAdded(const UserListElement& user)
{
	// jesli dodany do listy uzyszkodnik jest uzyszkodnikiem anonimowym
	// (odezwal sie do nas) i mamy wlaczone dokowanie (mozemy kliknac
	// na kopertce w trayu, zeby odebrac wiadomosc) to nie dodajemy
	// go do userboxa itp bo po co.
	if (user.anonymous && Docked)
		return;
	userlist.writeToFile();

	Userbox->addUser(user.altnick);
	UserBox::all_refresh();

	refreshGroupTabBar();

	if (user.uin)
		gg_add_notify(sess, user.uin);
}

void Kadu::mouseButtonClicked(int button, QListBoxItem *item) {
	kdebugm(KDEBUG_FUNCTION_START, "Kadu::mouseButtonClicked(): button=%d\n", button);
	if (!item)
		InfoPanel->setText("");

	if (button !=4 || !item)
		return;
}

/* if something's pending, open it, if not, open new message */
void Kadu::sendMessage(QListBoxItem *item)
{
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)
		return;
	UinType uin = userlist.byAltNick(item->text()).uin;
	if (uin) {
		UinsList uins = activeUserBox->getSelectedUins();
		if (uins.findIndex(config_file.readNumEntry("General", "UIN")) == -1)
			chat_manager->sendMessage(uin, uins);
	}
}

/* when we want to change the status */
void Kadu::slotHandleState(int command) {
	ChooseDescription *cd;
	
	switch (command) {
		case 0:
			Autohammer = true;
			setStatus(GG_STATUS_AVAIL);
			break;
		case 1:
			cd = new ChooseDescription(1);
			if (cd->exec() == QDialog::Accepted) {
				Autohammer = true;
				setStatus(GG_STATUS_AVAIL_DESCR);
				}
			break;
		case 2:
			Autohammer = true;
			setStatus(GG_STATUS_BUSY);
			break;
		case 3:
			cd = new ChooseDescription(3);
			if (cd->exec() == QDialog::Accepted) {
				Autohammer = true;
				setStatus(GG_STATUS_BUSY_DESCR);
				}
			break;
		case 4:
			Autohammer = true;
			setStatus(GG_STATUS_INVISIBLE);
			break;
		case 5:
			cd = new ChooseDescription(5);
			if (cd->exec() == QDialog::Accepted) {
				Autohammer = true;
				setStatus(GG_STATUS_INVISIBLE_DESCR);
				}
			break;
		case 6:
			gadu->logout();
		//	disconnectNetwork();
			AutoConnectionTimer::off();
			Autohammer = false;
			setCurrentStatus(GG_STATUS_NOT_AVAIL);
			break;
		case 7:
			cd = new ChooseDescription(7);
			if (cd->exec() == QDialog::Accepted) {
				setStatus(GG_STATUS_NOT_AVAIL_DESCR);
				statusppm->setItemEnabled(7, false);
				gadu->logout();
				//disconnectNetwork();
				AutoConnectionTimer::off();
				Autohammer = false;
				}
			break;
		case 8:
			int nstat=sess->status & (~GG_STATUS_FRIENDS_MASK);
			if (nstat)
			{
				statusppm->setItemChecked(8, !statusppm->isItemChecked(8));
				dockppm->setItemChecked(8, !dockppm->isItemChecked(8));
				config_file.writeEntry("General", "PrivateStatus",statusppm->isItemChecked(8));
				if (!statusppm->isItemChecked(6) && !statusppm->isItemChecked(7))
					setStatus(nstat);
			}
			break;
		}
}

void Kadu::setCurrentStatus(int status) {
	kdebugf();

	int statusnr;

	statusnr = statusGGToStatusNr(status);
	for(int i=0; i<8; i++)
	{
		statusppm->setItemChecked(i, false);
		dockppm->setItemChecked(i, false);
	}
	statusppm->setItemChecked(statusnr, true);
	dockppm->setItemChecked(statusnr, true);

	statusbutton->setText(qApp->translate("@default", statustext[statusnr]));
	statusppm->setItemEnabled(7, statusnr != 6);
	dockppm->setItemEnabled(7, statusnr != 6);
	QPixmap pix = icons_manager.loadIcon(gg_icons[statusnr]);
	statusbutton->setIconSet(QIconSet(pix));
	setIcon(pix);
	UserBox::all_refresh();
	emit currentStatusChanged(status);
}

void Kadu::setStatus(int status) {
	kdebugf();

	if (!UpdateChecked)
	{
		UinType myUin=(UinType)config_file.readNumEntry("General", "UIN");
		if (myUin)
		{
			uc = new UpdatesClass(myUin);
			QObject::connect(uc->op, SIGNAL(data(const QByteArray &, QNetworkOperation *)),
					this, SLOT(gotUpdatesInfo(const QByteArray &, QNetworkOperation *)));
			uc->run();
			UpdateChecked=true;
		}
	}

	gadu->setStatus(status);
}

void Kadu::connecting()
{
	kdebugf();

	DoBlink = true;

	if (!blinktimer)
	{
		blinktimer = new QTimer;
		QObject::connect(blinktimer, SIGNAL(timeout()), kadu, SLOT(blink()));
	}

	blinktimer->start(1000, true);
}

void Kadu::connected()
{
	kdebugf();

	DoBlink = false;

	kadu->setCurrentStatus(loginparams.status & (~GG_STATUS_FRIENDS_MASK));
	if ((loginparams.status & (~GG_STATUS_FRIENDS_MASK)) == GG_STATUS_INVISIBLE_DESCR)
		kadu->setStatus(loginparams.status & (~GG_STATUS_FRIENDS_MASK));
}

void Kadu::error(GaduError err)
{
	kdebugf();

	QString msg = QString::null;

	switch (err)
	{
		case ConnectionServerNotFound:
			msg = QString(tr("Unable to connect, server has not been found"));
			break;

		case ConnectionCannotConnect:
			msg = QString(tr("Unable to connect"));
			break;

		case ConnectionNeedEmail:
			msg = QString(tr("Please change your email in \"Change password/email\" window. "
				"Leave new password field blank."));
			Autohammer = false; /* FIXME 2/2*/
			AutoConnectionTimer::off();
			hintmanager->addHintError(msg);
			MessageBox::msg(msg);
			break;

		case ConnectionInvalidData:
			msg = QString(tr("Unable to connect, server has returned unknown data"));
			break;
			
		case ConnectionCannotRead:
			msg = QString(tr("Unable to connect, connection break during reading"));
			break;

		case ConnectionCannotWrite:
			msg = QString(tr("Unable to connect, connection break during writing"));
			break;

		case ConnectionIncorrectPassword:
			msg = QString(tr("Unable to connect, incorrect password"));
			Autohammer = false; /* FIXME 2/2*/
			AutoConnectionTimer::off();
			hintmanager->addHintError(msg);
			QMessageBox::critical(0, tr("Incorrect password"), tr("Connection will be stoped\nYour password is incorrect !!!"), QMessageBox::Ok, 0);
			return;

		case ConnectionTlsError:
			msg = QString(tr("Unable to connect, error of negotiation TLS"));
			break;

		case ConnectionUnknow:
			kdebugm(KDEBUG_INFO, "Connection broken unexpectedly!\nUnscheduled connection termination\n");
			break;

		case ConnectionTimeout:
			msg = QString(tr("Connection timeout!"));
			break;

		case Disconnected:
			kdebugm(KDEBUG_INFO, "Disconnection!\n");
			msg = QString(tr("Disconnection has occured"));
			break;

		default:
			break;
	
	}

	if (msg != QString::null)
	{
		QHostAddress* server = gadu->activeServer();
		QString host;
		if (server != NULL)
			host = server->toString();
		else
			host = "HUB";
		msg = QString("(") + host + ") " + msg;
		kdebugm(KDEBUG_INFO, "%s\n", unicode2latin(msg).data());
		hintmanager->addHintError(msg);
	}

	if (Autohammer)
		AutoConnectionTimer::on();

}

void Kadu::systemMessageReceived(QString &msg)
{
	MessageBox::msg(msg);
}

void Kadu::dataReceived(void) {
	kdebugf();
	if (sess->check && GG_CHECK_READ)
		event_manager.eventHandler(sess);
}

void Kadu::dataSent(void) {
	kdebugf();
	kadusnw->setEnabled(false);
	if (sess->check & GG_CHECK_WRITE)
		event_manager.eventHandler(sess);
}

void Kadu::disconnected()
{
	kdebugm(KDEBUG_FUNCTION_START, "Kadu::disconnected(): Disconnection has occured\n");

	DoBlink = false;

	if (blinktimer)
	{
		blinktimer->stop();
		delete blinktimer;
		blinktimer = NULL;
	}

	Autohammer = false;
	AutoConnectionTimer::off();
}

void Kadu::dccFinished(dccSocketClass *dcc) {
	kdebugf();
	delete dcc;
}

bool Kadu::event(QEvent *e) {
	QCustomEvent *ce;
	dccSocketClass *dcc;
	dccSocketClass **data;

	if (e->type() == QEvent::User) {
		kdebugf();
		ce = (QCustomEvent *)e;
		data = (dccSocketClass **)ce->data();
		dcc = *data;
		switch (dcc->state) {
			case DCC_SOCKET_TRANSFER_FINISHED:
				MessageBox::msg(tr("File has been transferred sucessfully."));
				break;
			case DCC_SOCKET_TRANSFER_DISCARDED:
				break;
			case DCC_SOCKET_TRANSFER_ERROR:
				MessageBox::msg(tr("File transfer error!"));
				break;
			case DCC_SOCKET_CONNECTION_BROKEN:
				break;
			case DCC_SOCKET_COULDNT_OPEN_FILE:
				MessageBox::msg(tr("Couldn't open file!"));
				break;
			}
		delete data;
		delete dcc;
		ce->setData(NULL);
		}
	return QWidget::event(e);
}

void Kadu::dccReceived(void) {
	kdebugf();
	watchDcc();
}

void Kadu::dccSent(void) {
	kdebugf();
	dccsnw->setEnabled(false);
	if (dccsock->check & GG_CHECK_WRITE)
		watchDcc();
}

void Kadu::watchDcc(void) {
	kdebugf();
	struct gg_event* dcc_e;
	if (!(dcc_e = gg_dcc_watch_fd(dccsock))) {
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "Kadu::watchDcc(): Connection broken unexpectedly!\n");
		config_file.writeEntry("Network", "AllowDCC", false);
		delete dccsnr;
		dccsnr = NULL;
		delete dccsnw;
		dccsnw = NULL;
		return;
		}

	switch (dcc_e->type) {
		case GG_EVENT_NONE:
			break;
		case GG_EVENT_DCC_ERROR:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "Kadu::watchDcc(): GG_EVENT_DCC_ERROR\n");
			break;
		case GG_EVENT_DCC_NEW:
			if (dccSocketClass::count < 8) {
				dccSocketClass *dcc;
				dcc = new dccSocketClass(dcc_e->event.dcc_new);
				connect(dcc, SIGNAL(dccFinished(dccSocketClass *)), this, SLOT(dccFinished(dccSocketClass *)));
				dcc->initializeNotifiers();
				kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "Kadu::watchDcc(): GG_EVENT_DCC_NEW: spawning object\n");
				}
			else {
				if (dcc_e->event.dcc_new->file_fd > 0)
					close(dcc_e->event.dcc_new->file_fd);
				gg_dcc_free(dcc_e->event.dcc_new);
				}
			break;
		default:
			break;
		}

	if (dccsock->check == GG_CHECK_WRITE)
		dccsnw->setEnabled(true);

	gg_free_event(dcc_e);
}

bool Kadu::close(bool quit) {
	if (!quit && Docked) {
		kdebugm(KDEBUG_INFO, "Kadu::close(): Kadu hide\n");
		hide();
		return false;
	}
	else {
		chat_manager->closeAllWindows();
		ConfigDialog::closeDialog();
		ModulesManager::closeModule();

		if (config_file.readBoolEntry("General", "SaveGeometry"))
		{
			if (config_file.readBoolEntry("Look", "ShowInfoPanel"))
			{
//				QSize split;
				config_file.writeEntry("General", "UserBoxHeight", Userbox->size().height());
				config_file.writeEntry("General", "DescriptionHeight", InfoPanel->size().height());
			}
			if (config_file.readBoolEntry("Look", "ShowStatusButton"))
			{
				config_file.writeEntry("General", "UserBoxHeight", Userbox->size().height());
			}
			saveGeometry(this, "General", "Geometry");
		}
	
		config_file.writeEntry("General", "DefaultDescription", defaultdescriptions.join("<-->"));
		config_file.writeEntry( "Look", "CurrentGroupTab", GroupBar->currentTab() );

		QString dockwindows=config_file.readEntry("General", "DockWindows");
		QTextStream stream(&dockwindows, IO_WriteOnly);
		stream << *kadu;
		dockwindows.replace(QRegExp("\\n"), "\\n");	
		config_file.writeEntry("General", "DockWindows", dockwindows);
		
		delete ToolBar::instance;
		
		config_file.sync();
		
		pending.writeToFile();
		writeIgnored();
		if (config_file.readBoolEntry("General", "DisconnectWithDescription") && getCurrentStatus() != GG_STATUS_NOT_AVAIL) {
			kdebugm(KDEBUG_INFO, "Kadu::close(): Set status NOT_AVAIL_DESCR with disconnect description(%s)\n",(const char *)config_file.readEntry("General", "DisconnectDescription").local8Bit());
			own_description = config_file.readEntry("General", "DisconnectDescription");
			setStatus(GG_STATUS_NOT_AVAIL_DESCR);
		}
//		disconnectNetwork();
		gadu->logout();
		kdebugm(KDEBUG_INFO, "Kadu::close(): Saved config, disconnect and ignored\n");
		QWidget::close(true);
		flock(lockFileHandle, LOCK_UN);
		lockFile->close();
		delete lockFile;
		lockFile=NULL;
		kdebugm(KDEBUG_INFO, "Kadu::close(): Graceful shutdown...\n");
		return true;
	}
}

void Kadu::quitApplication() {
	kdebugf();
	close(true);
}

Kadu::~Kadu(void) {
	kdebugf();
	if (hintmanager)
	{
		delete hintmanager;
		hintmanager=NULL;
	}
}

void Kadu::createMenu() {

	MenuBar = new QMenuBar(this, "MenuBar");

	MainMenu = new QPopupMenu(this, "MainMenu");
	MainMenu->insertItem(tr("Manage &ignored"), this, SLOT(manageIgnored()));
	MainMenu->insertItem(icons_manager.loadIcon("Configuration"), tr("&Configuration"), this, SLOT(configure()),HotKey::shortCutFromFile("ShortCuts", "kadu_configure"));
	MainMenu->insertSeparator();

	MainMenu->insertItem(tr("Remind &password"), this, SLOT(remindPassword()));
	MainMenu->insertItem(tr("&Change password/email"), this, SLOT(changePassword()));
	MainMenu->insertItem(icons_manager.loadIcon("RegisterUser"),tr("Register &new user"), this, SLOT(registerUser()));
	MainMenu->insertItem(tr("Unregister user"), this, SLOT(unregisterUser()));
	MainMenu->insertItem(tr("Personal information"), this,SLOT(personalInfo()));
	MainMenu->insertSeparator();
	MainMenu->insertItem(icons_manager.loadIcon("LookupUserInfo"), tr("&Search for users"), this, SLOT(searchInDirectory()));
	MainMenu->insertItem(tr("I&mport / Export userlist"), this, SLOT(importExportUserlist()));
	MainMenu->insertItem(icons_manager.loadIcon("AddUser"), tr("&Add user"), this, SLOT(addUserAction()),HotKey::shortCutFromFile("ShortCuts", "kadu_adduser"));
	MainMenu->insertSeparator();
	MainMenu->insertItem(tr("H&elp"), this, SLOT(help()));
	MainMenu->insertItem(tr("A&bout..."), this, SLOT(about()));
	MainMenu->insertSeparator();
	MainMenu->insertItem(tr("&Hide Kadu"), this, SLOT(hideKadu()));
	MainMenu->insertItem(icons_manager.loadIcon("Exit"), tr("&Exit Kadu"), this, SLOT(quit()));

	MenuBar->insertItem(tr("&Kadu"), MainMenu);
}

void Kadu::InitModules()
{
}

void Kadu::statusMenuAboutToHide() {
	closestatusppmtime.restart();
}

void Kadu::createStatusPopupMenu() {

	QPixmap pix;
	QIconSet icon;

	statusppm = new QPopupMenu(this, "statusppm");
	dockppm = new QPopupMenu(this, "dockppm");

	for (int i=0; i<8; i++) {
		pix = icons_manager.loadIcon(gg_icons[i]);
		icon = QIconSet(pix);
		statusppm->insertItem(icon, qApp->translate("@default", statustext[i]), i);
		dockppm->insertItem(icon, qApp->translate("@default", statustext[i]), i);
		}

	bool privateStatus=config_file.readBoolEntry("General", "PrivateStatus");
	statusppm->insertSeparator();
	dockppm->insertSeparator();
	statusppm->insertItem(tr("Private"), 8);
	statusppm->setItemChecked(8, privateStatus);
	dockppm->insertItem(tr("Private"), 8);
	dockppm->setItemChecked(8, privateStatus);

	statusppm->setCheckable(true);
	dockppm->setCheckable(true);
	statusppm->setItemChecked(6, true);
	dockppm->setItemChecked(6, true);

	statusppm->setItemEnabled(7, false);
	dockppm->setItemEnabled(7, false);

	connect(statusppm, SIGNAL(activated(int)), this, SLOT(slotHandleState(int)));
}

void Kadu::showdesc(bool show) {
	if (show)
		InfoPanel->show();
	else
		InfoPanel->QWidget::hide();
}

void Kadu::infopanelUpdate(UinType uin) {
	if (!config_file.readBoolEntry("Look", "ShowInfoPanel"))
		return;
	kdebugm(KDEBUG_INFO, "Kadu::infopanelUpdate(%d)\n", uin);
	if (Userbox->currentItem() != -1 && uin == userlist.byAltNick(Userbox->currentText()).uin)
	{
		HtmlDocument doc;
		doc.parseHtml(parse(config_file.readEntry("Look", "PanelContents"), userlist.byUin(uin)));
		doc.convertUrlsToHtml();
		if((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle")!=EMOTS_NONE && config_file.readBoolEntry("General", "ShowEmotPanel"))
		{
			InfoPanel->mimeSourceFactory()->addFilePath(emoticons->themePath());
			emoticons->expandEmoticons(doc, config_file.readColorEntry("Look", "InfoPanelBgColor"));
		}
		InfoPanel->setText(doc.generateHtml());
		if (config_file.readBoolEntry("General", "ShowEmotPanel"))
			InfoPanel->scrollToBottom();
	}
}

QMenuBar* Kadu::menuBar()
{
	return MenuBar;
}

QPopupMenu* Kadu::mainMenu()
{
	return MainMenu;
}

KaduTabBar* Kadu::groupBar()
{
	return GroupBar;
}

UserBox* Kadu::userbox()
{
	return Userbox;
}

void Kadu::setDocked(bool docked)
{
	Docked = docked;
}

bool Kadu::docked()
{
	return Docked;
}

void Kadu::show()
{
	QMainWindow::show();
	emit shown();
}

/*
void Kadu::showMinimized()
{
	kdebugf();
	QWidget::showMinimized();
//	emit minimized();
}

void Kadu::hide()
{
	kdebugf();
//	emit minimized();
	QWidget::hide();
}
*/

void KaduSlots::onCreateConfigDialog()
{
	kdebugf();
	QLineEdit *e_password=ConfigDialog::getLineEdit("General", "Password");
	e_password->setEchoMode(QLineEdit::Password);
	e_password->setText(pwHash(config_file.readEntry("General", "Password", "")));
	QComboBox *cb_language= ConfigDialog::getComboBox("General", "Set language:");

	QDir locale(dataPath("kadu/translations/"), "kadu_*.qm");
	QStringList files=locale.entryList();

	for ( QStringList::Iterator it = files.begin(); it != files.end(); ++it )
		*it=translateLanguage(qApp, (*it).mid(5, (*it).length()-8), true);
	cb_language->insertStringList(files);
	cb_language->setCurrentText(translateLanguage(qApp,
	config_file.readEntry("General", "Language", QTextCodec::locale()),true));

	QCheckBox *b_disconnectdesc= ConfigDialog::getCheckBox("General", "On shutdown, set description:");
	QLineEdit *e_disconnectdesc= ConfigDialog::getLineEdit("General", "", "e_defaultstatus");
	e_disconnectdesc->setMaxLength(GG_STATUS_DESCR_MAXSIZE);
	e_disconnectdesc->setEnabled(b_disconnectdesc->isChecked());
	connect(b_disconnectdesc, SIGNAL(toggled(bool)), e_disconnectdesc, SLOT(setEnabled(bool)));

	QComboBox* cb_defstatus= ConfigDialog::getComboBox("General", "Default status", "cb_defstatus");
	int statusnr=config_file.readNumEntry("General", "DefaultStatus", GG_STATUS_NOT_AVAIL);
	cb_defstatus->clear();
	int i;
	for (i = 0;i < 7; i++)
		cb_defstatus->insertItem(qApp->translate("@default", statustext[i]));
	i=0;	
	while (i<7 && statusnr !=gg_statuses[i])
		i++;
	cb_defstatus->setCurrentItem(i);
	updatePreview();
}

void KaduSlots::onDestroyConfigDialog()
{
	kdebugf();
	QLineEdit *e_password=ConfigDialog::getLineEdit("General", "Password");
	e_password->setEchoMode(QLineEdit::Password);
	config_file.writeEntry("General", "Password",pwHash(e_password->text()));

	kadu->showdesc(config_file.readBoolEntry("Look", "ShowInfoPanel"));

	if (config_file.readBoolEntry("Look", "ShowStatusButton"))
		statusbutton->show();
	else
		statusbutton->hide();

	if (config_file.readBoolEntry("Look", "MultiColumnUserbox"))
		kadu->userbox()->setColumnMode(QListBox::FitToWidth);
	else
		kadu->userbox()->setColumnMode(1);

	QComboBox* cb_defstatus= ConfigDialog::getComboBox("General", "Default status", "cb_defstatus");
	config_file.writeEntry("General", "DefaultStatus", gg_statuses[cb_defstatus->currentItem()]);

	int status = getCurrentStatus();

	bool privateStatus=config_file.readBoolEntry("General", "PrivateStatus");
	
	if (status != GG_STATUS_NOT_AVAIL)
	if ((!(status & GG_STATUS_FRIENDS_MASK)&& privateStatus)
		|| ((status & GG_STATUS_FRIENDS_MASK) && !privateStatus))
		kadu->setStatus(status & (~GG_STATUS_FRIENDS_MASK));

	statusppm->setItemChecked(8, privateStatus);

	/* I od¶wie¿ okno Kadu */
	kadu->changeAppearance();
	chat_manager->changeAppearance();
	kadu->refreshGroupTabBar();
	kadu->setCaption(tr("Kadu: %1").arg((UinType)config_file.readNumEntry("General", "UIN")));

	QComboBox *cb_language= ConfigDialog::getComboBox("General", "Set language:");
	config_file.writeEntry("General", "Language", translateLanguage(qApp, cb_language->currentText(),false));
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
		kdebugm(KDEBUG_ERROR, "chooseColor: label '%s' not known\n", name);
}

void KaduSlots::chooseFont(const char *name, const QFont& font)
{
	kdebugf();
	QLabel *preview= ConfigDialog::getLabel("Look", "<b>Text</b> preview", "preview_panel");
	if (QString(name)=="panel_font_box")
		preview->setFont(font);
}

void KaduSlots::updatePreview()
{
	kdebugf();
	QLabel *preview= ConfigDialog::getLabel("Look", "<b>Text</b> preview", "preview_panel");
	preview->setFont(config_file.readFontEntry("Look", "PanelFont"));
	preview->setPaletteForegroundColor(config_file.readColorEntry("Look", "InfoPanelFgColor"));
	preview->setPaletteBackgroundColor(config_file.readColorEntry("Look", "InfoPanelBgColor"));
	preview->setAlignment(Qt::AlignLeft);
}

void Kadu::resizeEvent(QResizeEvent *)
{
//	Userbox->all_refresh();
}

/*void Kadu::moveEvent(QMoveEvent *e)
{
//	kdebugm(KDEBUG_INFO, "kadu::moveEvent: %d %d %d %d\n", x(), y(), width(), height());
	QWidget::moveEvent(e);
}*/

void Kadu::startupProcedure()
{
	if (ShowMainWindowOnStart)
		show();

	if (!config_file.readNumEntry("General","UIN"))
	{
		QString path_;
		path_ = ggPath("");
		mkdir(path_.local8Bit().data(), 0700);
		path_.append("/history/");
		mkdir(path_.local8Bit().data(), 0700);
		switch (QMessageBox::information(kadu, "Kadu",
			tr("You don't have a config file.\nWhat would you like to do?"),
			tr("New UIN"),
			tr("Configure"),
			tr("Cancel"), 0, 1) )
		{
			case 1: // Configure
				ConfigDialog::showConfigDialog(qApp);
				break;
			case 0: // Register
				(new Register())->show();
				break;
			case 2: // Nothing
				break;
		}
		setCaption(tr("Kadu: new user"));
	}

	own_description = defaultdescriptions.first();
	int defaultStatus = config_file.readNumEntry("General","DefaultStatus",GG_STATUS_NOT_AVAIL);
	if (defaultStatus != GG_STATUS_NOT_AVAIL && defaultStatus != GG_STATUS_NOT_AVAIL_DESCR)
	{
		Autohammer = true;
		setStatus(defaultStatus);
	}
}

void Kadu::setShowMainWindowOnStart(bool show)
{
	ShowMainWindowOnStart = show;
}
