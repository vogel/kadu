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

QValueList<QHostAddress> gg_servers;
const char *gg_servers_ip[7] = {"217.17.41.82", "217.17.41.83", "217.17.41.84", "217.17.41.85",
	"217.17.41.86", "217.17.41.87", "217.17.41.88"};

QValueList<ToolBar::ToolButton> ToolBar::RegisteredToolButtons;
ToolBar* ToolBar::instance=NULL;

ToolBar::ToolBar(QMainWindow* parent) : QToolBar(parent, "main toolbar")
{
	setCloseMode(QDockWindow::Undocked);
	setLabel(tr("Main toolbar"));
	setVerticallyStretchable(true);

	config_file.addVariable("General", "ToolBarHidden", false);
	if (config_file.readBoolEntry("General", "ToolBarHidden"))
		hide();

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

	QFrame *toolbarfiller = new QFrame(this);
	setStretchableWidget(toolbarfiller);
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
	kdebug("return NULL\n");
	return NULL;
}

void Kadu::gotUpdatesInfo(const QByteArray &data, QNetworkOperation *op) {
	char buf[32];
//	int i;
	QString newestversion;

	if (config_file.readBoolEntry("General", "CheckUpdates"))
	{
		if (data.size() > 31) {
			kdebug("Kadu::gotUpdatesInfo(): cannot obtain update info\n");
			delete uc;
			return;
		}
		for (unsigned i = 0; i < data.size(); i++)
			buf[i] = data[i];
		buf[data.size()] = 0;
		newestversion = buf;

		kdebug("Kadu::gotUpdatesInfo(): %s\n", buf);

		if (uc->ifNewerVersion(newestversion))
			QMessageBox::information(this, tr("Update information"),
				tr("The newest Kadu version is %1").arg(newestversion), QMessageBox::Ok);
	}
	updateChecked=true;
	delete uc;
}

void Kadu::keyPressEvent(QKeyEvent *e) {
	if (e->key() == Key_Escape && Docked) {
		kdebug("Kadu::keyPressEvent(Key_Escape): Kadu hide\n");
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
	updateChecked=false;
	Docked=false;

	KaduSlots *kaduslots=new KaduSlots();
	uin_t myUin=config_file.readNumEntry("General", "UIN");

	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "General"));
	ConfigDialog::addHGroupBox("General", "General", QT_TRANSLATE_NOOP("@default", "User data"));
	ConfigDialog::addLineEdit("General", "User data", QT_TRANSLATE_NOOP("@default", "Uin"), "UIN", "0");
	ConfigDialog::addLineEdit("General", "User data", QT_TRANSLATE_NOOP("@default", "Password"), "Password", "");
	ConfigDialog::addLineEdit("General", "User data", QT_TRANSLATE_NOOP("@default", "Nick"), "Nick", tr("Me"));
	ConfigDialog::addComboBox("General", "General", QT_TRANSLATE_NOOP("@default", "Set language:"));
	ConfigDialog::addGrid("General", "General", "grid", 2);
	ConfigDialog::addCheckBox("General", "grid", QT_TRANSLATE_NOOP("@default", "Log messages"), "Logging", true);
	ConfigDialog::addCheckBox("General", "grid", QT_TRANSLATE_NOOP("@default", "Restore window geometry"), "SaveGeometry", true);
	ConfigDialog::addCheckBox("General", "grid", QT_TRANSLATE_NOOP("@default", "Check for updates"), "CheckUpdates", true);

	ConfigDialog::addCheckBox("General", "grid", QT_TRANSLATE_NOOP("@default", "Private status"), "PrivateStatus", false);

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

	//zaladowanie wartosci domyslnych (pierwsze uruchomienie)
	QRect def_rect(0, 0, 145, 465);
	config_file.addVariable("General", "Geometry", def_rect);
	config_file.addVariable("General", "UserBoxHeight", 300);
	config_file.addVariable("General", "DescriptionHeight", 60);

	QFontInfo info(qApp->font());
	QFont def_font(info.family(),info.pointSize());
	config_file.addVariable("Look", "UserboxFont", &def_font);
	config_file.addVariable("Look", "UserboxDescFont", &def_font);


	closestatusppmtime.start();

	gg_proxy_host = NULL;

	/* timers, cause event loops and QSocketNotifiers suck. */

	//pingtimer = blinktimer = readevent = NULL; zamieniamy na(powod: patrz plik events.cpp)
	pingtimer = blinktimer = NULL;

	/* blinker */
	blinkOn = false;
	doBlink = false;

	/* another API change, another hack */
	memset(&loginparams, 0, sizeof(loginparams));
	loginparams.async = 1;

	QRect geom;
	geom=config_file.readRectEntry("General", "Geometry");
	kdebug("Setting size: width=%d, height=%d and setting position: x=%d, y=%d\n",
		geom.width(),geom.height(),
		geom.x(), geom.y());
	resize(geom.width(),geom.height());
	move(geom.x(),geom.y());

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

	descrtb = new QTextBrowser(split, "descrtb");
	descrtb->setFrameStyle(QFrame::NoFrame);
	descrtb->setMinimumHeight(int(1.5 * QFontMetrics(descrtb->font()).height()));
//	descrtb->resize(descrtb->size().width(), int(1.5 * QFontMetrics(descrtb->font()).height()));
	descrtb->setTextFormat(Qt::RichText);
	descrtb->setAlignment(Qt::AlignVCenter | Qt::WordBreak | Qt::DontClip);
	descrtb->setVScrollBarMode(QScrollView::AlwaysOff);
	descrtb->setPaletteBackgroundColor(config_file.readColorEntry("Look", "UserboxDescBgColor"));
	descrtb->setPaletteForegroundColor(config_file.readColorEntry("Look", "UserboxDescTextColor"));
	descrtb->setFont(config_file.readFontEntry("Look", "UserboxDescFont"));
	if (!config_file.readBoolEntry("Look", "ShowInfoPanel"))
		descrtb->hide();
	QObject::connect(&userlist, SIGNAL(dnsNameReady(uin_t)), this, SLOT(infopanelUpdate(uin_t)));

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

//	tworzymy liste serverow domyslnych gg
	QHostAddress ip;
	for (int i = 0; i < 7; i++) {
		ip.setAddress(QString(gg_servers_ip[i]));
		gg_servers.append(ip);
		}

	refreshGroupTabBar();
	int configTab = config_file.readNumEntry( "Look", "CurrentGroupTab" );
	if ( configTab >= 0 && configTab < GroupBar -> count() )
		((QTabBar*) GroupBar) -> setCurrentTab( configTab );

	dccsock = NULL;
	/* dirty workaround for multiple showEvents */
	commencing_startup = true;

	show();
}

void Kadu::createToolBar()
{
	new ToolBar(this);
	setRightJustification(true);
	setDockEnabled(Qt::DockBottom, false);
	setAppropriate(ToolBar::instance, true);
}

void Kadu::popupMenu()
{
	UserList users;
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)//to siê zdarza...
		return;
	users = activeUserBox->getSelectedUsers();
	UserListElement user = users.first();

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
			UserListElement user = users.first();
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
		sd = new SearchDialog(0, tr("User info"), userlist.byAltNick(users.first().altnick).uin);
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
		UserInfo *ui = new UserInfo("user info", 0, users.first().altnick);
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
		descrtb->setText("");
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
	About *about = new About;
	about->show();
}

void Kadu::remindPassword()
{
	RemindPassword *rp = new RemindPassword();
	rp->start();
}

void Kadu::changePassword()
{
	new ChangePassword();
}

void Kadu::registerUser()
{
	new Register();
}

void Kadu::unregisterUser()
{
	new Unregister();
}

void Kadu::quit()
{
	kdebugf();
	close(true);
}

void Kadu::exportUserlist()
{
	UserlistExport *ule = new UserlistExport();
	ule->show();
}

void Kadu::importUserlist()
{
	UserlistImport *uli = new UserlistImport();
	uli->show();
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
	UserListElement *puser = &userlist.byAltNick(activeUserBox->getSelectedUsers().first().altnick);
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
	UserListElement *puser = &userlist.byAltNick(activeUserBox->getSelectedUsers().first().altnick);
	puser->notify = !puser->notify;
	userlist.writeToFile();
}

void Kadu::offlineToUser()
{
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)
		return;
	UserListElement *puser = &userlist.byAltNick(activeUserBox->getSelectedUsers().first().altnick);
	puser->offline_to_user = !puser->offline_to_user;
	gg_remove_notify_ex(sess, puser->uin, puser->offline_to_user ? GG_USER_NORMAL : GG_USER_OFFLINE);
	gg_add_notify_ex(sess, puser->uin, puser->offline_to_user ? GG_USER_OFFLINE : GG_USER_NORMAL);
	userlist.writeToFile();
}

void Kadu::changeAppearance() {
	kdebug("kadu::changeAppearance()\n");

	Userbox->setPaletteBackgroundColor(config_file.readColorEntry("Look", "UserboxBgColor"));
	Userbox->setPaletteForegroundColor(config_file.readColorEntry("Look", "UserboxFgColor"));
	Userbox->QListBox::setFont(config_file.readFontEntry("Look", "UserboxFont"));

	GroupBar->setFont(QFont(config_file.readFontEntry("Look", "UserboxFont").family(), config_file.readFontEntry("Look", "UserboxFont").pointSize(),75));

	descrtb->setPaletteBackgroundColor(config_file.readColorEntry("Look", "UserboxDescBgColor"));
	descrtb->setPaletteForegroundColor(config_file.readColorEntry("Look", "UserboxDescTextColor"));
	descrtb->setFont(config_file.readFontEntry("Look", "UserboxDescFont"));
}

void Kadu::currentChanged(QListBoxItem *item) {
	if (!item || !item->isSelected())
		return;

	kdebug("Kadu::currentChanged(): %s\n", (const char *)item->text().local8Bit());

	if (config_file.readBoolEntry("Look", "ShowInfoPanel"))
		descrtb->setText(parse(config_file.readEntry("Look", "PanelContents"),userlist.byAltNick(item->text())));
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
	for (unsigned int i = 0; i < userlist.count(); i++)
	{
		QString groups = userlist[i].group();
		QString group;
		for (int g = 0; (group = groups.section(',' ,g ,g)) != ""; g++)
			if(!group_list.contains(group))
				group_list.append(group);
	}
	kdebug("%i groups found\n",group_list.count());
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
	kdebug("%i group tabs\n", GroupBar->count());
	GroupBar->show();
	/* odswiezamy - dziala tylko jesli jest widoczny */
	GroupBar->update();
}

void Kadu::setActiveGroup(const QString& group)
{
	Userbox->clearUsers();
	for (unsigned int i = 0; i < userlist.count(); i++)
		{
		bool belongsToGroup;
		if (group == "")
			belongsToGroup = true;
		else
			{
			belongsToGroup = false;
			QString user_groups = userlist[i].group();
			QString user_group;
			for (int g = 0; (user_group = user_groups.section(',',g,g)) != ""; g++)
				if (user_group == group)
					belongsToGroup = true;
			}
		if (belongsToGroup && (!userlist[i].anonymous || !Docked))
			Userbox->addUser(userlist[i].altnick);
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
	kdebug("Kadu::userListStatusModified(): %d\n", user->uin);
	if ((user->status == GG_STATUS_NOT_AVAIL)
		|| (user->status == GG_STATUS_NOT_AVAIL_DESCR))
		descrtb->setText("");
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

	if (!doBlink && socket_active) {
		setCurrentStatus(loginparams.status & (~GG_STATUS_FRIENDS_MASK));
		return;
		}
	else
		if (!doBlink && !socket_active)
		{
			pix = icons_manager.loadIcon("Offline");
			statusbutton->setIconSet(QIconSet(pix));
			emit connectingBlinkShowOffline();
			return;
		}

	if (blinkOn) {
		pix = icons_manager.loadIcon("Offline");
		statusbutton->setIconSet(QIconSet(pix));
		emit connectingBlinkShowOffline();
		blinkOn = false;
		}
	else {
		i = statusGGToStatusNr(loginparams.status & (~GG_STATUS_FRIENDS_MASK));
		pix = icons_manager.loadIcon(gg_icons[i]);
		statusbutton->setIconSet(QIconSet(pix));
		emit connectingBlinkShowStatus(loginparams.status & (~GG_STATUS_FRIENDS_MASK));
		blinkOn = true;
		}

	blinktimer->start(1000, TRUE);
}

/* dcc initials */
void Kadu::prepareDcc(void) {
	QHostAddress dccip;

	if (!config_dccip.ip4Addr())
		dccip.setAddress("255.255.255.255");
	else
		dccip = config_dccip;

	dccsock = gg_dcc_socket_create(config_file.readNumEntry("General", "UIN"), config_file.readNumEntry("Network", "LocalPort", 1550));

	if (!dccsock) {
		kdebug("Kadu::prepareDcc(): Couldn't bind DCC socket.\n");
		gg_dcc_free(dccsock);
		QMessageBox::warning(kadu, "",
			tr("Couldn't create DCC socket.\nDirect connections disabled."));
		return;
		}

	gg_dcc_ip = htonl(dccip.ip4Addr());
	gg_dcc_port = dccsock->port;

	kdebug("Kadu::prepareDcc() DCC_IP=%s DCC_PORT=%d\n", dccip.toString().latin1(), dccsock->port);

	dccsnr = new QSocketNotifier(dccsock->fd, QSocketNotifier::Read, kadu);
	QObject::connect(dccsnr, SIGNAL(activated(int)), kadu, SLOT(dccReceived()));

	dccsnw = new QSocketNotifier(dccsock->fd, QSocketNotifier::Write, kadu);
	QObject::connect(dccsnw, SIGNAL(activated(int)), kadu, SLOT(dccSent()));
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
	kdebug("Kadu::mouseButtonClicked(): button=%d\n", button);
	if (!item)
		descrtb->setText("");

	if (button !=4 || !item)
		return;
}


/* if something's pending, open it, if not, open new message */
void Kadu::sendMessage(QListBoxItem *item)
{
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)
		return;
	uin_t uin = userlist.byAltNick(item->text()).uin;
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
			autohammer = true;
			setStatus(GG_STATUS_AVAIL);
			break;
		case 1:
			cd = new ChooseDescription(1);
			if (cd->exec() == QDialog::Accepted) {
				autohammer = true;
				setStatus(GG_STATUS_AVAIL_DESCR);
				}
			break;
		case 2:
			autohammer = true;
			setStatus(GG_STATUS_BUSY);
			break;
		case 3:
			cd = new ChooseDescription(3);
			if (cd->exec() == QDialog::Accepted) {
				autohammer = true;
				setStatus(GG_STATUS_BUSY_DESCR);
				}
			break;
		case 4:
			autohammer = true;
			setStatus(GG_STATUS_INVISIBLE);
			break;
		case 5:
			cd = new ChooseDescription(5);
			if (cd->exec() == QDialog::Accepted) {
				autohammer = true;
				setStatus(GG_STATUS_INVISIBLE_DESCR);
				}
			break;
		case 6:
			disconnectNetwork();
			AutoConnectionTimer::off();
			autohammer = false;
			setCurrentStatus(GG_STATUS_NOT_AVAIL);
			break;
		case 7:
			cd = new ChooseDescription(7);
			if (cd->exec() == QDialog::Accepted) {
				setStatus(GG_STATUS_NOT_AVAIL_DESCR);
				statusppm->setItemEnabled(7, false);
				disconnectNetwork();
				AutoConnectionTimer::off();
				autohammer = false;
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
	int statusnr;
	int i = 0;

	statusnr = statusGGToStatusNr(status);
	while (i<8) {
		statusppm->setItemChecked(i, false);
		dockppm->setItemChecked(i, false);
		i++;
		}
	statusppm->setItemChecked(statusnr, true);
	dockppm->setItemChecked(statusnr, true);

	statusbutton->setText(qApp->translate("@default", statustext[statusnr]));
	statusppm->setItemEnabled(7, statusnr != 6);
	dockppm->setItemEnabled(7, statusnr != 6);
	QPixmap pix = icons_manager.loadIcon(gg_icons[statusnr]);
	statusbutton->setIconSet(QIconSet(pix));
	setIcon(pix);
	emit currentStatusChanged(status);
}

void Kadu::setStatus(int status) {
	unsigned char *descr;
	QHostAddress ip;

	kdebug("Kadu::setStatus(): setting status: %d\n",
		status | (GG_STATUS_FRIENDS_MASK * config_file.readBoolEntry("General", "PrivateStatus")));

	if (!updateChecked)
	{
		uin_t myUin=(uin_t)config_file.readNumEntry("General", "UIN");
		if (myUin) {
			uc = new UpdatesClass(myUin);
			QObject::connect(uc->op, SIGNAL(data(const QByteArray &, QNetworkOperation *)),
					this, SLOT(gotUpdatesInfo(const QByteArray &, QNetworkOperation *)));
			uc->run();
			updateChecked=true;
		}
	}

	bool with_description;

	with_description = ifStatusWithDescription(status);
	status &= ~GG_STATUS_FRIENDS_MASK;

	i_wanna_be_invisible = false;
	if (status == GG_STATUS_INVISIBLE || status == GG_STATUS_INVISIBLE_DESCR)
		i_wanna_be_invisible = true;

	if (!userlist_sent) {
		doBlink = true;
		if (!blinktimer) {
			blinktimer = new QTimer;
			QObject::connect(blinktimer, SIGNAL(timeout()), kadu, SLOT(blink()));
			}
		blinktimer->start(1000, TRUE);
		}

	if (socket_active) {
		QString sigDesc = QString::null;
		bool stop = FALSE;
		if (with_description)
			sigDesc = own_description;

		emit changingStatus(status, sigDesc, stop);
		if (stop)
			return;

		doBlink = false;
		if (with_description) {
			descr = (unsigned char *)strdup(unicode2cp(own_description).data());
			if (status == GG_STATUS_NOT_AVAIL_DESCR)
				gg_change_status_descr(sess, status, (const char *)descr);
			else
				gg_change_status_descr(sess,
					status | (GG_STATUS_FRIENDS_MASK * config_file.readBoolEntry("General", "PrivateStatus")), (const char *)descr);
			free(descr);
			}
		else {
			gg_change_status(sess, status | (GG_STATUS_FRIENDS_MASK * config_file.readBoolEntry("General", "PrivateStatus")));
			own_description = QString::null;
			}
		if (sess->check & GG_CHECK_WRITE)
			kadusnw->setEnabled(true);

		setCurrentStatus(status);

		kdebug("Kadu::setStatus(): actual status: %d\n", sess->status);
		/** AutoConnectionTimer u¿ywa loginparams.status jako statusu do nowego po³±czenia siê
				po stracie po³±czenia, czyli kadu bêdzie próbowal sie po³±czyæ z statusem takim
				samym, co by³ ostatnio ustawiony(oprócz niedostêpny i niedostêpny z opisem).
		**/
		if (status != GG_STATUS_NOT_AVAIL && status != GG_STATUS_NOT_AVAIL_DESCR)
			loginparams.status = status | (GG_STATUS_FRIENDS_MASK * config_file.readBoolEntry("General", "PrivateStatus"));
		UserBox::all_refresh();
		return;
		}

	memset(&loginparams, 0, sizeof(loginparams));
	loginparams.async = 1;
	
	// maksymalny rozmiar grafiki w kb
	loginparams.image_size = config_file.readNumEntry("Chat", "MaxImageSize", 20);

	if (config_file.readBoolEntry("Network", "AllowDCC"))
		prepareDcc();

	if (gg_proxy_host) {
		free(gg_proxy_host);
		gg_proxy_host = NULL;
		}
	if (gg_proxy_username) {
		free(gg_proxy_username);
		free(gg_proxy_password);
		gg_proxy_username = gg_proxy_password = NULL;
		}
	if (config_file.readBoolEntry("Network", "UseProxy")) {
		gg_proxy_host = strdup((char *)unicode2latin(config_file.readEntry("Network", "ProxyHost")).data());
		kdebug("Kadu::setStatus(): gg_proxy_host = %s\n", gg_proxy_host);
		gg_proxy_port = config_file.readNumEntry("Network", "ProxyPort");
		kdebug("Kadu::setStatus(): gg_proxy_port = %d\n", gg_proxy_port);
		if (config_file.readEntry("Network", "ProxyUser").length()) {
			gg_proxy_username = strdup((char *)unicode2latin(config_file.readEntry("Network", "ProxyUser")).data());
			gg_proxy_password = strdup((char *)unicode2latin(config_file.readEntry("Network", "ProxyPassword")).data());
			}
		gg_proxy_enabled = 1;
		}
	else
		gg_proxy_enabled = 0;

	loginparams.status = status | (GG_STATUS_FRIENDS_MASK * config_file.readBoolEntry("General", "PrivateStatus"));
	if (with_description)
		loginparams.status_descr = strdup((const char *)unicode2cp(own_description));
	loginparams.password = strdup((const char *)unicode2cp(pwHash(config_file.readEntry("General", "Password"))));
	char *tmp = strdup((const char *)unicode2latin(pwHash(config_file.readEntry("General", "Password"))));
	kdebug("Kadu::setStatus(): password = %s\n", tmp);
	free(tmp);
	loginparams.uin = (uin_t)config_file.readNumEntry("General", "UIN");
	loginparams.has_audio = config_file.readBoolEntry("Network", "AllowDCC");
	loginparams.last_sysmsg = config_file.readNumEntry("Global", "SystemMsgIndex");

	if (config_file.readBoolEntry("Network", "AllowDCC") && config_extip.ip4Addr() && config_file.readNumEntry("Network", "ExternalPort") > 1023) {
		loginparams.external_addr = htonl(config_extip.ip4Addr());
		loginparams.external_port = config_file.readNumEntry("Network", "ExternalPort");
		}
	else {
		loginparams.external_addr = 0;
		loginparams.external_port = 0;
		}
	if (config_servers.count() && !config_file.readBoolEntry("Network", "isDefServers") && config_servers[server_nr].ip4Addr()) {
		loginparams.server_addr = htonl(config_servers[server_nr].ip4Addr());
		loginparams.server_port = config_file.readNumEntry("Network", "DefaultPort");
		server_nr++;
		if (server_nr >= config_servers.count())
			server_nr = 0;
		}
	else {
		if (server_nr) {
			loginparams.server_addr = htonl(gg_servers[server_nr - 1].ip4Addr());
			loginparams.server_port = config_file.readNumEntry("Network", "DefaultPort");
			}			
		else {
			loginparams.server_addr = 0;
			loginparams.server_port = 0;
			}
		server_nr++;
		if (server_nr > gg_servers.count())
			server_nr = 0;
		}
//	polaczenia TLS z serwerami GG na razie nie dzialaja
//	loginparams.tls = config_file.readBoolEntry("Network", "UseTLS");
	loginparams.tls = 0;
	loginparams.client_version = GG_DEFAULT_CLIENT_VERSION;
	loginparams.protocol_version = GG_DEFAULT_PROTOCOL_VERSION;
	if (loginparams.tls) {
		kdebug("Kadu::setStatus(): using TLS\n");
		loginparams.server_port = 0;
		if (config_file.readBoolEntry("Network", "isDefServers"))
			loginparams.server_addr = 0;
		loginparams.server_port = 443;
		}
	else
		loginparams.server_port = config_file.readNumEntry("Network", "DefaultPort");
	ConnectionTimeoutTimer::on();
	ConnectionTimeoutTimer::connectTimeoutRoutine(&event_manager, SLOT(connectionTimeoutTimerSlot()));
	sess = gg_login(&loginparams);
//	free(loginparams.client_version);
	free(loginparams.password);
	if (loginparams.status_descr)
		free(loginparams.status_descr);

	AutoConnectionTimer::off();

	if (sess) {
		socket_active = true;

		kadusnw = new QSocketNotifier(sess->fd, QSocketNotifier::Write, this);
		QObject::connect(kadusnw, SIGNAL(activated(int)), kadu, SLOT(dataSent()));

		kadusnr = new QSocketNotifier(sess->fd, QSocketNotifier::Read, this);
		QObject::connect(kadusnr, SIGNAL(activated(int)), kadu, SLOT(dataReceived()));
		}
	else {
		disconnectNetwork();
		QMessageBox::warning(kadu, tr("Connection problem"),
			tr("Couldn't connect.\nCheck your internet connection."));
		}
}

void Kadu::dataReceived(void) {
	kdebug("Kadu::dataReceived()\n");
	if (sess->check && GG_CHECK_READ)
		event_manager.eventHandler(sess);
}

void Kadu::dataSent(void) {
	kdebug("Kadu::dataSent()\n");
	kadusnw->setEnabled(false);
	if (sess->check & GG_CHECK_WRITE)
		event_manager.eventHandler(sess);
}

void Kadu::pingNetwork(void) {
	kdebug("Kadu::pingNetwork()\n");
	gg_ping(sess);
	pingtimer->start(60000, TRUE);
}

void Kadu::disconnectNetwork() {
	unsigned int i;
	doBlink = false;

	kdebug("Kadu::disconnectNetwork(): calling offline routines\n");

	emit disconnectingNetwork();
	
	ConnectionTimeoutTimer::off();
	if (pingtimer) {
		pingtimer->stop();
		delete pingtimer;
		pingtimer = NULL;
		}
	if (blinktimer) {
		blinktimer->stop();
		delete blinktimer;
		blinktimer = NULL;
		}
	if (kadusnw) {
		kadusnw->setEnabled(false);
		delete kadusnw;
		kadusnw = NULL;
		}
	if (kadusnr) {
		kadusnr->setEnabled(false);
		delete kadusnr;
		kadusnr = NULL;
		}
	if (dccsnr) {
		delete dccsnr;
		dccsnr = NULL;
		}
	if (dccsnw) {
		delete dccsnw;
		dccsnw = NULL;
		}
	if (dccsock) {
		gg_dcc_free(dccsock);
		dccsock = NULL;
		gg_dcc_ip = 0;
		gg_dcc_port = 0;
		}

	if (sess) {
		gg_logoff(sess);
		gg_free_session(sess);
		sess = NULL;
		}
	userlist_sent = false;

	i = 0;
	while (i < userlist.count()) {
		userlist[i].status = GG_STATUS_NOT_AVAIL;
		userlist[i].description = "";
		i++;
		}

	chat_manager->refreshTitles();

//	own_description = QString::null;
	UserBox::all_refresh();

	socket_active = false;

	setCurrentStatus(GG_STATUS_NOT_AVAIL);
	
	emit disconnectedNetwork();
}


void Kadu::dccFinished(dccSocketClass *dcc) {
	kdebug("dccFinished\n");
	delete dcc;
}

bool Kadu::event(QEvent *e) {
	QCustomEvent *ce;
	dccSocketClass *dcc;
	dccSocketClass **data;

	if (e->type() == QEvent::User) {
		kdebug("Kadu::event()\n");
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
	kdebug("Kadu::dccReceived()\n");
	watchDcc();
}

void Kadu::dccSent(void) {
	kdebug("Kadu::dccSent()\n");
	dccsnw->setEnabled(false);
	if (dccsock->check & GG_CHECK_WRITE)
		watchDcc();
}

void Kadu::watchDcc(void) {
	kdebug("Kadu::watchDcc(): data on socket\n");
	if (!(dcc_e = gg_dcc_watch_fd(dccsock))) {
		kdebug("Kadu::watchDcc(): Connection broken unexpectedly!\n");
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
			kdebug("Kadu::watchDcc(): GG_EVENT_DCC_ERROR\n");
			break;
		case GG_EVENT_DCC_NEW:
			if (dccSocketClass::count < 8) {
				dccSocketClass *dcc;
				dcc = new dccSocketClass(dcc_e->event.dcc_new);
				connect(dcc, SIGNAL(dccFinished(dccSocketClass *)), this, SLOT(dccFinished(dccSocketClass *)));
				dcc->initializeNotifiers();
				kdebug("Kadu::watchDcc(): GG_EVENT_DCC_NEW: spawning object\n");
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
		kdebug("Kadu::close(): Kadu hide\n");
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
				QSize split;
				config_file.writeEntry("General", "UserBoxHeight", Userbox->size().height());
				config_file.writeEntry("General", "DescriptionHeight", descrtb->size().height());
			}
			if (config_file.readBoolEntry("Look", "ShowStatusButton"))
			{
				config_file.writeEntry("General", "UserBoxHeight", Userbox->size().height());
			}
			QRect geom;
			geom.setX(pos().x());
			geom.setY(pos().y());
			geom.setWidth(size().width());
			geom.setHeight(size().height());
	
			config_file.writeEntry("General", "Geometry",geom);
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
		if (config_file.readBoolEntry("General", "DisconnectWithDescription") && getActualStatus() != GG_STATUS_NOT_AVAIL) {
			kdebug("Kadu::close(): Set status NOT_AVAIL_DESCR with disconnect description(%s)\n",(const char *)config_file.readEntry("General", "DisconnectDescription").local8Bit());
			own_description = config_file.readEntry("General", "DisconnectDescription");
			setStatus(GG_STATUS_NOT_AVAIL_DESCR);
		}
		disconnectNetwork();
		kdebug("Kadu::close(): Saved config, disconnect and ignored\n");
		QWidget::close(true);
		flock(lockFileHandle, LOCK_UN);
		lockFile->close();
		delete lockFile;
		lockFile=NULL;
		kdebug("Kadu::close(): Graceful shutdown...\n");
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
	MainMenu->insertItem(tr("I&mport userlist"), this, SLOT(importUserlist()));
	MainMenu->insertItem(tr("E&xport userlist"), this, SLOT(exportUserlist()));
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
	GaduProtocol::initModule();
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
		descrtb->show();
	else
		descrtb->hide();
}

void Kadu::infopanelUpdate(uin_t uin) {
	if (!config_file.readBoolEntry("Look", "ShowInfoPanel"))
		return;
	kdebug("Kadu::infopanelUpdate(%d)\n", uin);
	if (Userbox->currentItem() != -1 && uin == userlist.byAltNick(Userbox->currentText()).uin)
		descrtb->setText(parse(config_file.readEntry("Look", "PanelContents"),userlist.byUin(uin)));
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

void KaduSlots::onCreateConfigDialog()
{
	kdebug("KaduSlots::onCreateConfigDialog() \n");
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
}

void KaduSlots::onDestroyConfigDialog()
{
	kdebug("KaduSlots::onDestroyConfigDialog() \n");
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

	int status = getActualStatus();

	bool privateStatus=config_file.readBoolEntry("General", "PrivateStatus");
	
	if (status != GG_STATUS_NOT_AVAIL)
	if ((!(status & GG_STATUS_FRIENDS_MASK)&& privateStatus)
		|| ((status & GG_STATUS_FRIENDS_MASK) && !privateStatus))
		kadu->setStatus(status & (~GG_STATUS_FRIENDS_MASK));

	statusppm->setItemChecked(8, privateStatus);


	/* I odswiez okno Kadu */
	kadu->changeAppearance();
	chat_manager->changeAppearance();
	kadu->refreshGroupTabBar();
	kadu->setCaption(tr("Kadu: %1").arg((uin_t)config_file.readNumEntry("General", "UIN")));

	QComboBox *cb_language= ConfigDialog::getComboBox("General", "Set language:");
	config_file.writeEntry("General", "Language", translateLanguage(qApp, cb_language->currentText(),false));
}

/*void Kadu::moveEvent(QMoveEvent *e)
{
//	kdebug("kadu::moveEvent: %d %d %d %d\n", x(), y(), width(), height());
	QWidget::moveEvent(e);
}*/
