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
#include "sms.h"
#include "about.h"
#include "ignore.h"
#include "hints.h"
#include "emoticons.h"
#include "history.h"
#include "pending_msgs.h"
#include "dock_widget.h"
#include "updates.h"
#include "password.h"
#include "tabbar.h"
#include "debug.h"
#include "sound.h"
#include "../config.h"


#define GG_USER_OFFLINE	0x01
#define	GG_USER_NORMAL	0x03
#define GG_USER_BLOCKED	0x04

bool userlist_sent = FALSE;
bool socket_active = FALSE;
bool disconnect_planned = FALSE;
int userlist_count = 0;
int last_ping;
int last_read_event = -1;
int server_nr = 0;
int muteitem;
bool timeout_connected = true;
bool i_wanna_be_invisible = true;
struct gg_event *e;

QTime closestatusppmtime;
QTimer *blinktimer;
QTimer *pingtimer;
//QTimer *readevent; - patrz plik events.cpp
QPopupMenu *statusppm;
QPopupMenu *dockppm;
QLabel *statuslabel;
QLabel *statuslabeltxt;
QPopupMenu *grpmenu;

QHostAddress config_dccip;
QHostAddress config_extip;
QValueList<QHostAddress> config_servers;

QValueList<struct chats> chats;
struct gg_session *sess = NULL;

struct gg_dcc * dccsock;
struct gg_login_params loginparams;
QSocketNotifier *kadusnr = NULL;
QSocketNotifier *kadusnw = NULL;
QSocketNotifier *dccsnr = NULL;
QSocketNotifier *dccsnw = NULL;
UpdatesClass *uc;

QValueList<QHostAddress> gg_servers;
const char *gg_servers_ip[7] = {"217.17.41.82", "217.17.41.83", "217.17.41.84", "217.17.41.85",
	"217.17.41.86", "217.17.41.87", "217.17.41.88"};

/* sends the userlist. ripped off EKG, actually, but works */
void sendUserlist() {
	uin_t *uins;
	char *types;
	int i, j;

	for (i = 0, j = 0; i < userlist.count(); i++)
		if (userlist[i].uin)
			j++;

	uins = (uin_t *) malloc(j * sizeof(uin_t));
	types = (char *) malloc(j * sizeof(char));

	for (i = 0, j = 0; i < userlist.count(); i++)
		if (userlist[i].uin && !userlist[i].anonymous) {
			uins[j] = userlist[i].uin;
			if (userlist[i].offline_to_user)
				types[j] = GG_USER_OFFLINE;
			else
				if (userlist[i].blocking)
					types[j] = GG_USER_BLOCKED;
				else
					types[j] = GG_USER_NORMAL;
			j++;
			}

	/* we were popping up sometimes, so let's keep the server informed */
	if (i_wanna_be_invisible)
		gg_change_status(sess, GG_STATUS_INVISIBLE);

	gg_notify_ex(sess, uins, types, j);
	kdebug("send_userlist(): Userlist sent\n");

	free(uins);
}

void Kadu::gotUpdatesInfo(const QByteArray &data, QNetworkOperation *op) {
	char buf[32];
	int i;
	QString newestversion;

	if (data.size() > 31) {
		kdebug("Kadu::gotUpdatesInfo(): cannot obtain update info\n");		
		delete uc;
		return;
		}
	for (i = 0; i < data.size(); i++)
		buf[i] = data[i];
	buf[data.size()] = 0;
	newestversion = buf;

	kdebug("Kadu::gotUpdatesInfo(): %s\n", buf);

	if (uc->ifNewerVersion(newestversion)) {
		QMessageBox::information(this, tr("Update information"),
			tr("The newest Kadu version is %1").arg(newestversion), QMessageBox::Ok);
		}
	delete uc;
}

void Kadu::keyPressEvent(QKeyEvent *e) {
	if (e->key() == Key_Escape && trayicon) {
		kdebug("Kadu::keyPressEvent(Key_Escape): Kadu hide\n");
		hide();
	}
	else if (HotKey::shortCut(e,"kadu_deleteuser"))
	{
	if (userbox->isSelected(userbox->currentItem()))
		deleteUsers();
	}
	else if (HotKey::shortCut(e,"kadu_persinfo"))
	{
	if (userbox->isSelected(userbox->currentItem()))
	        showUserInfo();
	}	
	else if (HotKey::shortCut(e,"kadu_sendsms"))
	{
		sendSmsToUser();
	}
	else if (HotKey::shortCut(e,"kadu_viewhistory"))
	{
		viewHistory();
	}	
	else if (HotKey::shortCut(e,"kadu_searchuser"))
	{
		lookupInDirectory();
	}	
	else if (HotKey::shortCut(e,"kadu_showinactive"))
	{
		showHideInactive();
	}
	else if (HotKey::shortCut(e, "kadu_voicechat")) {
		makeVoiceChat();
		}
	else if (HotKey::shortCut(e,"kadu_sendfile"))
	{
		sendFile();
	}		
	else if (HotKey::shortCut(e,"kadu_configure"))
	{
		configure();
	}	

	
	QWidget::keyPressEvent(e);
}

/* a monstrous constructor so Kadu would take longer to start up */
Kadu::Kadu(QWidget *parent, const char *name) : QMainWindow(parent, name)
{

	//zachowanie wstecznej kompatybilnosci ze starym konfigiem
	//mysle ze zaraz po wydaniu 0.3.5 powinno to zniknac
	config_file.addVariable("General", "UIN", config_file.readEntry("Global", "UIN"));
	config_file.addVariable("General", "Password", config_file.readEntry("Global", "Password"));
	config_file.addVariable("General", "Nick", config_file.readEntry("Global", "Nick"));
	config_file.addVariable("General", "Geometry", config_file.readEntry("Global", "Geometry"));
	config_file.addVariable("General", "DefaultStatus", config_file.readNumEntry("Global", "DefaultStatus", GG_STATUS_NOT_AVAIL));
	config_file.addVariable("General", "DefaultDescription", config_file.readEntry("Global", "DefaultDescription",tr("I am busy.")));
	config_file.addVariable("General", "ShowHideInactive", config_file.readBoolEntry("Other", "ShowHideInactive", true));
	config_file.addVariable("General", "OpenChatOnMessage", config_file.readBoolEntry("Other", "OpenChatOnMessage", false));
	config_file.addVariable("General", "AutoSend", config_file.readBoolEntry("Other", "AutoSend", false));
	config_file.addVariable("General", "DisconnectWithDescription", config_file.readBoolEntry("Other", "DisconnectWithDescription", false));
	config_file.addVariable("General", "DisconnectDescription", config_file.readEntry("Other", "DisconnectDescription"));
	config_file.addVariable("General", "DockWindows", config_file.readEntry("Global", "DockWindows", QString::null));
	config_file.addVariable("General", "SplitSize", config_file.readEntry("Global", "SplitSize"));
	config_file.addVariable("Sounds", "Message_sound", config_file.readEntry("Global", "Message_sound"));
	config_file.addVariable("Sounds", "Chat_sound", config_file.readEntry("Global", "Chat_sound"));
	config_file.addVariable("General", "Logging", config_file.readBoolEntry("Global", "Logging", true));
	config_file.addVariable("General", "SystemMsgIndex", config_file.readEntry("Global", "SystemMsgIndex"));
	config_file.addVariable("General", "SaveGeometry", config_file.readBoolEntry("Global", "SaveGeometry", true));
	config_file.addVariable("Sounds", "PlaySoundChat", config_file.readBoolEntry("Global", "PlaySoundChat", true));
	config_file.addVariable("Sounds", "PlaySoundChatInvisible", config_file.readBoolEntry("Global", "PlaySoundChatInvisible", true));
	config_file.addVariable("Sounds", "SoundPlayer", config_file.readEntry("Global", "SoundPlayer"));
	config_file.addVariable("Sounds", "PlaySound", config_file.readEntry("Global", "PlaySound"));
	config_file.addVariable("Sounds", "PlaySoundArtsDsp", config_file.readBoolEntry("Global", "PlaySoundArtsDsp", false));
	config_file.addVariable("Sounds", "SoundVolume", config_file.readDoubleNumEntry("Global", "SoundVolume")*100);
	config_file.addVariable("Sounds", "VolumeControl", config_file.readBoolEntry("Global", "VolumeControl", false));
	config_file.addVariable("General", "AutoAway", config_file.readBoolEntry("Global", "AutoAway", false));
	config_file.addVariable("General", "AutoAwayTime", config_file.readNumEntry("Global", "AutoAwayTime", 300));
	config_file.addVariable("General", "UseDocking", config_file.readBoolEntry("Global", "UseDocking", true));
	config_file.addVariable("General", "RunDocked", config_file.readBoolEntry("Global", "RunDocked", false));
	config_file.addVariable("General", "AutoRaise", config_file.readBoolEntry("Global", "AutoRaise", false));
	config_file.addVariable("General", "PrivateStatus", config_file.readBoolEntry("Global", "PrivateStatus", false));
	config_file.addVariable("General", "CheckUpdates", config_file.readBoolEntry("Global", "CheckUpdates", true));
	config_file.addVariable("Look", "DisplayGroupTabs", config_file.readBoolEntry("Global", "DisplayGroupTabs", true));
	config_file.addVariable("General", "AddToDescription", config_file.readBoolEntry("Global", "AddToDescription", false));
	config_file.addVariable("Look", "ShowDesc", config_file.readBoolEntry("Global", "ShowDesc", true));
	config_file.addVariable("Look", "MultiColumnUserbox", config_file.readBoolEntry("Global", "MultiColumnUserbox", true));
	config_file.addVariable("Network", "UseProxy", config_file.readBoolEntry("Proxy", "UseProxy", false));
	config_file.addVariable("Network", "ProxyHost", config_file.readEntry("Proxy", "ProxyHost"));
	config_file.addVariable("Network", "ProxyPort", config_file.readNumEntry("Proxy", "ProxyPort"));
	config_file.addVariable("Network", "ProxyUser", config_file.readEntry("Proxy", "ProxyUser"));
	config_file.addVariable("Network", "ProxyPassword", config_file.readEntry("Proxy", "ProxyPassword"));
	config_file.addVariable("Network", "AllowDCC", config_file.readBoolEntry("Global", "AllowDCC", false));
	config_file.addVariable("Network", "DccIP", config_file.readEntry("Global", "DccIP"));
	config_file.addVariable("Network", "DccForwarding", config_file.readBoolEntry("Global", "DccForwarding", false));
	config_file.addVariable("Network", "ExternalIP", config_file.readEntry("Global", "ExternalIP"));
	config_file.addVariable("Network", "ExternalPort", config_file.readEntry("Global", "ExternalPort"));
	config_file.addVariable("Network", "isDefServers", config_file.readEntry("Global", "isDefServers"));
	config_file.addVariable("Network", "Server", config_file.readEntry("Global", "Server"));
	config_file.addVariable("Network", "UseTLS", config_file.readBoolEntry("Global", "UseTLS", false));
	config_file.addVariable("Network", "DefaultPort", config_file.readNumEntry("Global", "DefaultPort",8074));
	config_file.addVariable("History", "ChatHistoryQuotationTime",-config_file.readNumEntry("Other", "ChatHistoryQuotationTime",336));
	config_file.addVariable("History", "ChatHistoryQuotation", config_file.readNumEntry("Other", "ChatHistoryQuotation", 10));
	config_file.addVariable("Chat", "ChatPrune", config_file.readBoolEntry("Other", "ChatPrune", false));
	config_file.addVariable("Chat", "ChatPruneLen", config_file.readNumEntry("Other", "ChatPruneLen", 20));
	config_file.addVariable("Chat", "EmoticonsStyle", config_file.readNumEntry("Other", "ChatEmoticonsStyle", EMOTS_ANIMATED));
	config_file.addVariable("Chat", "EmoticonsTheme", config_file.readEntry("Other", "ChatEmoticonsTheme"));
	config_file.addVariable("Chat", "ScrollDown", config_file.readBoolEntry("Other", "ScrollDown", true));
	config_file.addVariable("Chat", "IgnoreAnonymousUser", config_file.readBoolEntry("Other", "IgnoreAnonymousUser", false));
	config_file.addVariable("Chat", "Encryption", config_file.readBoolEntry("Other", "Encryption", false));
	config_file.addVariable("Chat", "MessageAcks", config_file.readBoolEntry("Other", "MessageAcks", true));
	config_file.addVariable("Chat", "BlinkChatTitle", config_file.readBoolEntry("Other", "BlinkChatTitle", true));
	config_file.addVariable("Chat", "DefaultWebBrowser", config_file.readBoolEntry("WWW", "DefaultWebBrowser", true));
	config_file.addVariable("Chat", "WebBrowser", config_file.readEntry("WWW", "WebBrowser"));
	config_file.addVariable("Look", "PanelContents", 
	config_file.readEntry("Other", "PanelContents", "[#%u][, %f] %r [- %d] [ (%i)]"));
	config_file.addVariable("Look", "ConferenceContents", config_file.readEntry("Other", "ConferenceContents", "%a (%s[: %d])"));
	config_file.addVariable("Look", "ChatContents", config_file.readEntry("Other", "ChatContents"));
	config_file.addVariable("Look", "ConferencePrefix", config_file.readEntry("Other", "ConferencePrefix"));

	QColor color;
	color=QColor("#FFFFFF");
	config_file.addVariable("Look", "UserboxBgColor", config_file.readColorEntry("Colors", "UserboxBgColor",&color));
	color=QColor("#000000");
	config_file.addVariable("Look", "UserboxFgColor", config_file.readColorEntry("Colors", "UserboxFgColor", &color));    
	color=QColor("#E0E0E0");
	config_file.addVariable("Look", "ChatMyBgColor", config_file.readColorEntry("Colors", "ChatMyBgColor", &color));
	color=QColor("#F0F0F0");
	config_file.addVariable("Look", "ChatUsrBgColor", config_file.readColorEntry("Colors", "ChatUsrBgColor", &color));
	color=QColor("#000000");
	config_file.addVariable("Look", "ChatMyFontColor", config_file.readColorEntry("Colors", "ChatMyFontColor", &color));
	color=QColor("#000000");
	config_file.addVariable("Look", "ChatUsrFontColor", config_file.readColorEntry("Colors", "ChatUsrFontColor", &color));
	color=QColor("#C0C0C0");
	config_file.addVariable("Look", "UserboxDescBgColor", config_file.readColorEntry("Colors", "UserboxDescBgColor", &color));
	color=QColor("#000000");
	config_file.addVariable("Look", "UserboxDescTextColor", config_file.readColorEntry("Colors", "UserboxDescTextColor", &color));

	QFontInfo info0(a->font());
	QFont def_font0(info0.family(),info0.pointSize());
	config_file.addVariable("Look", "UserboxFont", config_file.readFontEntry("Fonts", "UserboxFont", &def_font0));
	config_file.addVariable("Look", "ChatFont", config_file.readFontEntry("Fonts", "ChatFont", &def_font0));
	config_file.addVariable("Look", "UserboxDescFont", config_file.readFontEntry("Fonts", "UserboxDescFont", &def_font0));
	//koniec wstecznej kompatybilnosci
	//

	//potrzebne do translacji
	QT_TRANSLATE_NOOP("@default", "General");
	QT_TRANSLATE_NOOP("@default", "User data");
	QT_TRANSLATE_NOOP("@default", "Uin");
	QT_TRANSLATE_NOOP("@default", "Password");
	QT_TRANSLATE_NOOP("@default", "Nick");
	QT_TRANSLATE_NOOP("@default", "Log messages");
	QT_TRANSLATE_NOOP("@default", "Restore window geometry");
	QT_TRANSLATE_NOOP("@default", "Check for updates");
	QT_TRANSLATE_NOOP("@default", "Set language:");

	KaduSlots *kaduslots=new KaduSlots();
	
	ConfigDialog::registerTab("General");
	ConfigDialog::addHGroupBox("General", "General", "User data");
	ConfigDialog::addLineEdit("General", "User data", "Uin", "UIN", "0");
	ConfigDialog::addLineEdit("General", "User data", "Password", "Password", "");
	ConfigDialog::addLineEdit("General", "User data", "Nick", "Nick", tr("Me"));
	ConfigDialog::addComboBox("General", "General", "Set language:");
	ConfigDialog::addGrid("General", "General", "grid", 3);
	ConfigDialog::addCheckBox("General", "grid", "Log messages", "Logging", true);
	ConfigDialog::addCheckBox("General", "grid", "Restore window geometry", "SaveGeometry", true);
	ConfigDialog::addCheckBox("General", "grid", "Check for updates", "CheckUpdates", true);

	Sms::initModule();
	Chat::initModule();
	UserBox::initModule();
	History::initModule();
	HintManager::initModule();
	AutoAwayTimer::initModule();
	SoundSlots::initModule();
	EventConfigSlots::initModule();


	ConfigDialog::registerSlotOnCreate(kaduslots, SLOT(onCreateConfigDialog()));
	ConfigDialog::registerSlotOnDestroy(kaduslots, SLOT(onDestroyConfigDialog()));

	//potrzebne do translacji
	QT_TRANSLATE_NOOP("@default", "ShortCuts");
	QT_TRANSLATE_NOOP("@default", "Define keys");
	QT_TRANSLATE_NOOP("@default", "Remove from userlist");
	QT_TRANSLATE_NOOP("@default", "View/edit user info");
	QT_TRANSLATE_NOOP("@default", "Send SMS");
	QT_TRANSLATE_NOOP("@default", "View history");
	QT_TRANSLATE_NOOP("@default", "Lookup in directory");
	QT_TRANSLATE_NOOP("@default", "Show / hide inactive users");
	QT_TRANSLATE_NOOP("@default", "Send file");
	QT_TRANSLATE_NOOP("@default", "Configuration");
	QT_TRANSLATE_NOOP("@default", "Add user");

	ConfigDialog::registerTab("ShortCuts");
	ConfigDialog::addVGroupBox("ShortCuts", "ShortCuts", "Define keys");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", "Remove from userlist", "kadu_deleteuser", "Del");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", "View/edit user info", "kadu_persinfo", "Ins");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", "Send SMS", "kadu_sendsms", "Ctrl+S");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", "View history", "kadu_viewhistory", "Ctrl+H");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", "Lookup in directory", "kadu_searchuser", "Ctrl+F");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", "Show / hide inactive users", "kadu_showinactive", "F9");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", "Voice chat", "kadu_voicechat", "F7");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", "Send file", "kadu_sendfile", "F8");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", "Configuration", "kadu_configure", "F2");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", "Add user", "kadu_adduser", "Ctrl+N");


	//zaladowanie wartosci domyslnych (pierwsze uruchomienie)
	QRect def_rect(0, 0, 145, 465);
	config_file.addVariable("General", "Geometry", def_rect);
	QSize def_size(340, 60);
	config_file.addVariable("General", "SplitSize", def_size);

	QFontInfo info(a->font());
	QFont def_font(info.family(),info.pointSize());
	config_file.addVariable("Look", "UserboxFont", &def_font);
	config_file.addVariable("Look", "UserboxDescFont", &def_font);
	
	closestatusppmtime.start();
	lastsoundtime.start();

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

	/* active group, 600 is all groups */
	activegrpno = 600;


        
	QRect geom;
	geom=config_file.readRectEntry("General", "Geometry");
	kdebug("Setting size: width=%d, height=%d and setting position: x=%d, y=%d\n",
		geom.width(),geom.height(),
		geom.x(), geom.y());
	resize(geom.width(),geom.height());
	move(geom.x(),geom.y());

	if (config_file.readBoolEntry("General", "UseDocking")) {
		trayicon = new TrayIcon(this);
		trayicon->show();
		trayicon->changeIcon();
		}

	if (config_file.readBoolEntry("Hints", "Hints"))
		hintmanager = new HintManager();

	/* read the userlist */
	userlist.readFromFile();

	readIgnored();

	/* a newbie? */

	if (config_file.readNumEntry("General", "UIN"))
		setCaption(tr("Kadu: %1").arg(config_file.readNumEntry("General", "UIN")));

	pending.loadFromFile();

	if (config_file.readBoolEntry("General", "UseDocking"))
		trayicon->changeIcon();

	centralFrame = new QFrame(this);
	setCentralWidget(centralFrame);

	QSplitter *split = new QSplitter(Qt::Vertical, centralFrame);

	QHBox *hbox1 = new QHBox(split);
	/* initialize group tabbar */
//	QVBox *vbox1 = new QVBox(hbox1);
//	vbox1->setMinimumWidth(10);
	group_bar = new KaduTabBar(hbox1, "groupbar");
	group_bar->setShape(QTabBar::RoundedBelow);
	group_bar->addTab(new QTab(tr("All")));
	group_bar->setFont(QFont(config_file.readFontEntry("Look", "UserboxFont").family(), config_file.readFontEntry("Look", "UserboxFont").pointSize(),75));
//	group_bar->setMinimumWidth(group_bar->sizeHint().width());
	connect(group_bar, SIGNAL(selected(int)), this, SLOT(groupTabSelected(int)));

//	QHBox *hbox2 = new QHBox(vbox1);
//	hbox2->setMinimumWidth(10);

//	vbox1->setStretchFactor(group_bar, 1);
//	vbox1->setStretchFactor(hbox2, 100);

	/* connect userlist modified signal */
	connect(&userlist, SIGNAL(modified()), this, SLOT(userListModified()));
	connect(&userlist, SIGNAL(statusModified(UserListElement *)), this, SLOT(userListStatusModified(UserListElement *)));

	/* initialize and configure userbox */
	userbox = new UserBox(hbox1, "userbox");
	if (config_file.readBoolEntry("Look", "MultiColumnUserbox"))
		userbox->setColumnMode(QListBox::FitToWidth);
	userbox->setPaletteBackgroundColor(config_file.readColorEntry("Look", "UserboxBgColor"));
	userbox->setPaletteForegroundColor(config_file.readColorEntry("Look", "UserboxFgColor"));
	userbox->QListBox::setFont(config_file.readFontEntry("Look", "UserboxFont"));
	userbox->setMinimumWidth(20);

	hbox1->setStretchFactor(group_bar, 1);
	hbox1->setStretchFactor(userbox, 100);

	/* add all users to userbox */
	setActiveGroup("");

	connect(userbox, SIGNAL(doubleClicked(QListBoxItem *)), this, SLOT(sendMessage(QListBoxItem *)));
	connect(userbox, SIGNAL(returnPressed(QListBoxItem *)), this, SLOT(sendMessage(QListBoxItem *)));
	connect(userbox, SIGNAL(rightButtonClicked(QListBoxItem *, const QPoint &)),
		this, SLOT(listPopupMenu(QListBoxItem *)));
	connect(userbox, SIGNAL(mouseButtonClicked(int, QListBoxItem *, const QPoint &)),
		this, SLOT(mouseButtonClicked(int, QListBoxItem *)));
	connect(userbox, SIGNAL(currentChanged(QListBoxItem *)), this, SLOT(currentChanged(QListBoxItem *)));

	statuslabeltxt = new MyLabel(centralFrame, "statuslabeltxt");
	statuslabeltxt->setText(tr("Offline"));
	statuslabeltxt->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
	statuslabeltxt->setFont(QFont("Verdana", 9));
	statuslabeltxt->setFixedWidth((width() - 45) > 50 ? width() - 45 : 50);

	/* a bit darker than the rest */
	statuslabeltxt->setPaletteBackgroundColor(QColor(
		qRed(statuslabeltxt->paletteBackgroundColor().rgb()) - 20,
		qGreen(statuslabeltxt->paletteBackgroundColor().rgb()) - 20,
		qBlue(statuslabeltxt->paletteBackgroundColor().rgb()) - 20));

	statuslabel = new MyLabel(centralFrame, "statuslabel");
	QPixmap *pix = icons->loadIcon("offline");
	statuslabel->setPixmap(*pix);
	statuslabel->setFixedWidth(pix->width());

	/* guess what */
	createMenu();
	createStatusPopupMenu();

	connect(statusppm, SIGNAL(aboutToHide()), this, SLOT(statusMenuAboutToHide()));

	dockppm->insertSeparator();
	dockppm->insertItem(loadIcon("exit.png"), tr("&Exit Kadu"), 9);
	if (config_file.readBoolEntry("General", "UseDocking"))
		trayicon->connectSignals();
//		connect(dockppm, SIGNAL(activated(int)), trayicon, SLOT(dockletChange(int)));

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
	if (!config_file.readBoolEntry("Look", "ShowDesc"))
		descrtb->hide();
	QObject::connect(&userlist, SIGNAL(dnsNameReady(uin_t)), this, SLOT(infopanelUpdate(uin_t)));

	QValueList<int> splitsizes;
	
	splitsizes.append(config_file.readSizeEntry("General", "SplitSize").width());
	splitsizes.append(config_file.readSizeEntry("General", "SplitSize").height());
	split->setSizes(splitsizes);

//	tworzymy pasek narzedziowy
	QToolBar *toolbar = new QToolBar(this, "main toolbar");
	setRightJustification(true);
	setDockEnabled(Qt::DockBottom, false);
	setAppropriate(toolbar, true);
	toolbar->setCloseMode(QDockWindow::Undocked);
	toolbar->setLabel(tr("Main toolbar"));

	QToolButton *inactivebtn = new QToolButton(*icons->loadIcon("offline"), tr("Show / hide inactive users"),
	QString::null, this, SLOT(showHideInactive()), toolbar, "ShowHideInactive");

	
	QIconSet *mu;
	if (mute)
	{mu= new QIconSet(loadIcon("mute.png"));}
	else
	{mu= new QIconSet(loadIcon("unmute.png"));}
	mutebtn = new QToolButton((*mu), tr("Mute sounds"),
	QString::null, this, SLOT(muteUnmuteSounds()), toolbar, "mute");

	QToolButton *configbtn = new QToolButton(loadIcon("configure.png"), tr("Configuration"),
		QString::null, this, SLOT(configure()), toolbar, "configure");
		
	toolbar->addSeparator();
	QToolButton *viewhistorybtn = new QToolButton(loadIcon("history.png"), tr("View history"),
		QString::null, this, SLOT(viewHistory()), toolbar, "viewhistory");
	QToolButton *userinfobtn = new QToolButton(loadIcon("identity.png"), tr("View/edit user info"),
		QString::null, this, SLOT(showUserInfo()), toolbar, "userinfo");
	QToolButton *lookupbtn = new QToolButton(loadIcon("viewmag.png"),tr("Lookup in directory"),
		QString::null, this, SLOT(lookupInDirectory()), toolbar, "lookup");
	toolbar->addSeparator();
	QToolButton *adduserbtn = new QToolButton(*icons->loadIcon("online"), tr("Add user"),
		QString::null, this, SLOT(addUserAction()), toolbar, "adduser");
	QFrame *toolbarfiller = new QFrame(toolbar);
	toolbar->setStretchableWidget(toolbarfiller);
	toolbar->setVerticallyStretchable(true);

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

//	tworzymy gridlayout
	grid = new QGridLayout(centralFrame, 2, 6);
	grid->addMultiCellWidget(split, 0, 0, 0, 5);
	grid->addWidget(statuslabeltxt, 1, 0, Qt::AlignLeft);
	grid->addWidget(statuslabel, 1, 5, Qt::AlignRight);
	grid->setColStretch(0, 5);
	grid->setColStretch(1, 1);
	grid->setColStretch(2, 1);
	grid->setColStretch(3, 1);
	grid->setColStretch(4, 1);
	grid->setColStretch(5, 1);
	grid->setRowStretch(0, 40);
	grid->setRowStretch(1, 1);
	grid->activate();

	centralFrame->setMinimumSize(50, 100);

	refreshGroupTabBar();

	dccsock = NULL;
	/* dirty workaround for multiple showEvents */
	commencing_startup = true;

	/* pokaz okno jesli RunDocked jest wylaczone lub dock wylaczone */
	if ((!config_file.readBoolEntry("General", "RunDocked")) || (!config_file.readBoolEntry("General", "UseDocking")))
		show();

	autostatus_timer = new AutoStatusTimer(this);
	if (config_file.readBoolEntry("General", "AddToDescription"))
		autostatus_timer->start(1000, true);

	if (config_file.readNumEntry("General", "UIN")) {
		uc = new UpdatesClass(config_file.readNumEntry("General", "UIN"));
		if (config_file.readBoolEntry("General", "CheckUpdates"))
			QObject::connect(uc->op, SIGNAL(data(const QByteArray &, QNetworkOperation *)),
				this, SLOT(gotUpdatesInfo(const QByteArray &, QNetworkOperation *)));
		uc->run();
		}
}

void Kadu::showHideInactive() 
{
	userbox->showHideInactive();
}

void Kadu::muteUnmuteSounds()
{
	mute = !mute;
	if (mute) {
		mutebtn->setIconSet(loadIcon("mute.png"));
		mutebtn->setTextLabel(tr("Unmute sounds"));
		mmb->changeItem(muteitem, loadIcon("mute.png"), tr("Unmute sounds"));
		}
	else {
		mmb->changeItem(muteitem, loadIcon("unmute.png"), tr("Mute sounds"));
		mutebtn->setTextLabel(tr("Mute sounds"));
		mutebtn->setIconSet(loadIcon("unmute.png"));
		}
}

void Kadu::configure() 
{
	ConfigDialog::showConfigDialog(a);
}

void Kadu::sendSms()
{
	Sms *sms = new Sms("", 0);
	sms->show();
}

void Kadu::sendSmsToUser()
{
	if ((userbox->isSelected(userbox->currentItem())) &&
	(userlist.byAltNick(kadu->userbox->currentText()).mobile.length()!=0)) {
		Sms *sms = new Sms(userbox->currentText(), 0);
		sms->show();
		}	
}


void Kadu::viewHistory() {
	UinsList uins;
	for (int i = 0; i < userbox->count(); i++)
		if (userbox->isSelected(i))
			uins.append(userlist.byAltNick(userbox->text(i)).uin);
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
			if (userbox->currentText() == "")
				return;
			UserListElement user = userlist.byAltNick(userbox->currentText());
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

void Kadu::makeVoiceChat()
{
	if (config_file.readBoolEntry("Network", "AllowDCC"))
		if (config_dccip.isIp4Addr()) {
			struct gg_dcc *dcc_new;
			if (userbox->currentText() == "")
				return;
			UserListElement user = userlist.byAltNick(userbox->currentText());
			if (user.port >= 10) {
				if ((dcc_new = gg_dcc_voice_chat(htonl(user.ip.ip4Addr()), user.port,
					config_file.readNumEntry("General", "UIN"), user.uin)) != NULL) {
					dccSocketClass *dcc = new dccSocketClass(dcc_new, DCC_TYPE_VOICE);
					connect(dcc, SIGNAL(dccFinished(dccSocketClass *)), this,
						SLOT(dccFinished(dccSocketClass *)));
					dcc->initializeNotifiers();
					}
				}
//			else
//				gg_dcc_request(sess, user.uin);
			}
}

void Kadu::lookupInDirectory() {
	if (userbox->currentItem() != -1) {
		SearchDialog *sd = new SearchDialog(0, tr("User info"),
			userlist.byAltNick(userbox->currentText()).uin);
		sd->show();
		sd->firstSearch();
		}
}

void Kadu::showUserInfo() {
	if (userbox->currentItem() != -1) 
		{
		UserInfo *ui = new UserInfo("user info", 0, userbox->currentText());
		ui->show();
		}
}

void Kadu::deleteUsers() 
{
	QStringList users;
	for (int i = 0; i < userbox->count(); i++)
		if (userbox->isSelected(i))
			users.append(userbox->text(i));
	removeUser(users, false);
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

void Kadu::sendKey()
{
#ifdef HAVE_OPENSSL
	UserListElement user = userlist.byAltNick(userbox->currentText());
	QString keyfile_path;
	QString mykey;
	QFile keyfile;

	keyfile_path.append(ggPath("keys/"));
	keyfile_path.append(config_file.readEntry("General", "UIN"));
	keyfile_path.append(".pem");

	keyfile.setName(keyfile_path);

	if (keyfile.open(IO_ReadOnly)) {
		QTextStream t(&keyfile);
		mykey = t.read();
		keyfile.close();
		QCString tmp(mykey.local8Bit());
		gg_send_message(sess, GG_CLASS_MSG, user.uin, (unsigned char *)tmp.data());
		QMessageBox::information(this, "Kadu",
			tr("Your public key has been sent"), tr("OK"), QString::null, 0);
		}
#endif
}

void Kadu::deleteHistory()
{
	UinsList uins;
	for (int i = 0; i < userbox->count(); i++)
		if (userbox->isSelected(i))
			uins.append(userlist.byAltNick(userbox->text(i)).uin);
	history.removeHistory(uins);
}

void Kadu::manageIgnored()
{
	Ignored *ign = new Ignored(0, "ignored");
	ign->show();
}

void Kadu::openChat()
{
	PendingMsgs::Element elem;
	int l,k;
	bool stop = false;
	UinsList uins = getSelectedUins();
	QString toadd;
	for (int i = 0; i < pending.count(); i++) {
		elem = pending[i];
		if (elem.uins.equals(uins))
			if ((elem.msgclass & GG_CLASS_CHAT) == GG_CLASS_CHAT
				|| (elem.msgclass & GG_CLASS_MSG) == GG_CLASS_MSG
				|| !elem.msgclass) {
				l = chats.count();
				k = openChat(elem.uins);
				QValueList<UinsList>::iterator it = wasFirstMsgs.begin();
				while (it != wasFirstMsgs.end() && !elem.uins.equals(*it))
					it++;
				if (it != wasFirstMsgs.end())
					wasFirstMsgs.remove(*it);
				if (l < chats.count())
					chats[k].ptr->writeMessagesFromHistory(elem.uins, elem.time);
				chats[k].ptr->formatMessage(false,
					userlist.byUin(elem.uins[0]).altnick, elem.msg,
					timestamp(elem.time), toadd);
				pending.deleteMsg(i);
				i--;
				stop = true;
				}
		}
	if (!stop) {
		k = openChat(uins);
		chats[k].ptr->writeMessagesFromHistory(uins, 0);
		}
}

void Kadu::searchInDirectory()
{
	SearchDialog *sd = new SearchDialog();
	sd->show();
}

void Kadu::help()
{
	QString link;
	if (QFile::exists(QString(DOCDIR) + "/index_doc.html"))
		link = QString(DOCDIR) + "/index_doc.html";
	else
		link = "http://kadu.net/index_doc.html";
	openWebBrowser(link);
}

void Kadu::about()
{
	About *about = new About;
	about->show();
}

void Kadu::remindPassword1()
{
	remindPassword *rp = new remindPassword();
	rp->start();
}

void Kadu::changePassword1()
{
	changePassword *cp = new changePassword();
}

void Kadu::registerUser()
{
	Register *reg = new Register;
}

void Kadu::unregisterUser()
{
	Unregister *ureg = new Unregister;
}

void Kadu::quit()
{
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
	if (trayicon)
		close();
}

void Kadu::sendUserlist1()
{
	sendUserlist();
}

void Kadu::ignoreUser()
{
	UinsList uins = getSelectedUins();
	if (isIgnored(uins))
		delIgnored(uins);
	else
		addIgnored(uins);
	writeIgnored();
}

void Kadu::blockUser()
{
	UserListElement *puser = &userlist.byAltNick(userbox->currentText());
	puser->blocking = !puser->blocking;
	gg_remove_notify_ex(sess, puser->uin, puser->blocking ? GG_USER_NORMAL : GG_USER_BLOCKED);
	gg_add_notify_ex(sess, puser->uin, puser->blocking ? GG_USER_BLOCKED : GG_USER_NORMAL);
	userlist.writeToFile();
}

void Kadu::notifyUser()
{
	UserListElement *puser = &userlist.byAltNick(userbox->currentText());
	puser->notify = !puser->notify;
	userlist.writeToFile();
}

void Kadu::offlineToUser()
{
	UserListElement *puser = &userlist.byAltNick(userbox->currentText());
	puser->offline_to_user = !puser->offline_to_user;
	gg_remove_notify_ex(sess, puser->uin, puser->offline_to_user ? GG_USER_NORMAL : GG_USER_OFFLINE);
	gg_add_notify_ex(sess, puser->uin, puser->offline_to_user ? GG_USER_OFFLINE : GG_USER_NORMAL);
	userlist.writeToFile();
}

void Kadu::resizeEvent(QResizeEvent *e) {
	statuslabeltxt->setFixedWidth((width() - 45) > 50 ? width() - 45 : 50);
}

void Kadu::changeAppearance() {
	kdebug("kadu::changeAppearance()\n");

	userbox->setPaletteBackgroundColor(config_file.readColorEntry("Look", "UserboxBgColor"));
	userbox->setPaletteForegroundColor(config_file.readColorEntry("Look", "UserboxFgColor"));
	userbox->QListBox::setFont(config_file.readFontEntry("Look", "UserboxFont"));

	group_bar->setFont(QFont(config_file.readFontEntry("Look", "UserboxFont").family(), config_file.readFontEntry("Look", "UserboxFont").pointSize(),75));

	descrtb->setPaletteBackgroundColor(config_file.readColorEntry("Look", "UserboxDescBgColor"));
	descrtb->setPaletteForegroundColor(config_file.readColorEntry("Look", "UserboxDescTextColor"));
	descrtb->setFont(config_file.readFontEntry("Look", "UserboxDescFont"));
}

void Kadu::currentChanged(QListBoxItem *item) {
	if (!item || !item->isSelected())
		return;

	kdebug("Kadu::currentChanged(): %s\n", (const char *)item->text().local8Bit());

	if (config_file.readBoolEntry("Look", "ShowDesc"))
		descrtb->setText(parse(config_file.readEntry("Look", "PanelContents"),userlist.byAltNick(item->text())));
}

void Kadu::refreshGroupTabBar()
{
	if (!config_file.readBoolEntry("Look", "DisplayGroupTabs"))
	{
		group_bar->hide();
		return;
	};	
	/* budujemy listê grup */
	QValueList<QString> group_list;
	for (int i = 0; i < userlist.count(); i++)
	{
		QString groups = userlist[i].group();
		QString group;
		for (int g = 0; (group = groups.section(',' ,g ,g)) != ""; g++)
			if(!group_list.contains(group))
				group_list.append(group);
	};
	kdebug("%i groups found\n",group_list.count());
	//
	if (group_list.count() == 0)
	{
		group_bar->hide();
		setActiveGroup("");
		return;
	};
	/* usuwamy wszystkie niepotrzebne zakladki - od tylu,
	   bo indeksy sie przesuwaja po usunieciu */
	for (int i = group_bar->count() - 1; i >= 1; i--)
		if(!group_list.contains(group_bar->tabAt(i)->text()))
			group_bar->removeTab(group_bar->tabAt(i));
	/* dodajemy nowe zakladki */
	for (int i = 0; i < group_list.count(); i++)
		{
		bool createNewTab = true;
		for (int j = 0; j < group_bar->count(); j++)
			if (group_bar->tabAt(j)->text() == group_list[i])
				createNewTab = false;
		if(createNewTab)
			group_bar->addTab(new QTab(group_list[i]));
		}
	kdebug("%i group tabs\n", group_bar->count());
	group_bar->show();
	/* odswiezamy - dziala tylko jesli jest widoczny */
	group_bar->update();
};

void Kadu::setActiveGroup(const QString& group)
{
	userbox->clearUsers();
	for (int i = 0; i < userlist.count(); i++)
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
		if (belongsToGroup && (!userlist[i].anonymous || !trayicon))
			userbox->addUser(userlist[i].altnick);
		}
	UserBox::all_refresh();
};

void Kadu::groupTabSelected(int id)
{
	if (id == 0)
		setActiveGroup("");
	else
		setActiveGroup(group_bar->tab(id)->text());
};

void Kadu::userListModified()
{
	refreshGroupTabBar();
};

void Kadu::userListStatusModified(UserListElement *user)
{
	kdebug("Kadu::userListStatusModified(): %d\n", user->uin);

//	int index = userbox->currentItem();
//	if (index >= 0) {
//		QListBoxItem *lbi = userbox->item(index);
//		UserListElement &u = userlist.byAltNick(lbi->text());
//		if (u.uin == user->uin)
//			currentChanged(lbi);
//		}
	for (int i = 0; i < chats.count(); i++)
		if (chats[i].uins.contains(user->uin))
			chats[i].ptr->setTitle();
};

void Kadu::removeUser(QStringList &users, bool permanently = false)
{
	if(QMessageBox::warning(kadu, "Kadu",
		tr("Selected users will be deleted. Are you sure?"),
		tr("&Yes"),tr("&No"))!=0)
		return;

	int i;

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
	QPixmap *pix;

	if (!doBlink && socket_active) {
		setCurrentStatus(loginparams.status & (~GG_STATUS_FRIENDS_MASK));
		return;
		}
	else
		if (!doBlink && !socket_active) {
			pix = icons->loadIcon("offline");
	    		statuslabel->setPixmap(*pix);
	    		if (trayicon)
				trayicon->setType(*pix);
	    		return;
	    		}

	if (blinkOn) {
		pix = icons->loadIcon("offline");
		statuslabel->setPixmap(*pix);
		if (trayicon)
			trayicon->setType(*pix);
		blinkOn = false;
		}
	else {
		i = statusGGToStatusNr(loginparams.status & (~GG_STATUS_FRIENDS_MASK));
		pix = icons->loadIcon(gg_icons[i]);
		statuslabel->setPixmap(*pix);
		if (trayicon)
			trayicon->setType(*pix);
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

	dccsock = gg_dcc_socket_create(config_file.readNumEntry("General", "UIN"), 0);

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

// code for addUser has been moved from adduser.cpp
// for sharing with search.cpp
void Kadu::addUser(UserListElement &ule)
{
	UserListElement e;
	e.first_name = ule.first_name;
	e.last_name = ule.last_name;
	e.nickname = ule.nickname;
	e.altnick = ule.altnick;
	e.mobile = ule.mobile;
	e.uin = ule.uin;
	e.setGroup(ule.group());
	e.email = ule.email;
	if (!userlist.containsUin(ule.uin) || (!ule.uin && !userlist.containsAltNick(ule.altnick))) {
		e.status = ule.status;
		e.image_size = ule.image_size;
		e.description = ule.description;
		e.anonymous = ule.anonymous;
		userlist.addUser(e);
		}
	else {
		UserListElement &oldule = userlist.byUin(ule.uin);
		kdebug("Kadu::addUser(): uin = %d\n", ule.uin);
		if (!ule.uin)
			oldule = userlist.byAltNick(ule.altnick);
		e.status = oldule.status;
		e.image_size = oldule.image_size;
		e.blocking = oldule.blocking;
		e.offline_to_user = oldule.offline_to_user;
		e.notify = oldule.notify;
		userlist.changeUserInfo(oldule.altnick, e);
		}
	userlist.writeToFile();

	userbox->addUser(ule.altnick);
	UserBox::all_refresh();

	refreshGroupTabBar();

	if (!ule.anonymous && ule.uin)
		gg_add_notify(sess, ule.uin);
};

int Kadu::openChat(UinsList senders) {
	int i;
	UinsList uins;

	i = 0;
	while (i < chats.count() && !chats[i].uins.equals(senders))
		i++;

	if (i == chats.count()) {
		Chat *chat;
		uins = senders;
		chat = new Chat(uins, 0);
		chat->setTitle();
		chat->show();
		}
	else {
		chats[i].ptr->raise();
		chats[i].ptr->setActiveWindow();
		return i;
		}

	i = 0;
	while (i < chats.count() && !chats[i].uins.equals(senders))
		i++;

	kdebug("Kadu::openChat(): return %d\n", i);

	return i;
}

UinsList Kadu::getSelectedUins() {
	UinsList uins;
	UserListElement user;
	for (int i = 0; i < userbox->count(); i++)
		if (userbox->isSelected(i)) {
			user = userlist.byAltNick(userbox->text(i));
			if (user.uin)
				uins.append(user.uin);
			}
	return uins;
}

/* changes the active group */
void Kadu::changeGroup(int group) {
	activegrpno = group;
	grpmenu->setItemChecked(true, group - 600);
	grpmenu->updateItem(group - 600);
	grpmenu->repaint();
	kdebug("Kadu::changeGroup(): group = %d\n", group - 600);
}

void Kadu::mouseButtonClicked(int button, QListBoxItem *item) {
	kdebug("Kadu::mouseButtonClicked(): button=%d\n", button);
	if (button !=4 || !item)
		return;
	UserListElement user;
	user = userlist.byAltNick(item->text());
	if (user.mobile.length())	
		sendSmsToUser();
}

/* the list that pops up if we right-click one someone */
void Kadu::listPopupMenu(QListBoxItem *item) {
	if (item == NULL)
		return;

	QPopupMenu * pm;
	pm = new QPopupMenu(this);

	QPixmap msg;
	msg = loadIcon("mail_generic.png");
	int smsitem;
	int sendfile;
	int voicechat;
	int deletehistoryitem;
	int historyitem;
	int searchuser;
	int openchatitem;
	int ignoreuseritem;
	int blockuseritem;
	int notifyuseritem;
	int offlinetouseritem;
	UserListElement user;
	user = userlist.byAltNick(item->text());

	openchatitem= pm->insertItem(tr("Open chat window") ,this, SLOT(openChat()));
	smsitem = pm->insertItem(tr("Send SMS"), this, SLOT(sendSmsToUser()),
		HotKey::shortCutFromFile("kadu_sendsms"));
	if (!user.mobile.length())
		pm->setItemEnabled(smsitem, false);

	sendfile = pm->insertItem(loadIcon("filesave.png"), tr("Send file"), this,
		SLOT(sendFile()), HotKey::shortCutFromFile("kadu_sendfile"));
	voicechat =  pm->insertItem(tr("Voice chat"), this,
		SLOT(makeVoiceChat()), HotKey::shortCutFromFile("kadu_voicechat"));
	if (dccSocketClass::count >= 8) {
		pm->setItemEnabled(sendfile, false);
		pm->setItemEnabled(voicechat, false);
		}
	if ((config_file.readBoolEntry("Network", "AllowDCC")) &&
		(user.status == GG_STATUS_AVAIL || user.status == GG_STATUS_AVAIL_DESCR ||
		user.status == GG_STATUS_BUSY || user.status == GG_STATUS_BUSY_DESCR)) {
			pm->setItemEnabled(sendfile, true);
			pm->setItemEnabled(voicechat, true);
			}
		else {
			pm->setItemEnabled(sendfile, false);
			pm->setItemEnabled(voicechat, false);
			}

#ifdef HAVE_OPENSSL
	int sendkeyitem;
	sendkeyitem= pm->insertItem(loadIcon("encrypted.png"), tr("Send my public key"), this, SLOT(sendKey()));

	QString keyfile_path;

	keyfile_path.append(ggPath("keys/"));
	keyfile_path.append(QString::number(config_file.readNumEntry("General", "UIN")));
	keyfile_path.append(".pem");

	QFileInfo keyfile(keyfile_path);
	if (keyfile.permission(QFileInfo::ReadUser) && user.uin)
		pm->setItemEnabled(sendkeyitem, true);
	else
		pm->setItemEnabled(sendkeyitem, false);

#endif

	pm->insertSeparator();
	ignoreuseritem= pm->insertItem(tr("Ignore user"), this, SLOT(ignoreUser()));
	blockuseritem= pm->insertItem(tr("Block user"), this, SLOT(blockUser()));
	notifyuseritem= pm->insertItem(tr("Notify about user"), this, SLOT(notifyUser()));
	offlinetouseritem= pm->insertItem(tr("Offline to user"), this, SLOT(offlineToUser()));
	if (!user.uin) {
		pm->setItemEnabled(ignoreuseritem, false);
		pm->setItemEnabled(blockuseritem, false);
		pm->setItemEnabled(notifyuseritem, false);
		pm->setItemEnabled(offlinetouseritem, false);
		}
	else {
		UinsList uins;
		uins = getSelectedUins();
		if (isIgnored(uins))
			pm->setItemChecked(ignoreuseritem, true);
		if (user.blocking)
			pm->setItemChecked(blockuseritem, true);
		pm->setItemEnabled(offlinetouseritem, config_file.readBoolEntry("General", "PrivateStatus"));
		if (user.offline_to_user)
			pm->setItemChecked(offlinetouseritem, true);
		pm->setItemEnabled(notifyuseritem, config_file.readBoolEntry("Notify", "NotifyStatusChange") && !config_file.readBoolEntry("Notify", "NotifyAboutAll"));
		if (user.notify)
			pm->setItemChecked(notifyuseritem, true);
		}

	pm->insertSeparator();
	pm->insertItem(loadIcon("remove.png"), tr("Remove from userlist"), this, SLOT(deleteUsers()),HotKey::shortCutFromFile("kadu_deleteuser"));
	deletehistoryitem= pm->insertItem(loadIcon("eraser.png"), tr("Clear history"), this, SLOT(deleteHistory()));
	QPixmap history;
	history = loadIcon("history.png");
	historyitem= pm->insertItem(history, tr("View history"),this,SLOT(viewHistory()),HotKey::shortCutFromFile("kadu_viewhistory"));
	pm->insertItem(loadIcon("identity.png"), tr("View/edit user info"), this, SLOT(showUserInfo()),HotKey::shortCutFromFile("kadu_persinfo"));
	searchuser= pm->insertItem(loadIcon("viewmag.png"), tr("Lookup in directory"), this, SLOT(lookupInDirectory()),HotKey::shortCutFromFile("kadu_searchuser"));
	if (!user.uin) {
		pm->setItemEnabled(deletehistoryitem, false);
		pm->setItemEnabled(historyitem, false);
		pm->setItemEnabled(searchuser, false);
		pm->setItemEnabled(openchatitem, false);
		}
	pm->insertSeparator();
	pm->insertItem(tr("About..."), this, SLOT(about()));

	pm->exec(QCursor::pos());    	
}

/* if something's pending, open it, if not, open new message */
void Kadu::sendMessage(QListBoxItem *item) {
	QString tmp;
	int i, j, k = -1, l;
	bool stop = false;
/*	rMessage *rmsg;
	Message *msg; */
	UinsList uins;
	PendingMsgs::Element elem;
	UserListElement e;
	bool ok;
	uin_t uin;
	QString toadd;
	bool msgsFromHist = false;

	uin = userlist.byAltNick(item->text()).uin;

	if (!uin) {
		sendSmsToUser();
		return;
		}

	for (i = 0; i < pending.count(); i++) {
		elem = pending[i];
		if ((!uins.count() && elem.uins.contains(uin)) || (uins.count() && elem.uins.equals(uins)))
			if ((elem.msgclass & GG_CLASS_CHAT) == GG_CLASS_CHAT
				|| (elem.msgclass & GG_CLASS_MSG) == GG_CLASS_MSG
				|| !elem.msgclass) {
				if (!uins.count())
					uins = elem.uins;
				for (j = 0; j < elem.uins.count(); j++)
					if (!userlist.containsUin(elem.uins[j])) {
						tmp = QString::number(pending[i].uins[j]);
						e.first_name = "";
						e.last_name = "";
						e.nickname = tmp;
						e.altnick = tmp;
						e.uin = tmp.toUInt(&ok);
						if (!ok)
							e.uin = 0;
						e.mobile = "";
						e.setGroup("");
						e.description = "";
						e.email = "";
						e.anonymous = true;
						if (trayicon)
							userlist.addUser(e);
						else
							addUser(e);
						}
				
				l = chats.count();
				k = openChat(elem.uins);
				QValueList<UinsList>::iterator it = wasFirstMsgs.begin();
				while (it != wasFirstMsgs.end() && !elem.uins.equals(*it))
					it++;
				if (it != wasFirstMsgs.end())
					wasFirstMsgs.remove(*it);
				if (!msgsFromHist) {
					if (l < chats.count())
						chats[k].ptr->writeMessagesFromHistory(elem.uins, elem.time);
					msgsFromHist = true;
					}
				chats[k].ptr->formatMessage(false,
					userlist.byUin(elem.uins[0]).altnick, elem.msg,
					timestamp(elem.time), toadd);	    
				pending.deleteMsg(i);
				kdebug("Kadu::sendMessage(): k=%d\n", k);
				i--;
				stop = true;
				}
	    	/*	else {
				if (!stop) {
  		    			rmsg = new rMessage(item->text(),
						elem.msgclass, elem.uins, elem.msg);
					deletePendingMessage(i);
					UserBox::all_refresh();
					rmsg->init();
					rmsg->show();
					}
				else
					chats[k].ptr->scrollMessages(toadd);
				return;
				}*/
		}

	if (stop) {
		chats[k].ptr->scrollMessages(toadd);
		UserBox::all_refresh();
		return;
		}

	uins.clear();
	for (i = 0; i < userbox->count(); i++)
		if (userbox->isSelected(i))
			uins.append(userlist.byAltNick(userbox->text(i)).uin);
	if (!uins.count())
		uins.append(userlist.byAltNick(item->text()).uin);

/*	if (uins.count() > 1 || (userlist.byUin(uins[0]).status != GG_STATUS_NOT_AVAIL
		&& userlist.byUin(uins[0]).status != GG_STATUS_NOT_AVAIL_DESCR))
		openChat(uins);
	else {
		msg = new Message(item->text());
		msg->init();
		msg->show();
		}*/
//	zawsze otwieraja sie czaty
	l = chats.count();
	k = openChat(uins);
	if (!msgsFromHist && l < chats.count())
		chats[k].ptr->writeMessagesFromHistory(uins, 0);
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
			statusppm->setItemChecked(8, !statusppm->isItemChecked(8));
			dockppm->setItemChecked(8, !dockppm->isItemChecked(8));	    
			config_file.writeEntry("General", "PrivateStatus",statusppm->isItemChecked(8));
			if (!statusppm->isItemChecked(6) && !statusppm->isItemChecked(7))
				setStatus(sess->status & (~GG_STATUS_FRIENDS_MASK));
			break;
		}
}

void Kadu::slotShowStatusMenu() {
	QPoint point = statuslabeltxt->mapToGlobal(QPoint(0, 0));
	point.setY(point.y() - statusppm->sizeHint().height());
	statusppm->popup(point);
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

	statuslabeltxt->setText(qApp->translate("@default", statustext[statusnr]));
	statusppm->setItemEnabled(7, statusnr != 6);
	dockppm->setItemEnabled(7, statusnr != 6);
	QPixmap *pix = icons->loadIcon(gg_icons[statusnr]);
	statuslabel->setPixmap(*pix);
	setIcon(*pix);
	if (!pending.pendingMsgs() && trayicon)
		trayicon->setType(*pix);
}

void Kadu::setStatus(int status) {
	QHostAddress ip;

	kdebug("Kadu::setStatus(): setting status: %d\n",
		status | (GG_STATUS_FRIENDS_MASK * config_file.readBoolEntry("General", "PrivateStatus")));

	bool with_description;
    
	with_description = ifStatusWithDescription(status);
	status &= ~GG_STATUS_FRIENDS_MASK;
    	
	i_wanna_be_invisible = false;
	if (status == GG_STATUS_INVISIBLE || status == GG_STATUS_INVISIBLE_DESCR)
		i_wanna_be_invisible = true;
	disconnect_planned = false;		

	if (!userlist_sent) {
		doBlink = true;
		if (!blinktimer) {
			blinktimer = new QTimer;
			QObject::connect(blinktimer, SIGNAL(timeout()), kadu, SLOT(blink()));
			}
		blinktimer->start(1000, TRUE);
		}
    
	if (socket_active) {
		doBlink = false;
		if (with_description) {
			unsigned char *descr;
			descr = (unsigned char *)strdup(unicode2cp(own_description).data());
			if (status == GG_STATUS_NOT_AVAIL_DESCR)
				gg_change_status_descr(sess, status, (const char *)descr);
			else
				gg_change_status_descr(sess,
					status | (GG_STATUS_FRIENDS_MASK * config_file.readBoolEntry("General", "PrivateStatus")), (const char *)descr);
			free(descr);
			}
		else
			gg_change_status(sess, status | (GG_STATUS_FRIENDS_MASK * config_file.readBoolEntry("General", "PrivateStatus")));
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
		return;
		}


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
        loginparams.password =
		strdup(unicode2cp(pwHash(config_file.readEntry("General", "Password"))).data());
	char *tmp =
		strdup(unicode2latin(pwHash(config_file.readEntry("General", "Password"))).data());
	kdebug("Kadu::setStatus(): password = %s\n", tmp);
	free(tmp);
	loginparams.uin = config_file.readNumEntry("General", "UIN");
	loginparams.client_version = GG_DEFAULT_CLIENT_VERSION;
	loginparams.has_audio = 1;
	
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
		if (server_nr > 7)
			server_nr = 0;
		}
	loginparams.tls = config_file.readNumEntry("Network", "UseTLS");
	loginparams.protocol_version = 0x21;
	loginparams.client_version = strdup("6, 0, 0, 135");
	if (config_file.readNumEntry("Network", "UseTLS")) {
		loginparams.server_port = 0;
		if (config_file.readBoolEntry("Network", "isDefServers"))
			loginparams.server_addr = 0;
		loginparams.server_port = 443;
		}
	else
		loginparams.server_port = config_file.readNumEntry("Network", "DefaultPort");
	sess = gg_login(&loginparams);
	free(loginparams.client_version);
	free(loginparams.password);

	AutoConnectionTimer::off();

	if (sess) {
		socket_active = true;
		last_ping = time(NULL);

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

/* patrz plik events.cpp
void Kadu::checkConnection(void) {
	// Since it doesnt work anymore...
	readevent->start(10000, TRUE);
	return;	
}
*/

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
	int i;
	doBlink = false;

	kdebug("Kadu::disconnectNetwork(): calling offline routines\n");

	if (config_file.readBoolEntry("General", "AutoAway"))
		AutoAwayTimer::off();
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

	disconnect_planned = true;
	if (sess) {
		if (sess->state == GG_STATE_CONNECTED)
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

	i = 0;
	while (i < chats.count()) {
		chats[i].ptr->setTitle();
		i++;
		}

	UserBox::all_refresh();
	
	socket_active = false;
	
	setCurrentStatus(GG_STATUS_NOT_AVAIL);
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
				QMessageBox::information(0, tr("Information"),
					tr("File has been transferred sucessfully."),
					tr("&OK"));
				break;
			case DCC_SOCKET_TRANSFER_DISCARDED:
				break;
			case DCC_SOCKET_VOICECHAT_DISCARDED:
				break;
			case DCC_SOCKET_TRANSFER_ERROR:
				QMessageBox::information(0, tr("Error"),
					tr("File transfer error!"),
					tr("&OK"));
				break;
			case DCC_SOCKET_CONNECTION_BROKEN:
				break;
			case DCC_SOCKET_COULDNT_OPEN_FILE:
				QMessageBox::information(0, tr("Error"),
					tr("Couldn't open file!"),
					tr("&OK"));
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
	if (!quit && trayicon) {
		kdebug("Kadu::close(): Kadu hide\n");
		hide();
		return false;
		}
	else {
	
	    if (config_file.readBoolEntry("General", "SaveGeometry"))
	    {
		QSize split;
		    split.setWidth(userbox->size().height());
		    split.setHeight(descrtb->size().height());
		
		config_file.writeEntry("General", "SplitSize",split);
		
		QRect geom;
		    geom.setX(pos().x());
		    geom.setY(pos().y());
		    geom.setWidth(size().width());
		    geom.setHeight(size().height());
		
		config_file.writeEntry("General", "Geometry",geom);
	    }
		config_file.writeEntry("General", "DefaultDescription", defaultdescriptions.join("<-->"));
		
		QString dockwindows=config_file.readEntry("General", "DockWindows");
		QTextStream stream(&dockwindows, IO_WriteOnly);
		stream << *kadu;
		dockwindows.replace(QRegExp("\\n"), "\\n");
		
		config_file.writeEntry("General", "DockWindows", dockwindows);
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
//		a->quit();
		kdebug("Kadu::close(): Graceful shutdown...\n");
		return true;
	}
}

void Kadu::quitApplication() {
	kdebug("Kadu::quitApplication()\n");
	close(true);
}

Kadu::~Kadu(void) {
	kdebug("Kadu::~Kadu()\n");
}

void Kadu::createMenu() {

	mmb = new QMenuBar(this, "mmb");

	QPopupMenu *ppm = new QPopupMenu(this, "ppm");
	ppm->insertItem(tr("Manage &ignored"), this, SLOT(manageIgnored()));
	ppm->insertItem(loadIcon("configure.png"), tr("&Configuration"), this, SLOT(configure()),HotKey::shortCutFromFile("kadu_configure"));
	ppm->insertItem(loadIcon("reload.png"), tr("Resend &userlist"), this, SLOT(sendUserlist1()));
	if (mute) {
		muteitem= ppm->insertItem(loadIcon("mute.png"), tr("Unmute sounds"), this, SLOT(muteUnmuteSounds()));
		}
	else {
		muteitem= ppm->insertItem(loadIcon("unmute.png"), tr("Mute sounds"), this, SLOT(muteUnmuteSounds()));
		}
	ppm->insertSeparator();

	grpmenu = new QPopupMenu(this);
	grpmenu->insertItem(tr("All"), 600);
	grpmenu->insertSeparator();

	ppm->insertItem(tr("Remind &password"), this, SLOT(remindPassword1()));
	ppm->insertItem(tr("&Change password"), this, SLOT(changePassword1()));
	ppm->insertItem(loadIcon("newuser.png"),tr("Register &new user"), this, SLOT(registerUser()));
	ppm->insertItem(tr("Unregister user"), this, SLOT(unregisterUser()));
	ppm->insertItem(tr("Personal information"), this,SLOT(personalInfo()));
	ppm->insertSeparator();
	QPixmap find;
	find = loadIcon("viewmag.png");
	ppm->insertItem(find, tr("&Search for users"), this, SLOT(searchInDirectory()));
	ppm->insertItem(tr("I&mport userlist"), this, SLOT(importUserlist()));
	ppm->insertItem(tr("E&xport userlist"), this, SLOT(exportUserlist()));
	ppm->insertItem(*icons->loadIcon("online"), tr("&Add user"), this, SLOT(addUserAction()),HotKey::shortCutFromFile("kadu_adduser"));
	ppm->insertItem(tr("Send SMS"), this,SLOT(sendSms()));
	ppm->insertSeparator();	
	ppm->insertItem(tr("H&elp"), this, SLOT(help()));	
	ppm->insertItem(tr("A&bout..."), this, SLOT(about()));
	ppm->insertSeparator();
	ppm->insertItem(tr("&Hide Kadu"), this, SLOT(hideKadu()));
	ppm->insertItem(loadIcon("exit.png"), tr("&Exit Kadu"), this, SLOT(quit()));

	mmb->insertItem(tr("&Kadu"), ppm);
//	mmb->polish();
}

void Kadu::statusMenuAboutToHide() {
	closestatusppmtime.restart();
}

void Kadu::createStatusPopupMenu() {

	QPixmap *pix;
	QIconSet icon;

	statusppm = new QPopupMenu(this, "statusppm");
	dockppm = new QPopupMenu(this, "dockppm");

	for (int i=0; i<8; i++) {
		pix = icons->loadIcon(gg_icons[i]);
		icon = QIconSet(*pix);
		statusppm->insertItem(icon, qApp->translate("@default", statustext[i]), i);
		dockppm->insertItem(icon, qApp->translate("@default", statustext[i]), i);
		}

	statusppm->insertSeparator();
	dockppm->insertSeparator();
	statusppm->insertItem(tr("Private"), 8);
	statusppm->setItemChecked(8, config_file.readBoolEntry("General", "PrivateStatus"));
	dockppm->insertItem(tr("Private"), 8);
	dockppm->setItemChecked(8, config_file.readBoolEntry("General", "PrivateStatus"));

	statusppm->setCheckable(true);
	dockppm->setCheckable(true);
	statusppm->setItemChecked(6, true);
	dockppm->setItemChecked(6, true);
	
	statusppm->setItemEnabled(7, false);
	dockppm->setItemEnabled(7, false);

	connect(statusppm, SIGNAL(activated(int)), this, SLOT(slotHandleState(int)));
}

void MyLabel::mousePressEvent (QMouseEvent * e) {
	if (e->button() == Qt::LeftButton && closestatusppmtime.elapsed() >= 100)
		kadu->slotShowStatusMenu();
}

void Kadu::showdesc(bool show) {
	if (show)
		descrtb->show();
	else
		descrtb->hide();
}

void Kadu::infopanelUpdate(uin_t uin) {
	if (!config_file.readBoolEntry("Look", "ShowDesc"))
		return;
	kdebug("Kadu::infopanelUpdate(%d)\n", uin);
	if (userbox->currentItem() != -1 && uin == userlist.byAltNick(userbox->currentText()).uin)
		descrtb->setText(parse(config_file.readEntry("Look", "PanelContents"),userlist.byUin(uin)));
}

void KaduSlots::onCreateConfigDialog()
{
	kdebug("KaduSlots::onCreateConfigDialog() \n");
	QLineEdit *e_password=ConfigDialog::getLineEdit("General", "Password");
	e_password->setEchoMode(QLineEdit::Password);
	e_password->setText(pwHash(config_file.readEntry("General", "Password", "")));
	QComboBox *cb_language= ConfigDialog::getComboBox("General", "Set language:");

	QString language;
	language= config_file.readEntry("General", "Language", QTextCodec::locale());
	QDir locale(QString(DATADIR)+"/kadu/translations/", "kadu_*.qm");
	QStringList files=locale.entryList();

	  for ( QStringList::Iterator it = files.begin(); it != files.end(); ++it ) {
	         *it=translateLanguage((*it).mid(5, (*it).length()-8), true);
		      }
	cb_language->insertStringList(files);
	cb_language->setCurrentText(translateLanguage(language,true));
}

void KaduSlots::onDestroyConfigDialog()
{
	kdebug("KaduSlots::onDestroyConfigDialog() \n");
	QLineEdit *e_password=ConfigDialog::getLineEdit("General", "Password");
	e_password->setEchoMode(QLineEdit::Password);
	config_file.writeEntry("General", "Password",pwHash(e_password->text()));

	if (config_file.readBoolEntry("General", "AutoAway"))
	        AutoAwayTimer::on();
	else
                AutoAwayTimer::off();
	    
	if (config_file.readBoolEntry("General", "UseDocking") && !trayicon) {
			trayicon = new TrayIcon(kadu);
			trayicon->show();
			trayicon->connectSignals();
			trayicon->setType(*icons->loadIcon(gg_icons[statusGGToStatusNr(getActualStatus() & (~GG_STATUS_FRIENDS_MASK))]));
			trayicon->changeIcon();
				       }
		    else
			    if (!config_file.readBoolEntry("General", "UseDocking") && trayicon) {
				delete trayicon;
				trayicon = NULL;
							  }

	if (!statusppm->isItemChecked(6) && !statusppm->isItemChecked(7)
		    && /*prevprivatestatus !=*/ config_file.readBoolEntry("General", "PrivateStatus")) {
			    statusppm->setItemChecked(8, config_file.readBoolEntry("General", "PrivateStatus"));
			    kadu->setStatus(sess->status & (~GG_STATUS_FRIENDS_MASK));
												}

	if (config_file.readBoolEntry("General", "AddToDescription") && !kadu->autostatus_timer->isActive())
		kadu->autostatus_timer->start(1000, true);
	else
		kadu->autostatus_timer->stop();

	kadu->showdesc(config_file.readBoolEntry("Look", "ShowDesc"));
	
	if (config_file.readBoolEntry("Look", "MultiColumnUserbox"))
		kadu->userbox->setColumnMode(QListBox::FitToWidth);
	else
		kadu->userbox->setColumnMode(1);


	/* I odswiez okno Kadu */
	int z;	
	kadu->changeAppearance();
	for (z = 0; z < chats.count(); z++)
		chats[z].ptr->changeAppearance();
	kadu->refreshGroupTabBar();

	kadu->setCaption(tr("Kadu: %1").arg(config_file.readNumEntry("General", "UIN")));


	QComboBox *cb_language= ConfigDialog::getComboBox("General", "Set language:");
	config_file.writeEntry("General", "Language", translateLanguage(cb_language->currentText(),false));

};
