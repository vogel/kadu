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
#include <qstyle.h>
#include <qstylefactory.h>

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
#include "chat.h"
#include "search.h"
#include "expimp.h"
#include "userinfo.h"
#include "personal_info.h"
#include "about.h"
#include "ignore.h"
#include "emoticons.h"
#include "history.h"
#include "pending_msgs.h"
#include "updates.h"
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
	kdebugf();
	setCloseMode(QDockWindow::Undocked);
	setLabel(qApp->translate("ToolBar", "Main toolbar"));

	config_file.addVariable("General", "ToolBarHidden", false);
	if (config_file.readBoolEntry("General", "ToolBarHidden"))
		hide();

	setVerticallyStretchable(true);
	setHorizontallyStretchable(true);

	createControls();
	instance=this;
	kdebugf2();
}

ToolBar::~ToolBar()
{
	config_file.writeEntry("General", "ToolBarHidden", isHidden());
	instance=NULL;
}

void ToolBar::createControls()
{
	kdebugf();
	for(QValueList<ToolButton>::iterator j=RegisteredToolButtons.begin(); j!=RegisteredToolButtons.end(); j++)
		if ((*j).caption== "--separator--")
			addSeparator();
		else
			(*j).button = new QToolButton((*j).iconfile, (*j).caption,
				QString::null, (*j).receiver, (*j).slot, this, (*j).name);

	setStretchableWidget(new QWidget(this));
	kdebugf2();
}

void ToolBar::registerSeparator(int position)
{
	kdebugf();
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
	kdebugf2();
}

void ToolBar::registerButton(const QIconSet& iconfile, const QString& caption,
			QObject* receiver, const char* slot, int position, const char* name)
{
	kdebugf();
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
	kdebugf2();
}

void ToolBar::unregisterButton(const char* name)
{
	kdebugf();
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
	kdebugf2();
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
	else if (HotKey::shortCut(e,"ShortCuts", "kadu_showonlydesc"))
		Userbox->showHideDescriptions();
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
	kdebugf();
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
	ConfigDialog::addCheckBox("General", "grid", QT_TRANSLATE_NOOP("@default", "Restore window geometry"), "SaveGeometry", true, QT_TRANSLATE_NOOP("@default", "Remember window size and position between startups"));
	ConfigDialog::addCheckBox("General", "grid", QT_TRANSLATE_NOOP("@default", "Check for updates"), "CheckUpdates", true, QT_TRANSLATE_NOOP("@default", "Automatically checks whether a new version is available"));

	ConfigDialog::addCheckBox("General", "grid", QT_TRANSLATE_NOOP("@default", "Private status"), "PrivateStatus", false, QT_TRANSLATE_NOOP("@default", "When enabled, you're visible only to users on your list"));

	ConfigDialog::addCheckBox("General", "grid", QT_TRANSLATE_NOOP("@default", "Show emoticons in panel"), "ShowEmotPanel", false);
	ConfigDialog::addCheckBox("General", "grid", QT_TRANSLATE_NOOP("@default", "Show emoticons in history"), "ShowEmotHist", false);
	ConfigDialog::addCheckBox("General", "grid", QT_TRANSLATE_NOOP("@default", "Force use of paragraphs (VERY experimental)"), "ForceUseParagraphs", false);

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
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Show / hide users without description"), "kadu_showonlydesc", "F10");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Configuration"), "kadu_configure", "F2");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Add user"), "kadu_adduser", "Ctrl+N");

	//zaladowanie wartosci domyslnych (pierwsze uruchomienie)
	config_file.addVariable("General", "UserBoxHeight", 300);
	config_file.addVariable("General", "DescriptionHeight", 60);

	QFontInfo info(qApp->font());
	QFont def_font(info.family(),info.pointSize());
	config_file.addVariable("Look", "UserboxFont", &def_font);
	config_file.addVariable("Look", "UserboxDescFont", &def_font);

	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "Look"));

	ConfigDialog::addComboBox("Look", "Look",
			QT_TRANSLATE_NOOP("@default","Qt Theme"));

	ConfigDialog::addCheckBox("Look", "varOpts", QT_TRANSLATE_NOOP("@default", "Show vertical scrollbar in information panel"), "PanelVerticalScrollbar", true);
	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Colors"));
		ConfigDialog::addVGroupBox("Look", "Colors", QT_TRANSLATE_NOOP("@default", "Main window"));
			ConfigDialog::addColorButton("Look", "Main window", QT_TRANSLATE_NOOP("@default", "Panel background color"), "InfoPanelBgColor", config_file.readColorEntry("Look","InfoPanelBgColor"), "", "panel_bg_color");
			ConfigDialog::addColorButton("Look", "Main window", QT_TRANSLATE_NOOP("@default", "Panel font color"), "InfoPanelFgColor", config_file.readColorEntry("Look","InfoPanelFgColor"), "", "panel_font_color");

	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Fonts"));
		ConfigDialog::addSelectFont("Look", "Fonts", QT_TRANSLATE_NOOP("@default", "Font in panel"), "PanelFont", def_font.toString(), "", "panel_font_box");

	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Information panel"));
		ConfigDialog::addCheckBox("Look", "Information panel", QT_TRANSLATE_NOOP("@default", "Show information panel"), "ShowInfoPanel", true);
		ConfigDialog::addTextEdit("Look", "Information panel", QT_TRANSLATE_NOOP("@default", "Information panel syntax:"), "PanelContents", "[#%u][, %f] %r [- %d] [ (%i)]", QT_TRANSLATE_NOOP("@default", "Syntax: %s - status, %d - description, %i - ip, %n - nick, %a - altnick, %f - frist name\n%r - surname, %m - mobile, %u - uin, %g - group, %o - return _space_ if user doesn't have us in userlist\n%v - revDNS, %p - port %e - email\nIf you leave blank, default settings will be used"));

	ConfigDialog::connectSlot("Look", "", SIGNAL(changed(const char *, const QColor&)), kaduslots, SLOT(chooseColor(const char *, const QColor&)), "panel_bg_color");
	ConfigDialog::connectSlot("Look", "", SIGNAL(changed(const char *, const QColor&)), kaduslots, SLOT(chooseColor(const char *, const QColor&)), "panel_font_color");

	ConfigDialog::connectSlot("Look", "Font in panel", SIGNAL(changed(const char *, const QFont&)),kaduslots, SLOT(chooseFont(const char *, const QFont&)), "panel_font_box");

	QVBox *vbox=new QVBox(this);
	setCentralWidget(vbox);
	QSplitter *split = new QSplitter(Qt::Vertical, vbox);
	QHBox* hbox1 = new QHBox(split);

	// groupbar
	GroupBar = new KaduTabBar(hbox1, "groupbar");
	GroupBar->setShape(QTabBar::RoundedBelow);
	GroupBar->addTab(new QTab(tr("All")));
	GroupBar->setFont(QFont(config_file.readFontEntry("Look", "UserboxFont").family(), config_file.readFontEntry("Look", "UserboxFont").pointSize(),75));
	hbox1->setStretchFactor(GroupBar, 1);
	connect(GroupBar, SIGNAL(selected(int)), this, SLOT(groupTabSelected(int)));

	// gadu, chat
	GaduProtocol::initModule();
	Chat::initModule();

	// userbox
	UserBox::initModule();
	Userbox = new UserBox(hbox1, "userbox");
	hbox1->setStretchFactor(Userbox, 100);
	connect(UserBox::userboxmenu, SIGNAL(popup()), this, SLOT(popupMenu()));
	connect(Userbox, SIGNAL(rightButtonClicked(QListBoxItem *, const QPoint &)),
		UserBox::userboxmenu, SLOT(show(QListBoxItem *)));
	connect(Userbox, SIGNAL(doubleClicked(QListBoxItem *)), this, SLOT(sendMessage(QListBoxItem *)));
	connect(Userbox, SIGNAL(returnPressed(QListBoxItem *)), this, SLOT(sendMessage(QListBoxItem *)));
	connect(Userbox, SIGNAL(mouseButtonClicked(int, QListBoxItem *, const QPoint &)),
		this, SLOT(mouseButtonClicked(int, QListBoxItem *)));
	connect(Userbox, SIGNAL(currentChanged(QListBoxItem *)), this, SLOT(currentChanged(QListBoxItem *)));
	UserBox::userboxmenu->addItem(tr("Open chat window") ,this, SLOT(openChat()));
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

	// history, hints
	History::initModule();

	closestatusppmtime.start();

	// blinktimer = NULL; zamieniamy na(powod: patrz plik events.cpp)
	blinktimer = NULL;

	/* blinker */
	BlinkOn = false;
	DoBlink = false;

	loadGeometry(this, "General", "Geometry", 0, 0, 145, 465);

	/* read the userlist */
	userlist.readFromFile();

	readIgnored();

	/* a newbie? */

	if (myUin)
		setCaption(tr("Kadu: %1").arg(myUin));

	pending.loadFromFile();

	/* connect userlist signals */
	connect(&userlist, SIGNAL(modified()), this, SLOT(userListModified()));
	connect(&userlist, SIGNAL(statusModified(UserListElement *, bool)), this, SLOT(userListStatusModified(UserListElement *, bool)));
	connect(&userlist, SIGNAL(userAdded(const UserListElement&)),this,SLOT(userListUserAdded(const UserListElement&)));

	/* add all users to userbox */
	setActiveGroup("");

	// dodanie przyciskow do paska narzedzi
	ToolBar::registerButton(icons_manager.loadIcon("ShowHideInactiveUsers"), tr("Show / hide inactive users"), Userbox, SLOT(showHideInactive()));
	ToolBar::registerButton(icons_manager.loadIcon("ShowOnlyDescriptionUsers"), tr("Show / hide users without description"), Userbox, SLOT(showHideDescriptions()));
	ToolBar::registerButton(icons_manager.loadIcon("Configuration"), tr("Configuration"), this, SLOT(configure()));
	ToolBar::registerSeparator();
	ToolBar::registerButton(icons_manager.loadIcon("History"), tr("View history"), this, SLOT(viewHistory()));
	ToolBar::registerButton(icons_manager.loadIcon("EditUserInfo"), tr("View/edit user info"), this, SLOT(showUserInfo()));
	ToolBar::registerButton(icons_manager.loadIcon("LookupUserInfo"), tr("Lookup in directory"), this, SLOT(lookupInDirectory()));
	ToolBar::registerSeparator();
	ToolBar::registerButton(icons_manager.loadIcon("AddUser"), tr("Add user"), this, SLOT(addUserAction()));

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

	connect(gadu, SIGNAL(chatMsgReceived2(UinsList, const QString &, time_t)),
		this, SLOT(chatMsgReceived(UinsList, const QString &, time_t)));
	connect(gadu, SIGNAL(connecting()), this, SLOT(connecting()));
	connect(gadu, SIGNAL(connected()), this, SLOT(connected()));
	connect(gadu, SIGNAL(disconnected()), this, SLOT(disconnected()));
	connect(gadu, SIGNAL(error(GaduError)), this, SLOT(error(GaduError)));
	connect(gadu, SIGNAL(goOnline(const QString &)), this, SLOT(wentOnline(const QString &)));
	connect(gadu, SIGNAL(goBusy(const QString &)), this, SLOT(wentBusy(const QString &)));
	connect(gadu, SIGNAL(goInvisible(const QString &)), this, SLOT(wentInvisible(const QString &)));
	connect(gadu, SIGNAL(goOffline(const QString &)), this, SLOT(wentOffline(const QString &)));
	connect(gadu, SIGNAL(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)),
		this, SLOT(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)));
	connect(gadu, SIGNAL(needTokenValue(QPixmap, QString &)),
		this, SLOT(readTokenValue(QPixmap, QString &)));
	connect(gadu, SIGNAL(systemMessageReceived(QString &)), this, SLOT(systemMessageReceived(QString &)));
	connect(gadu, SIGNAL(userListChanged()), this, SLOT(userListChanged()));
	connect(gadu, SIGNAL(userStatusChanged(UserListElement&, int, bool)),
		this, SLOT(userStatusChanged(UserListElement&, int, bool)));

	kdebugf2();
}

void Kadu::createToolBar()
{
	kdebugf();
	new ToolBar(this);
	setRightJustification(true);
//	setDockEnabled(Qt::DockBottom, false);
	setAppropriate(ToolBar::instance, true);
	kdebugf2();
}

void Kadu::popupMenu()
{
	kdebugf();
	UserList users;
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)//to siê zdarza...
	{
		kdebugf2();
		return;
	}
	users = activeUserBox->getSelectedUsers();
	UserListElement user = (*users.begin());

	bool isOurUin=users.containsUin(config_file.readNumEntry("General", "UIN"));

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
	kdebugf2();
}


void Kadu::configure()
{
	ConfigDialog::showConfigDialog(qApp);
}

void Kadu::viewHistory() {
	kdebugf();
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)
	{
		kdebugf2();
		return;
	}
	UinsList uins= activeUserBox->getSelectedUins();
	if (uins.count()) {
		History *hb = new History(uins);
		hb->show();
	}
	kdebugf2();
}

void Kadu::lookupInDirectory() {
	kdebugf();
	SearchDialog *sd;
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	UserList users;
	if (activeUserBox==NULL)
	{
		kdebugf2();
		return;
	}
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
	kdebugf2();
}

void Kadu::showUserInfo() {
	kdebugf();
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	UserList users;
	if (activeUserBox==NULL)
	{
		kdebugf2();
		return;
	}
	users = activeUserBox->getSelectedUsers();
	if (users.count() == 1)
	{
		UserInfo *ui = new UserInfo("user info", 0, (*users.begin()).altnick);
		ui->show();
	}
	kdebugf2();
}

void Kadu::deleteUsers()
{
	kdebugf();
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)
	{
		kdebugf2();
		return;
	}
	QStringList  users = activeUserBox->getSelectedAltNicks();
	removeUser(users, false);
	if (!Userbox->isSelected(Userbox->currentItem()))
		InfoPanel->setText("");
	kdebugf2();
}

void Kadu::personalInfo()
{
	(new PersonalInfoDialog())->show();
}

void Kadu::addUserAction() {
	(new UserInfo("add user", 0, QString::null, true))->show();
}

void Kadu::deleteHistory()
{
	kdebugf();
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)
	{
		kdebugf2();
		return;
	}
	history.removeHistory(activeUserBox->getSelectedUins());
	kdebugf2();
}

void Kadu::manageIgnored()
{
	(new Ignored(0, "ignored"))->show();
}

void Kadu::openChat()
{
	kdebugf();
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)
	{
		kdebugf2();
		return;
	}
	chat_manager->openPendingMsgs(activeUserBox->getSelectedUins());
	kdebugf2();
}

void Kadu::searchInDirectory()
{
	(new SearchDialog())->show();
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
	kdebugf();
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)
	{
		kdebugf2();
		return;
	}
	UinsList uins = activeUserBox->getSelectedUins();
	if (isIgnored(uins))
		delIgnored(uins);
	else
		addIgnored(uins);
	writeIgnored();
	kdebugf2();
}

void Kadu::blockUser()
{
	kdebugf();
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)
	{
		kdebugf2();
		return;
	}
	UserListElement *puser = &userlist.byAltNick((*activeUserBox->getSelectedUsers().begin()).altnick);
	puser->blocking = !puser->blocking;

	gadu->blockUser(puser->uin, puser->blocking);

	userlist.writeToFile();
	kdebugf2();
}

void Kadu::notifyUser()
{
	kdebugf();
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)
	{
		kdebugf2();
		return;
	}
	UserListElement *puser = &userlist.byAltNick((*activeUserBox->getSelectedUsers().begin()).altnick);
	puser->notify = !puser->notify;
	userlist.writeToFile();
	kdebugf2();
}

void Kadu::offlineToUser()
{
	kdebugf();
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)
	{
		kdebugf2();
		return;
	}
	UserListElement *puser = &userlist.byAltNick((*activeUserBox->getSelectedUsers().begin()).altnick);
	puser->offline_to_user = !puser->offline_to_user;

	gadu->offlineToUser(puser->uin, puser->offline_to_user);

	userlist.writeToFile();
	kdebugf2();
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
	kdebugf2();
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
	kdebugf2();
}

void Kadu::refreshGroupTabBar()
{
	kdebugf();
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
	kdebugf2();
}

void Kadu::setActiveGroup(const QString& group)
{
	kdebugf();
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
	kdebugf2();
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

void Kadu::userListStatusModified(UserListElement *user, bool onConnection)
{
	kdebugm(KDEBUG_FUNCTION_START, "Kadu::userListStatusModified(): %d\n", user->uin);
	if ((user->status == GG_STATUS_NOT_AVAIL)
		|| (user->status == GG_STATUS_NOT_AVAIL_DESCR))
		InfoPanel->setText("");
	chat_manager->refreshTitlesForUin(user->uin);
	kdebugf2();
}

void Kadu::userListChanged()
{
	kdebugf();
	UserBox::all_refresh();
	kdebugf2();
}

void Kadu::userStatusChanged(UserListElement &user, int oldstatus, bool onConnection)
{
	kdebugf();

	history.appendStatus(user.uin, user.status, user.description.length() ? user.description : QString::null);
	chat_manager->refreshTitlesForUin(user.uin);

	kdebugf2();
}

void Kadu::removeUser(QStringList &users, bool permanently = false)
{
	kdebugf();
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
			gadu->removeNotify(user.uin);
		userlist.removeUser(user.altnick);
		}

	userlist.writeToFile();
	refreshGroupTabBar();
	kdebugf2();
}

void Kadu::blink() {
	QPixmap pix;

	kdebugf();

	if (!DoBlink && !gadu->status().isOffline())
	{
		return;
	}
	else if (!DoBlink && gadu->status().isOffline())
	{
		pix = gadu->status().pixmap(Offline, false);
		statusbutton->setIconSet(QIconSet(pix));
		emit statusPixmapChanged(pix);
		return;
	}

	if (BlinkOn)
	{
		pix = gadu->status().pixmap(Offline, false);
		statusbutton->setIconSet(QIconSet(pix));
		emit statusPixmapChanged(pix);
	}
	else
	{
		pix = gadu->status().pixmap(status);
		statusbutton->setIconSet(QIconSet(pix));
		emit statusPixmapChanged(pix);
	}
	BlinkOn=!BlinkOn;

	blinktimer->start(1000, TRUE);
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
		gadu->addNotify(user.uin);
}

void Kadu::mouseButtonClicked(int button, QListBoxItem *item) {
	kdebugm(KDEBUG_FUNCTION_START, "Kadu::mouseButtonClicked(): button=%d\n", button);
	if (!item)
		InfoPanel->setText("");

	if (button !=4 || !item)
		return;
	kdebugf2();
}

/* if something's pending, open it, if not, open new message */
void Kadu::sendMessage(QListBoxItem *item)
{
	kdebugf();
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)
	{
		kdebugf2();
		return;
	}
	UinType uin = userlist.byAltNick(item->text()).uin;
	if (uin)
	{
		UinsList uins = activeUserBox->getSelectedUins();
		if (uins.count())
			if (uins.findIndex(config_file.readNumEntry("General", "UIN")) == -1)
				chat_manager->sendMessage(uin, uins);
	}
	kdebugf2();
}

/* when we want to change the status */
void Kadu::slotHandleState(int command) {
	kdebugf();
	ChooseDescription *cd;
	QString desc;

	status.setStatus(gadu->status());
	switch (command)
	{
		case 0:
			status.setOnline();
			break;
		case 1:
			cd = new ChooseDescription(1);
			if (cd->exec() == QDialog::Accepted)
			{
				cd->getDescription(desc);
				status.setOnline(desc);
			}
			delete cd;
			break;
		case 2:
			status.setBusy();
			break;
		case 3:
			cd = new ChooseDescription(3);
			if (cd->exec() == QDialog::Accepted)
			{
				cd->getDescription(desc);
				status.setBusy(desc);
			}
			delete cd;
			break;
		case 4:
			status.setInvisible();
			break;
		case 5:
			cd = new ChooseDescription(5);
			if (cd->exec() == QDialog::Accepted)
			{
				cd->getDescription(desc);
				status.setInvisible(desc);
			}
			delete cd;
			break;
		case 6:
			status.setOffline();
			break;
		case 7:
			cd = new ChooseDescription(7);
			if (cd->exec() == QDialog::Accepted)
			{
				cd->getDescription(desc);
				status.setOffline(desc);
			}
			delete cd;
			break;
		case 8:
			statusppm->setItemChecked(8, !statusppm->isItemChecked(8));
			dockppm->setItemChecked(8, !dockppm->isItemChecked(8));
			config_file.writeEntry("General", "PrivateStatus",statusppm->isItemChecked(8));
			status.setFriendsOnly(statusppm->isItemChecked(8));
			break;
	}

	bool stop = false;
	emit changingStatus(status, stop);
	if (!stop)
	{
		gadu->status().setStatus(status);

		if (status.isOffline())
		{
			Autohammer = false;
			gadu->disableAutoConnection();
			statusppm->setItemEnabled(7, false);
			dockppm->setItemEnabled(7, false);
		}
		else
			Autohammer = true;
	}
	else
		status.setStatus(gadu->status());

	kdebugf2();
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
	kdebugf2();
}

void Kadu::chatMsgReceived(UinsList senders, const QString &msg, time_t time)
{
	kdebugf();
//	UserListElement ule = userlist.byUinValue(senders[0]);

	pending.addMsg(senders, msg, GG_CLASS_CHAT, time);

	UserBox::all_refresh();

	if (config_file.readBoolEntry("General","AutoRaise"))
	{
		kadu->showNormal();
		kadu->setFocus();
	}

	if(config_file.readBoolEntry("Chat", "OpenChatOnMessage"))
		pending.openMessages();

	kdebugf2();
}

void Kadu::connected()
{
	kdebugf();

	DoBlink = false;

	kdebugf2();
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
			gadu->disableAutoConnection();
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
			gadu->disableAutoConnection();
			MessageBox::wrn(tr("Connection will be stoped\nYour password is incorrect !"));
			break;
			
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
			msg = QString(tr("Disconnection has occured"));
			break;

		default:
			kdebugm(KDEBUG_WARNING, "Unhandled error?\n");
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
		kdebugm(KDEBUG_INFO, "%s\n", msg.local8Bit().data());
		emit connectionError(msg);
	}

	if (Autohammer)
		gadu->enableAutoConnection();
	kdebugf2();
}

void Kadu::imageReceivedAndSaved(UinType sender, uint32_t size, uint32_t crc32, const QString &path)
{
	for (int i = 0; i < pending.count(); i++)
	{
		PendingMsgs::Element& e = pending[i];
		e.msg = gadu_images_manager.replaceLoadingImages(e.msg,sender,size,crc32);
	}
}

void Kadu::systemMessageReceived(QString &msg)
{
	MessageBox::msg(msg);
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
	gadu->disableAutoConnection();
	kdebugf2();
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
//		if (config_file.readBoolEntry("General", "DisconnectWithDescription") && gadu->getCurrentStatus() != GG_STATUS_NOT_AVAIL) {
		if (config_file.readBoolEntry("General", "DisconnectWithDescription") && !gadu->status().isOffline())
		{
			kdebugm(KDEBUG_INFO, "Kadu::close(): Set status NOT_AVAIL_DESCR with disconnect description(%s)\n",(const char *)config_file.readEntry("General", "DisconnectDescription").local8Bit());
			gadu->status().setOffline(config_file.readEntry("General", "DisconnectDescription"));
		}
//		disconnectNetwork();
//		gadu->logout();
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

	kdebugf2();
}

void Kadu::createMenu() {
	kdebugf();

	MenuBar = new QMenuBar(this, "MenuBar");

	MainMenu = new QPopupMenu(this, "MainMenu");
	MainMenu->insertItem(tr("Manage &ignored"), this, SLOT(manageIgnored()));
	MainMenu->insertItem(icons_manager.loadIcon("Configuration"), tr("&Configuration"), this, SLOT(configure()),HotKey::shortCutFromFile("ShortCuts", "kadu_configure"));
	MainMenu->insertSeparator();

	personalInfoMenuId=MainMenu->insertItem(tr("Personal information"), this,SLOT(personalInfo()));
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
	kdebugf2();
}

void Kadu::statusMenuAboutToHide() {
	closestatusppmtime.restart();
}

void Kadu::createStatusPopupMenu() {
	kdebugf();

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
	kdebugf2();
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
	kdebugf2();
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

void KaduSlots::onCreateConfigDialog()
{
	kdebugf();
	QLineEdit *e_password=ConfigDialog::getLineEdit("General", "Password");
	e_password->setEchoMode(QLineEdit::Password);
	e_password->setText(pwHash(config_file.readEntry("General", "Password", "")));

	QComboBox *cb_qttheme=ConfigDialog::getComboBox("Look", "Qt Theme");
	QStringList sl_themes=QStyleFactory::keys();
	cb_qttheme->insertStringList(sl_themes);
	if(!sl_themes.contains(QApplication::style().name()))
		cb_qttheme->setCurrentText(tr("Unknown"));
	else
		cb_qttheme->setCurrentText(QApplication::style().name());

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

	int statusIndex = config_file.readNumEntry("General", "DefaultStatusIndex", -1);

	// BEGIN: wsteczna kompatybilno¶æ, do wywalenia w 0.5.x
	if (statusIndex == -1)
	{
		statusIndex = config_file.readNumEntry("General", "DefaultStatus", -1);
		switch (statusIndex)
		{
			case 0x0001: statusIndex = Status::index(Offline, false); break;
			case 0x0015: statusIndex = Status::index(Offline, true); break;
			case 0x0002: statusIndex = Status::index(Online, false); break;
			case 0x0004: statusIndex = Status::index(Online, true); break;
			case 0x0003: statusIndex = Status::index(Busy, false); break;
			case 0x0005: statusIndex = Status::index(Busy, true); break;
			case 0x0014: statusIndex = Status::index(Invisible, false); break;
			case 0x0016: statusIndex = Status::index(Invisible, true); break;
			default:
				statusIndex = -1;
		}
	}
	if (statusIndex == -1)
		statusIndex = Status::index(Offline, false);
	// END: wsteczna kombatybilno¶æ, do wywalenia w 0.5.x

	int max = Status::initCount();
	QComboBox* cb_defstatus = ConfigDialog::getComboBox("General", "Default status", "cb_defstatus");
	cb_defstatus->clear();
	for (int i = 0; i < max; i++)
		cb_defstatus->insertItem(qApp->translate("@default", Status::name(i)));
	cb_defstatus->setCurrentItem(statusIndex);

	updatePreview();
	kdebugf2();
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

	QComboBox* cb_defstatus = ConfigDialog::getComboBox("General", "Default status", "cb_defstatus");
	config_file.writeEntry("General", "DefaultStatusIndex", cb_defstatus->currentItem());

	bool privateStatus = config_file.readBoolEntry("General", "PrivateStatus");
	gadu->status().setFriendsOnly(privateStatus);

	statusppm->setItemChecked(8, privateStatus);

	/* I od¶wie¿ okno Kadu */
	kadu->changeAppearance();
	chat_manager->changeAppearance();
	kadu->refreshGroupTabBar();
	kadu->setCaption(tr("Kadu: %1").arg((UinType)config_file.readNumEntry("General", "UIN")));

	QComboBox *cb_language= ConfigDialog::getComboBox("General", "Set language:");
	config_file.writeEntry("General", "Language", translateLanguage(qApp, cb_language->currentText(),false));

	QString new_style=ConfigDialog::getComboBox("Look", "Qt Theme")->currentText();
	if(new_style!=tr("Unknown") && new_style != QApplication::style().name()){
		QApplication::setStyle(new_style);
		config_file.writeEntry("Look", "QtStyle", new_style);
	}

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
		kdebugm(KDEBUG_ERROR, "chooseColor: label '%s' not known\n", name);
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
	kdebugf();
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
				emit wantRegister();
				break;
			case 2: // Nothing
				break;
		}
		setCaption(tr("Kadu: new user"));
	}

	QString descr = defaultdescriptions.first();
	int statusIndex = config_file.readNumEntry("General", "DefaultStatusIndex", -1);
	// BEGIN: wsteczna kompatybilno¶æ, do wywalenia w 0.5.x
	if (statusIndex == -1)
	{
		statusIndex = config_file.readNumEntry("General", "DefaultStatus", -1);
		switch (statusIndex)
		{
			case 0x0001: statusIndex = Status::index(Offline, false); break;
			case 0x0015: statusIndex = Status::index(Offline, true); break;
			case 0x0002: statusIndex = Status::index(Online, false); break;
			case 0x0004: statusIndex = Status::index(Online, true); break;
			case 0x0003: statusIndex = Status::index(Busy, false); break;
			case 0x0005: statusIndex = Status::index(Busy, true); break;
			case 0x0014: statusIndex = Status::index(Invisible, false); break;
			case 0x0016: statusIndex = Status::index(Invisible, true); break;
			default:
				statusIndex = -1;
		}
	}
	// END: wsteczna kombatybilno¶æ, do wywalenia w 0.5.x

	if (statusIndex == -1)
		status.setOffline(descr);
	else
		status.setIndex(statusIndex, descr);
	status.setFriendsOnly(config_file.readBoolEntry("General", "PrivateStatus"));

	if (!status.isOffline())
	{
		Autohammer = true;
		gadu->status().setStatus(status);
	}

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
	for(int i = 0; i < 8; i++)
	{
		statusppm->setItemChecked(i, false);
		dockppm->setItemChecked(i, false);
	}
	statusppm->setItemChecked(statusNr, true);
	dockppm->setItemChecked(statusNr, true);
	statusppm->setItemChecked(8, gadu->status().isFriendsOnly());
	dockppm->setItemChecked(8, gadu->status().isFriendsOnly());

	statusbutton->setText(qApp->translate("@default", statustext[statusNr]));
	statusppm->setItemEnabled(7, statusNr != 6);
	dockppm->setItemEnabled(7, statusNr != 6);
	QPixmap pix = gadu->status().pixmap();
	statusbutton->setIconSet(QIconSet(pix));
	setIcon(pix);
	UserBox::all_refresh();

	emit statusPixmapChanged(pix);
}

void Kadu::readTokenValue(QPixmap tokenImage, QString &tokenValue)
{
	TokenDialog *td = new TokenDialog(tokenImage);

	if (td->exec() == QDialog::Accepted)
		td->getValue(tokenValue);
	else
		tokenValue = "";

	delete td;
}
