/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qdir.h>
#include <qfile.h>
#include <qfont.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qregexp.h>
#include <qstring.h>
#include <qstyle.h>
#include <qstylefactory.h>
#include <qtextstream.h>
#include <qtextcodec.h>
#include <qtimer.h>
#include <qtoolbar.h>
#include <qwidget.h>

#include <sys/file.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <stdlib.h>
#include <sys/stat.h>
//
#include "about.h"
#include "chat.h"
#include "config_dialog.h"
#include "config_file.h"
#include "debug.h"
#include "emoticons.h"
#include "expimp.h"
#include "gadu.h"
#include "history.h"
#include "ignore.h"
#include "kadu.h"
#include "message_box.h"
#include "modules.h"
#include "pending_msgs.h"
#include "personal_info.h"
#include "search.h"
#include "tabbar.h"
#include "userinfo.h"
#include "updates.h"

#include "kadu-config.h"

ConfigFile config_file(ggPath(QString("kadu.conf")));

static QTimer* blinktimer;
QPopupMenu* dockMenu;

int lockFileHandle;
QFile *lockFile;

const QString Kadu::SyntaxText=QT_TRANSLATE_NOOP("@default", "Syntax: %s - status, %d - description, %i - ip, %n - nick, %a - altnick, %f - first name\n%r - surname, %m - mobile, %u - uin, %g - group, %o - return _space_ if user doesn't have us in userlist\n%v - revDNS, %p - port %e - email %x - max image size\n");

QValueList<ToolBar::ToolButton> ToolBar::RegisteredToolButtons;
ToolBar* ToolBar::instance=NULL;

ToolBar::ToolBar(QMainWindow* parent) : QToolBar(parent, "mainToolbar")
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
	FOREACH(j, RegisteredToolButtons)
		if ((*j).caption== "--separator--")
			addSeparator();
		else
			(*j).button = new QToolButton(icons_manager.loadIcon((*j).iconname), (*j).caption,
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

void ToolBar::registerButton(const QString &iconname, const QString& caption,
			QObject* receiver, const char* slot, int position, const char* name)
{
	kdebugf();
	if(instance!=NULL)
		instance->clear();

	ToolButton RToolButton;

	RToolButton.iconname= iconname;
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

	FOREACH(j, RegisteredToolButtons)
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
	FOREACH(j, RegisteredToolButtons)
		if ((*j).name == name)
			return (*j).button;
	kdebugmf(KDEBUG_WARNING, "'%s' return NULL\n", name?name:"[null]");
	return NULL;
}

void ToolBar::refreshIcons(const QString &caption, const QString &newIconName, const QString &newCaption)
{
	kdebugf();
	if (caption==QString::null) //wszystkie siê od¶wie¿aj±
	{
		FOREACH(j, RegisteredToolButtons)
			if ((*j).caption!="--separator--")
				(*j).button->setIconSet(icons_manager.loadIcon((*j).iconname));
		if (kadu->isVisible())
		{
			kadu->hide();
			kadu->show();
		}
	}
	else
		FOREACH(j, RegisteredToolButtons)
			if ((*j).caption == caption)
			{
				if (newIconName!=QString::null)
					(*j).iconname=newIconName;
				(*j).button->setIconSet(icons_manager.loadIcon((*j).iconname));
				if (newCaption!=QString::null)
				{
					(*j).caption=newCaption;
					(*j).button->setTextLabel(newCaption);
				}
				break;
			}
	kdebugf2();
}

void Kadu::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Key_Escape && Docked)
	{
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
	Docked = false;
	ShowMainWindowOnStart = true;

	KaduSlots *kaduslots=new KaduSlots(this, "kaduslots");
	UinType myUin=config_file.readNumEntry("General", "UIN");

	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "General"), "GeneralTab");
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
	config_file.addVariable("General", "ParagraphSeparator", 4);
#ifdef DEBUG_ENABLED
	ConfigDialog::addLineEdit("General", "General", QT_TRANSLATE_NOOP("@default", "Debugging mask"), "DEBUG_MASK");
#endif

	ConfigDialog::addVGroupBox("General", "General", "Status");
	ConfigDialog::addComboBox("General", "Status", QT_TRANSLATE_NOOP("@default", "Default status"), "", "cb_defstatus");
	ConfigDialog::addHBox("General", "Status", "discstatus");
	ConfigDialog::addCheckBox("General", "discstatus", QT_TRANSLATE_NOOP("@default", "On shutdown, set description:"), "DisconnectWithDescription", false);
	ConfigDialog::addLineEdit("General", "discstatus", "", "DisconnectDescription", "", "", "e_defaultstatus");
	ConfigDialog::addSpinBox("General", "Status", QT_TRANSLATE_NOOP("@default", "Number of kept descriptions"), "NumberOfDescriptions", 1, 30, 1, 4);

	ConfigDialog::registerSlotOnCreate(kaduslots, SLOT(onCreateConfigDialog()));
	ConfigDialog::registerSlotOnApply(kaduslots, SLOT(onDestroyConfigDialog()));

	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "ShortCuts"), "ShortCutsTab");
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

	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "Look"), "LookTab");

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
		ConfigDialog::addTextEdit("Look", "Information panel", QT_TRANSLATE_NOOP("@default", "Information panel syntax:"), "PanelContents", "[#%u][, %f] %r [- %d] [ (%i)]", SyntaxText);

	ConfigDialog::connectSlot("Look", "", SIGNAL(changed(const char *, const QColor&)), kaduslots, SLOT(chooseColor(const char *, const QColor&)), "panel_bg_color");
	ConfigDialog::connectSlot("Look", "", SIGNAL(changed(const char *, const QColor&)), kaduslots, SLOT(chooseColor(const char *, const QColor&)), "panel_font_color");

	ConfigDialog::connectSlot("Look", "Font in panel", SIGNAL(changed(const char *, const QFont&)),kaduslots, SLOT(chooseFont(const char *, const QFont&)), "panel_font_box");

	QVBox *vbox=new QVBox(this, "centralBox");
	setCentralWidget(vbox);
	QSplitter *split = new QSplitter(Qt::Vertical, vbox, "splitter");
	QHBox* hbox1 = new QHBox(split, "firstBox");

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
	connect(Userbox, SIGNAL(rightButtonPressed(QListBoxItem *, const QPoint &)),
		UserBox::userboxmenu, SLOT(show(QListBoxItem *)));
	connect(Userbox, SIGNAL(doubleClicked(const QString &)), this, SLOT(sendMessage(const QString &)));
	connect(Userbox, SIGNAL(returnPressed(const QString &)), this, SLOT(sendMessage(const QString &)));
	connect(Userbox, SIGNAL(mouseButtonClicked(int, QListBoxItem *, const QPoint &)),
		this, SLOT(mouseButtonClicked(int, QListBoxItem *)));
	connect(Userbox, SIGNAL(currentChanged(QListBoxItem *)), this, SLOT(currentChanged(QListBoxItem *)));
	UserBox::userboxmenu->addItem("OpenChat", tr("Open chat window") ,this, SLOT(openChat()));
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

	/* add all users to userbox */
	setActiveGroup("");

	// dodanie przyciskow do paska narzedzi
	ToolBar::registerButton("ShowHideInactiveUsers", tr("Show / hide inactive users"), Userbox, SLOT(showHideInactive()), -1, "inactiveUsersButton");
	ToolBar::registerButton("ShowOnlyDescriptionUsers", tr("Show / hide users without description"), Userbox, SLOT(showHideDescriptions()), -1, "withDescriptionUsersButton");
	ToolBar::registerButton("Configuration", tr("Configuration"), this, SLOT(configure()), -1, "configurationButton");
	ToolBar::registerSeparator();
	ToolBar::registerButton("History", tr("View history"), this, SLOT(viewHistory()), -1, "historyButton");
	ToolBar::registerButton("EditUserInfo", tr("View/edit user info"), this, SLOT(showUserInfo()), -1, "editUserButton");
	ToolBar::registerButton("LookupUserInfo", tr("Lookup in directory"), this, SLOT(lookupInDirectory()), -1, "lookupUserButton");
	ToolBar::registerSeparator();
	ToolBar::registerButton("AddUser", tr("Add user"), this, SLOT(addUserAction()), -1, "addUserButton");

	/* guess what */
	createMenu();
	createStatusPopupMenu();

	connect(statusMenu, SIGNAL(aboutToHide()), this, SLOT(statusMenuAboutToHide()));

	dockMenu->insertSeparator();
	dockMenu->insertItem(icons_manager.loadIcon("Exit"), tr("&Exit Kadu"), 9);
	icons_manager.registerMenuItem(dockMenu, tr("&Exit Kadu"), "Exit");

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
	QObject::connect(&userlist, SIGNAL(dnsNameReady(UinType)), this, SLOT(infopanelUpdate(UinType)));

	statusButton = new QPushButton(QIconSet(icons_manager.loadIcon("Offline")), tr("Offline"), vbox, "statusButton");
	statusButton->setPopup(statusMenu);

	if (!config_file.readBoolEntry("Look", "ShowStatusButton"))
		statusButton->hide();

	QValueList<int> splitsizes;

	splitsizes.append(config_file.readNumEntry("General", "UserBoxHeight"));
	splitsizes.append(config_file.readNumEntry("General", "DescriptionHeight"));

	split->setSizes(splitsizes);

//	tworzymy pasek narzedziowy
	createToolBar();
	if (config_file.readEntry("General", "DockWindows") != QString::null)
	{
		QString dockwindows=config_file.readEntry("General", "DockWindows").replace(QRegExp("\\\\n"), "\n");
		QTextStream stream(&dockwindows, IO_ReadOnly);
		stream >> *this;
	}

	refreshGroupTabBar();
	int configTab = config_file.readNumEntry( "Look", "CurrentGroupTab" );
	if ( configTab >= 0 && configTab < GroupBar -> count() )
		((QTabBar*) GroupBar) -> setCurrentTab( configTab );

	connect(&userlist, SIGNAL(userDataChanged(const UserListElement * const, const UserListElement * const)),
		gadu, SLOT(userDataChanged(const UserListElement * const, const UserListElement * const)));

	connect(gadu, SIGNAL(chatMsgReceived2(UinsList, const QString &, time_t)),
		this, SLOT(chatMsgReceived(UinsList, const QString &, time_t)));
	connect(gadu, SIGNAL(connecting()), this, SLOT(connecting()));
	connect(gadu, SIGNAL(connected()), this, SLOT(connected()));
	connect(gadu, SIGNAL(disconnected()), this, SLOT(disconnected()));
	connect(gadu, SIGNAL(error(GaduError)), this, SLOT(error(GaduError)));
	connect(gadu, SIGNAL(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)),
		this, SLOT(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)));
	connect(gadu, SIGNAL(needTokenValue(QPixmap, QString &)),
		this, SLOT(readTokenValue(QPixmap, QString &)));
	connect(gadu, SIGNAL(systemMessageReceived(QString &)), this, SLOT(systemMessageReceived(QString &)));
	connect(gadu, SIGNAL(userListChanged()), this, SLOT(userListChanged()));
	connect(gadu, SIGNAL(userStatusChanged(const UserListElement&, const UserStatus &, bool)),
		this, SLOT(userStatusChanged(const UserListElement&, const UserStatus &, bool)));

	connect(&(gadu->currentStatus()), SIGNAL(goOnline(const QString &)),
		this, SLOT(wentOnline(const QString &)));
	connect(&(gadu->currentStatus()), SIGNAL(goBusy(const QString &)),
		this, SLOT(wentBusy(const QString &)));
	connect(&(gadu->currentStatus()), SIGNAL(goInvisible(const QString &)),
		this, SLOT(wentInvisible(const QString &)));
	connect(&(gadu->currentStatus()), SIGNAL(goOffline(const QString &)),
		this, SLOT(wentOffline(const QString &)));

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

	if (!user.uin() || isOurUin)
	{
		UserBox::userboxmenu->setItemEnabled(ignoreuseritem, false);
		UserBox::userboxmenu->setItemEnabled(blockuseritem, false);
		UserBox::userboxmenu->setItemEnabled(notifyuseritem, false);
		UserBox::userboxmenu->setItemEnabled(offlinetouseritem, false);
	}
	else
	{
		UinsList uins;
		uins = activeUserBox->getSelectedUins();
		if (isIgnored(uins))
			UserBox::userboxmenu->setItemChecked(ignoreuseritem, true);
		if (user.blocking())
			UserBox::userboxmenu->setItemChecked(blockuseritem, true);
		UserBox::userboxmenu->setItemEnabled(offlinetouseritem, config_file.readBoolEntry("General", "PrivateStatus"));
		if (user.offlineTo())
			UserBox::userboxmenu->setItemChecked(offlinetouseritem, true);
		UserBox::userboxmenu->setItemEnabled(notifyuseritem, !config_file.readBoolEntry("Notify", "NotifyAboutAll"));
		if (user.notify())
			UserBox::userboxmenu->setItemChecked(notifyuseritem, true);
	}

	int deletehistoryitem = UserBox::userboxmenu->getItem(tr("Clear history"));
	int historyitem = UserBox::userboxmenu->getItem(tr("View history"));
	int searchuser = UserBox::userboxmenu->getItem(tr("Lookup in directory"));
	if (!user.uin() || isOurUin)
	{
		UserBox::userboxmenu->setItemEnabled(deletehistoryitem, false);
		UserBox::userboxmenu->setItemEnabled(historyitem, false);
	}
	if (users.count() != 1 || !user.uin())
		UserBox::userboxmenu->setItemEnabled(searchuser, false);
	if (users.count() != 1)
		UserBox::userboxmenu->setItemEnabled(UserBox::userboxmenu->getItem(tr("View/edit user info")), false);
	if (!user.uin() || isOurUin)
		UserBox::userboxmenu->setItemEnabled(UserBox::userboxmenu->getItem(tr("Open chat window")), false);
	kdebugf2();
}


void Kadu::configure()
{
	ConfigDialog::showConfigDialog(qApp);
}

void Kadu::viewHistory()
{
	kdebugf();
	UserBox *activeUserBox=UserBox::getActiveUserBox();

	if (activeUserBox==NULL)
	{	
		kdebugf2();
		return;
	}
	UinsList uins = activeUserBox->getSelectedUins();
	(new History(uins))->show();
	kdebugf2();
}

void Kadu::lookupInDirectory()
{
	kdebugf();
	SearchDialog *sd;
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)
	{
		kdebugf2();
		return;
	}
	UserList users = activeUserBox->getSelectedUsers();
	if (users.count() == 1)
	{
		sd = new SearchDialog(0, tr("User info"), (*(users.begin())).uin());
		sd->show();
		sd->firstSearch();
	}
	else
	{
		sd = new SearchDialog();
		sd->show();
	}
	kdebugf2();
}

void Kadu::showUserInfo()
{
	kdebugf();
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)
	{
		kdebugf2();
		return;
	}
	UserList users = activeUserBox->getSelectedUsers();
	if (users.count() == 1)
		(new UserInfo((*users.begin()).altNick(), false, 0, "user info"))->show();
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
	QStringList users = activeUserBox->getSelectedAltNicks();
	removeUser(users, false);
	if (!Userbox->isSelected(Userbox->currentItem()))
		InfoPanel->setText("");
	kdebugf2();
}

void Kadu::personalInfo()
{
	(new PersonalInfoDialog())->show();
}

void Kadu::addUserAction()
{
	(new UserInfo(QString::null, true, 0, "add user"))->show();
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
	openWebBrowser("http://www.kadu.net/doc.php");
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

// ca³y ten szmelc do optymalizacji
void Kadu::blockUser()
{
	kdebugf();
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)
	{
		kdebugf2();
		return;
	}

	UserListElement puser = userlist.byAltNick((*activeUserBox->getSelectedUsers().begin()).altNick());
	puser.setBlocking(!puser.blocking());

	userlist.changeUserInfo(puser.altNick(), puser);
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
	UserListElement puser = userlist.byAltNick((*activeUserBox->getSelectedUsers().begin()).altNick());
	puser.setNotify(!puser.notify());

	userlist.changeUserInfo(puser.altNick(), puser);
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
	UserListElement puser = userlist.byAltNick((*activeUserBox->getSelectedUsers().begin()).altNick());
	puser.setOfflineTo(!puser.offlineTo());

	userlist.changeUserInfo(puser.altNick(), puser);
	userlist.writeToFile();
	kdebugf2();
}

void Kadu::changeAppearance()
{
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

	QPixmap pix=gadu->status().pixmap(status);
	statusButton->setIconSet(QIconSet(pix));
	emit statusPixmapChanged(pix);
	kdebugf2();
}

void Kadu::currentChanged(QListBoxItem *item)
{
	if (!item || !item->isSelected())
		return;

	kdebugmf(KDEBUG_INFO, "%s\n", item->text().local8Bit().data());

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
	for (UserList::ConstIterator i = userlist.begin(); i != userlist.end(); ++i)
	{
		QString groups = (*i).group();
		QString group;
		for (int g = 0; (group = groups.section(',' ,g ,g)) != ""; ++g)
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
	for (int i = GroupBar->count() - 1; i >= 1; --i)
		if(!group_list.contains(GroupBar->tabAt(i)->text()))
			GroupBar->removeTab(GroupBar->tabAt(i));
	/* dodajemy nowe zakladki */
	for (unsigned int i = 0; i < group_list.count(); ++i)
	{
		bool createNewTab = true;
		for (int j = 0; j < GroupBar->count(); ++j)
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
	for (UserList::ConstIterator i = userlist.begin(); i != userlist.end(); ++i)
	{
		bool belongsToGroup = group.isEmpty();
		if (!belongsToGroup)
		{
			QString user_groups = (*i).group();
			QString user_group;
			for (int g = 0; (user_group = user_groups.section(',',g,g)) != ""; ++g)
				if (user_group == group)
				{
					belongsToGroup = true;
					break;
				}
		}
		if (belongsToGroup && (!(*i).isAnonymous() || !Docked))
			Userbox->addUser((*i).altNick());
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

void Kadu::userListChanged()
{
	kdebugf();
	UserBox::all_refresh();
	kdebugf2();
}

void Kadu::userStatusChanged(const UserListElement &user, const UserStatus &/*oldstatus*/, bool onConnection)
{
	kdebugf();

	history.appendStatus(user.uin(), user.status());
	chat_manager->refreshTitlesForUin(user.uin());
	if (user.status().isOffline())
		InfoPanel->setText("");
	if (!onConnection)//refresh zrobimy jak ju¿ ca³± listê przetworzymy, czyli w userListChanged()
		UserBox::all_refresh();

	kdebugf2();
}

void Kadu::removeUser(QStringList &users, bool /*permanently*/)
{
	kdebugf();
	if(QMessageBox::warning(kadu, "Kadu",
		tr("Selected users will be deleted. Are you sure?"),
		tr("&Yes"),tr("&No"))!=0)
		return;

	unsigned int i;

	for (i = 0; i < users.count(); ++i)
		UserBox::all_removeUser(users[i]);
	UserBox::all_refresh();

	for (i = 0; i < users.count(); ++i)
	{
		UserListElement user = userlist.byAltNick(users[i]);
		userlist.removeUser(user.altNick());
	}

	userlist.writeToFile();
	refreshGroupTabBar();
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
		emit statusPixmapChanged(pix);
		return;
	}

	if (BlinkOn)
		pix = gadu->status().pixmap(Offline, false);
	else
		pix = gadu->status().pixmap(status);

	statusButton->setIconSet(QIconSet(pix));
	emit statusPixmapChanged(pix);

	BlinkOn=!BlinkOn;

	blinktimer->start(1000, TRUE);
}

void Kadu::mouseButtonClicked(int button, QListBoxItem *item)
{
	kdebugmf(KDEBUG_FUNCTION_START, "button=%d\n", button);
	if (!item)
		InfoPanel->setText("");
	kdebugf2();
}

/* if something's pending, open it, if not, open new message */
void Kadu::sendMessage(const QString &to)
{
	kdebugf();
	UserBox *activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)
	{
		kdebugf2();
		return;
	}
	UinType uin = userlist.byAltNick(to).uin();
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
void Kadu::slotHandleState(int command)
{
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
			statusMenu->setItemChecked(8, !statusMenu->isItemChecked(8));
			dockMenu->setItemChecked(8, !dockMenu->isItemChecked(8));
			config_file.writeEntry("General", "PrivateStatus",statusMenu->isItemChecked(8));
			status.setFriendsOnly(statusMenu->isItemChecked(8));
			break;
	}

	bool stop = false;
	emit changingStatus(status, stop);
	if (!stop)
	{
		gadu->status().setStatus(status);

		if (status.isOffline())
		{
			statusMenu->setItemEnabled(7, false);
			dockMenu->setItemEnabled(7, false);
		}
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
		blinktimer = new QTimer(this, "blinktimer");
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

	bool continue_connecting = true;
	switch (err)
	{
		case ConnectionServerNotFound:
			msg = tr("Unable to connect, server has not been found");
			break;

		case ConnectionCannotConnect:
			msg = tr("Unable to connect");
			break;

		case ConnectionNeedEmail:
			msg = tr("Please change your email in \"Change password/email\" window. "
				"Leave new password field blank.");
			continue_connecting = false;
			MessageBox::msg(msg);
			break;

		case ConnectionInvalidData:
			msg = tr("Unable to connect, server has returned unknown data");
			break;

		case ConnectionCannotRead:
			msg = tr("Unable to connect, connection break during reading");
			break;

		case ConnectionCannotWrite:
			msg = tr("Unable to connect, connection break during writing");
			break;

		case ConnectionIncorrectPassword:
			msg = tr("Unable to connect, incorrect password");
			continue_connecting = false;
			MessageBox::wrn(tr("Connection will be stoped\nYour password is incorrect !"));
			break;

		case ConnectionTlsError:
			msg = tr("Unable to connect, error of negotiation TLS");
			break;

		case ConnectionUnknow:
			kdebugm(KDEBUG_INFO, "Connection broken unexpectedly!\nUnscheduled connection termination\n");
			break;

		case ConnectionTimeout:
			msg = tr("Connection timeout!");
			break;

		case Disconnected:
			msg = tr("Disconnection has occured");
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

	// je¶li b³±d który wyst±pi³ umo¿liwia dalsze próby po³±czenia
	// i w miêdzyczasie u¿ytkownik nie zmieni³ statusu na niedostêpny
	// to za sekundê próbujemy ponownie
	if (continue_connecting && !gadu->status().isOffline())
		gadu->connectAfterOneSecond();

	kdebugf2();
}

void Kadu::imageReceivedAndSaved(UinType sender, uint32_t size, uint32_t crc32, const QString &/*path*/)
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
	kdebugmf(KDEBUG_FUNCTION_START, "Disconnection has occured\n");

	chat_manager->refreshTitles();
	UserBox::all_refresh();

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
	if (!quit && Docked)
	{
		kdebugmf(KDEBUG_INFO, "hiding\n");
		hide();
		return false;
	}
	else
	{
		chat_manager->closeAllWindows();
		ConfigDialog::closeDialog();
		ModulesManager::closeModule();
		Updates::deactivateModule();

		if (config_file.readBoolEntry("General", "SaveGeometry"))
		{
			if (config_file.readBoolEntry("Look", "ShowInfoPanel"))
			{
				config_file.writeEntry("General", "UserBoxHeight", Userbox->size().height());
				config_file.writeEntry("General", "DescriptionHeight", InfoPanel->size().height());
			}
			if (config_file.readBoolEntry("Look", "ShowStatusButton"))
				config_file.writeEntry("General", "UserBoxHeight", Userbox->size().height());
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
		if (config_file.readBoolEntry("General", "DisconnectWithDescription") && !gadu->status().isOffline())
		{
			kdebugmf(KDEBUG_INFO, "Set status NOT_AVAIL_DESCR with disconnect description(%s)\n",(const char *)config_file.readEntry("General", "DisconnectDescription").local8Bit());
			gadu->status().setOffline(config_file.readEntry("General", "DisconnectDescription"));
		}
//		disconnectNetwork();
//		gadu->logout();
		kdebugmf(KDEBUG_INFO, "Saved config, disconnect and ignored\n");
		QWidget::close(true);
		flock(lockFileHandle, LOCK_UN);
		lockFile->close();
		delete lockFile;
		lockFile=NULL;
		kdebugmf(KDEBUG_INFO, "Graceful shutdown...\n");
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
	kdebugf2();
}

void Kadu::createMenu()
{
	kdebugf();
	MenuBar = new QMenuBar(this, "MenuBar");

	MainMenu = new QPopupMenu(this, "MainMenu");
	MainMenu->insertItem(icons_manager.loadIcon("ManageIgnored"), tr("Manage &ignored"), this, SLOT(manageIgnored()));
	MainMenu->insertItem(icons_manager.loadIcon("Configuration"), tr("&Configuration"), this, SLOT(configure()),HotKey::shortCutFromFile("ShortCuts", "kadu_configure"));
	MainMenu->insertSeparator();

	personalInfoMenuId=MainMenu->insertItem(icons_manager.loadIcon("PersonalInfo"), tr("Personal information"), this,SLOT(personalInfo()));
	MainMenu->insertSeparator();
	MainMenu->insertItem(icons_manager.loadIcon("LookupUserInfo"), tr("&Search for users"), this, SLOT(searchInDirectory()));
	MainMenu->insertItem(icons_manager.loadIcon("ImportExport"), tr("I&mport / Export userlist"), this, SLOT(importExportUserlist()));
	MainMenu->insertItem(icons_manager.loadIcon("AddUser"), tr("&Add user"), this, SLOT(addUserAction()),HotKey::shortCutFromFile("ShortCuts", "kadu_adduser"));
	MainMenu->insertSeparator();
	MainMenu->insertItem(icons_manager.loadIcon("HelpMenuItem"), tr("H&elp"), this, SLOT(help()));
	MainMenu->insertItem(icons_manager.loadIcon("AboutMenuItem"), tr("A&bout..."), this, SLOT(about()));
	MainMenu->insertSeparator();
	MainMenu->insertItem(icons_manager.loadIcon("HideKadu"), tr("&Hide Kadu"), this, SLOT(hideKadu()));
	MainMenu->insertItem(icons_manager.loadIcon("Exit"), tr("&Exit Kadu"), this, SLOT(quit()));

	MenuBar->insertItem(tr("&Kadu"), MainMenu);

	icons_manager.registerMenu(MainMenu);
	icons_manager.registerMenuItem(MainMenu, tr("Manage &ignored"), "ManageIgnored");
	icons_manager.registerMenuItem(MainMenu, tr("&Configuration"), "Configuration");
	icons_manager.registerMenuItem(MainMenu, tr("Personal information"), "PersonalInfo");
	icons_manager.registerMenuItem(MainMenu, tr("&Search for users"), "LookupUserInfo");
	icons_manager.registerMenuItem(MainMenu, tr("I&mport / Export userlist"), "ImportExport");
	icons_manager.registerMenuItem(MainMenu, tr("&Add user"), "AddUser");
	icons_manager.registerMenuItem(MainMenu, tr("H&elp"), "HelpMenuItem");
	icons_manager.registerMenuItem(MainMenu, tr("A&bout..."), "AboutMenuItem");
	icons_manager.registerMenuItem(MainMenu, tr("&Hide Kadu"), "HideKadu");
	icons_manager.registerMenuItem(MainMenu, tr("&Exit Kadu"), "Exit");
	kdebugf2();
}

void Kadu::statusMenuAboutToHide()
{
}

void Kadu::createStatusPopupMenu()
{
	kdebugf();

	QPixmap pix;
	QIconSet icon;

	statusMenu = new QPopupMenu(this, "statusMenu");
	dockMenu = new QPopupMenu(this, "dockMenu");

	icons_manager.registerMenu(statusMenu);
	icons_manager.registerMenu(dockMenu);

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
		statusMenu->insertItem(icon, qApp->translate("@default", UserStatus::name(i)), i);
		dockMenu->insertItem(icon, qApp->translate("@default", UserStatus::name(i)), i);

		icons_manager.registerMenuItem(statusMenu, qApp->translate("@default", UserStatus::name(i)), UserStatus::toString(s->status(), s->hasDescription()));
		icons_manager.registerMenuItem(dockMenu, qApp->translate("@default", UserStatus::name(i)), UserStatus::toString(s->status(), s->hasDescription()));
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

void Kadu::infopanelUpdate(UinType uin)
{
	if (!config_file.readBoolEntry("Look", "ShowInfoPanel"))
		return;
	kdebugmf(KDEBUG_INFO, "%d\n", uin);
	if (Userbox->currentItem() != -1 && uin == userlist.byAltNick(Userbox->currentText()).uin())
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
			case 0x0001: statusIndex = UserStatus::index(Offline, false); break;
			case 0x0015: statusIndex = UserStatus::index(Offline, true); break;
			case 0x0002: statusIndex = UserStatus::index(Online, false); break;
			case 0x0004: statusIndex = UserStatus::index(Online, true); break;
			case 0x0003: statusIndex = UserStatus::index(Busy, false); break;
			case 0x0005: statusIndex = UserStatus::index(Busy, true); break;
			case 0x0014: statusIndex = UserStatus::index(Invisible, false); break;
			case 0x0016: statusIndex = UserStatus::index(Invisible, true); break;
			default:
				statusIndex = -1;
		}
	}
	if (statusIndex == -1)
		statusIndex = UserStatus::index(Offline, false);
	// END: wsteczna kombatybilno¶æ, do wywalenia w 0.5.x

	int max = UserStatus::initCount();
	QComboBox* cb_defstatus = ConfigDialog::getComboBox("General", "Default status", "cb_defstatus");
	cb_defstatus->clear();
	for (int i = 0; i < max; ++i)
		cb_defstatus->insertItem(qApp->translate("@default", UserStatus::name(i)));
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
		kadu->statusButton->show();
	else
		kadu->statusButton->hide();

	if (config_file.readBoolEntry("Look", "MultiColumnUserbox"))
		kadu->userbox()->setColumnMode(QListBox::FitToWidth);
	else
		kadu->userbox()->setColumnMode(1);

	config_file.writeEntry("General", "DefaultStatusIndex",
		ConfigDialog::getComboBox("General", "Default status", "cb_defstatus")->currentItem());

	bool privateStatus = config_file.readBoolEntry("General", "PrivateStatus");
	gadu->status().setFriendsOnly(privateStatus);

	kadu->statusMenu->setItemChecked(8, privateStatus);

	/* I od¶wie¿ okno Kadu */
	kadu->changeAppearance();
	chat_manager->changeAppearance();
	kadu->refreshGroupTabBar();

	if (!config_file.readBoolEntry("Look", "DisplayGroupTabs"))
	{

		kadu->groupBar()->setCurrentTab(kadu->groupBar()->tabAt(0));
		config_file.writeEntry("Look", "CurrentGroupTab", 0);
		kadu->setActiveGroup("");
	}

	kadu->setCaption(tr("Kadu: %1").arg((UinType)config_file.readNumEntry("General", "UIN")));

	QComboBox *cb_language= ConfigDialog::getComboBox("General", "Set language:");
	config_file.writeEntry("General", "Language", translateLanguage(qApp, cb_language->currentText(),false));

	QString new_style=ConfigDialog::getComboBox("Look", "Qt Theme")->currentText();
	if(new_style!=tr("Unknown") && new_style != QApplication::style().name())
	{
		QApplication::setStyle(new_style);
		config_file.writeEntry("Look", "QtStyle", new_style);
	}
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

KaduSlots::KaduSlots(QObject *parent, const char *name) : QObject(parent, name)
{
}

void Kadu::resizeEvent(QResizeEvent *)
{
//	Userbox->all_refresh();
}

/*void Kadu::moveEvent(QMoveEvent *e)
{
//	kdebugmf(KDEBUG_INFO, "%d %d %d %d\n", x(), y(), width(), height());
	QWidget::moveEvent(e);
}*/

void Kadu::startupProcedure()
{
	kdebugf();
	if (ShowMainWindowOnStart)
		show();

	QString path_;
	path_ = ggPath("");
	mkdir(path_.local8Bit().data(), 0700);
	path_.append("/history/");
	mkdir(path_.local8Bit().data(), 0700);

/*	if (!config_file.readNumEntry("General","UIN"))
	{
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
*/
	Updates::initModule();

	QString descr = defaultdescriptions.first();
	int statusIndex = config_file.readNumEntry("General", "DefaultStatusIndex", -1);
	// BEGIN: wsteczna kompatybilno¶æ, do wywalenia w 0.5.x
	if (statusIndex == -1)
	{
		statusIndex = config_file.readNumEntry("General", "DefaultStatus", -1);
		switch (statusIndex)
		{
			case 0x0001: statusIndex = UserStatus::index(Offline, false); break;
			case 0x0015: statusIndex = UserStatus::index(Offline, true); break;
			case 0x0002: statusIndex = UserStatus::index(Online, false); break;
			case 0x0004: statusIndex = UserStatus::index(Online, true); break;
			case 0x0003: statusIndex = UserStatus::index(Busy, false); break;
			case 0x0005: statusIndex = UserStatus::index(Busy, true); break;
			case 0x0014: statusIndex = UserStatus::index(Invisible, false); break;
			case 0x0016: statusIndex = UserStatus::index(Invisible, true); break;
			default:
				statusIndex = -1;
		}
	}
	// END: wsteczna kombatybilno¶æ, do wywalenia w 0.5.x

	if (statusIndex == -1)
		status.setOffline();
	else
		status.setIndex(statusIndex, descr);
	status.setFriendsOnly(config_file.readBoolEntry("General", "PrivateStatus"));

	gadu->status().setStatus(status);

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

	statusButton->setText(qApp->translate("@default", gadu->status().name()));
	statusMenu->setItemEnabled(7, statusNr != 6);
	dockMenu->setItemEnabled(7, statusNr != 6);
	QPixmap pix = gadu->status().pixmap();
	statusButton->setIconSet(QIconSet(pix));
	setIcon(pix);
	UserBox::all_refresh();

	emit statusPixmapChanged(pix);
}

void Kadu::readTokenValue(QPixmap tokenImage, QString &tokenValue)
{
	TokenDialog *td = new TokenDialog(tokenImage, 0, "token_dialog");

	if (td->exec() == QDialog::Accepted)
		td->getValue(tokenValue);
	else
		tokenValue = "";

	delete td;
}
