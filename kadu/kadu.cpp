/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <kapp.h>
#include <klocale.h>
#include <qevent.h>
#include <qwidget.h>
#include <qdialog.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qpixmap.h>
#include <qcombobox.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qlineedit.h>
#include <qpopupmenu.h>
#include <qtextstream.h>
#include <qpoint.h>
#include <qlayout.h>
#include <qslider.h>
#include <kiconloader.h>
#include <kconfig.h>
#include <qapplication.h>
#include <qhbuttongroup.h>
#include <kfiledialog.h>
#include <qradiobutton.h>
#include <ksystemtray.h>
#include <qtextview.h>
#include <ktextbrowser.h>
#include <qtooltip.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qfont.h>
#include <qcursor.h>
#include <qmessagebox.h>
#include <qhgroupbox.h>
#include <qprocess.h>
#include <qsocketnotifier.h>
#include <qprogressdialog.h>
#include <qregexp.h>
#include <qlistview.h>
#include <qtabdialog.h>
#include <qstrlist.h>
#include <qgroupbox.h>
#include <kmainwindow.h>
#include <kmenubar.h>
#include <qcheckbox.h>
#include <qtextcodec.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <qtimer.h>
#include <qaccel.h>
#include <qpainter.h>
#include <qmenubar.h>
#include <qnetworkprotocol.h>
#include <qstringlist.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <iostream>
// #include <net/route.h>
#define RTF_GATEWAY 0x2

#include <pthread.h>
#include <ctype.h>
#include <errno.h>

#include <pwd.h>
#include <sys/stat.h>

#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "libgadu.h"

//
#include "misc.h"
#include "kadu.h"
#include "pixmaps.h"
#include "userbox.h"
#include "events.h"
#include "chat.h"
#include "search.h"
#include "dcc.h"
#include "search.h"
#include "expimp.h"
#include "userinfo.h"
#include "personal_info.h"
#include "register.h"
#include "sms.h"
#include "adduser.h"
#include "about.h"
#include "config.h"
#include "ignore.h"
#include "history.h"
#include "message.h"
#include "pending_msgs.h"
#include "dock_widget.h"
#include "updates.h"
#include "password.h"
#ifdef HAVE_OPENSSL
#include "simlite.h"
#endif
//

#define GG_USER_OFFLINE	0x01
#define	GG_USER_NORMAL	0x03
#define GG_USER_BLOCKED	0x04

extern void ackHandler(int);
extern void UinToUserlistEntry (uin_t, int);

bool userlist_sent = FALSE;
bool socket_active = FALSE;
bool disconnect_planned = FALSE;
int userlist_count = 0;
int last_ping;
int last_read_event = -1;
int server_nr = 0;
bool timeout_connected = true;
bool i_wanna_be_invisible = true;
Operation *progresswindow;

struct timeval tv;
struct gg_event *e;
fd_set rd, wd;
int ret;

QTimer * blinktimer;
QTimer * pingtimer;
QTimer * readevent;
QPopupMenu *statusppm;
KPopupMenu *dockppm;
QLabel * statuslabel;
QLabel * statuslabeltxt;
QPopupMenu * grpmenu;

UserList userlist;
PendingMsgs pending;
QValueList<struct chats> chats;
struct gg_session *sess = NULL;
struct sigaction sigact;
struct config config;
QArray<struct acks> acks(0);
struct gg_dcc * dccsock;
QArray<uin_t> ignored;
QArray<groups> grouplist;
struct gg_login_params loginparams;
QSocketNotifier *kadusnr = NULL;
QSocketNotifier *kadusnw = NULL;
QSocketNotifier *dccsnr = NULL;
QSocketNotifier *dccsnw = NULL;
UpdatesClass *uc;

const char **gg_xpm[] = {gg_act_xpm, gg_actdescr_xpm, gg_busy_xpm, gg_busydescr_xpm,
	gg_invi_xpm, gg_invidescr_xpm, gg_inact_xpm, gg_inactdescr_xpm, gg_stop_xpm};
enum {
	KADU_CMD_SEND_MESSAGE,
	KADU_CMD_OPEN_CHAT,
	KADU_CMD_REMOVE_USER,
	KADU_CMD_DELETE_HISTORY,
	KADU_CMD_SHOW_HISTORY,
	KADU_CMD_USERINFO,
	KADU_CMD_SEARCH,
	KADU_CMD_MUTE,
	KADU_CMD_ADD_USER,
	KADU_CMD_MANAGE_IGNORED,
	KADU_CMD_HELP,
	KADU_CMD_ABOUT,
	KADU_CMD_SEND_USERLIST,
	KADU_CMD_SMS,
	KADU_CMD_MAINMENU_SMS,
	KADU_CMD_REMIND_PASSWORD,
	KADU_CMD_CHANGE_PASSWORD,
	KADU_CMD_REGISTER_USER,
	KADU_CMD_UNREGISTER_USER,
	KADU_CMD_QUIT,
	KADU_CMD_SEARCH_USER,
	KADU_CMD_EXPORT_USERLIST,
	KADU_CMD_CONFIG,
	KADU_CMD_IMPORT_USERLIST,
	KADU_CMD_HIDE,
	KADU_CMD_SEND_FILE,
#ifdef HAVE_OPENSSL
	KADU_CMD_PERSONAL_INFO,
	KADU_CMD_SEND_KEY
#else
	KADU_CMD_PERSONAL_INFO
#endif
		     
};

/* our own description container */
QString own_description;

unsigned long getMyIP(void) {
	unsigned long dest, gw;
	int flags, fd;
	FILE *file;
	char buf[256],name[32];
	bool stopped = false;
	struct ifreq ifr;

	file = fopen("/proc/net/route", "r");
	if (!file)
		return 0;

	fgets(buf, 256, file);
	while (!feof(file))
		if (fgets(buf, 256, file)) {
			sscanf(buf, "%s %x %x %x", name, &dest, &gw, &flags);
			if (!dest && gw &&(flags & RTF_GATEWAY)) {
				stopped = true;
				break;
				}	    
	    		}
	fclose(file);

	if (!stopped)
		return 0;

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		return 0;

	strcpy(ifr.ifr_name, name);
	if (ioctl(fd, SIOCGIFADDR, &ifr) < 0 || ioctl(fd, SIOCGIFFLAGS, &ifr) <0 ) {
		close(fd);
		return 0;
		}

	return ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
}

void deletePendingMessage(int nr) {
	fprintf(stderr, "KK predeletePendingMessage(), counts: %d\n",pending.count());
	pending.deleteMsg(nr);
	fprintf(stderr, "KK deletePendingMessage(%d), counts: %d\n",nr,pending.count());
	if (!pending.pendingMsgs()) {
		fprintf(stderr, "KK pendingMessage is false\n");
		if (dw)
			dw->setType((char **)gg_xpm[statusGGToStatusNr(getActualStatus() & (~GG_STATUS_FRIENDS_MASK))]);
		}
}

void readIgnore(void)
{
	FILE *f;
	const int BUF_SIZE=256;
	char buf[BUF_SIZE];

	if(!(f=fopen(ggPath("ignore"),"r")))
	{
		fprintf(stderr,"readIgnore(): Failed to open ignore file. Ignore list empty. Need to read manual?\n");
		return;
	};

	while(fgets(buf,BUF_SIZE-1,f))
	{
		if(buf[strlen(buf)-1]=='\n')
			buf[strlen(buf)-1]=0;
		if(buf[0]=='#')
			continue;
		addIgnored(atoi(buf));
	};
	
	fclose(f);
};

void confirmHistoryDeletion(UinsList &uins) {
	QString fname;
	QString s;
	int i;

	switch(QMessageBox::information( kadu, "Kadu",i18n("Clear history?"), i18n("Yes"), i18n("No"), QString::null, 1, 1)) {
		case 0: // Yes?
			fname = ggPath("history/");
			uins.sort();
			for (i = 0; i < uins.count(); i++) {
				fname.append(QString::number(uins[i]));
				if (i < uins.count() - 1)
					fname.append("_");
				}
			fprintf(stderr, "KK confirmHistoryDeletion(): deleting %s\n", (const char *)fname.local8Bit());
			unlink((const char *)fname.local8Bit());
			break;
		case 1: // Nope?
			return;
		}
}

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
	fprintf(stderr, "KK send_userlist(): Userlist sent\n");

	free(uins);
}

void Kadu::gotUpdatesInfo(const QByteArray &data, QNetworkOperation *op) {	
	char buf[32];
	int i;
	QString newestversion;
	
	for (i = 0; i < data.size(); i++)
		buf[i] = data[i];
	buf[data.size()] = 0;
	newestversion = buf;

	fprintf(stderr, "KK Kadu::gotUpdatesInfo(): %s\n", buf);
	
	if (uc->ifNewerVersion(newestversion)) {
		QMessageBox::information(this,(i18n("Update information")),
			QString(i18n("The newest Kadu version is %1")).arg(newestversion), QMessageBox::Ok);
		}
	delete uc;
}

/* a monstrous constructor so Kadu would take longer to start up */
Kadu::Kadu(QWidget *parent, const char *name) : QMainWindow(parent, name)
{
	/* timers, cause event loops and QSocketNotifiers suck. */
	pingtimer = blinktimer = readevent = NULL;

	/* initalize our closeEvent workaround */
	close_permitted = false;

	/* blinker */
	blinkOn = false;
	doBlink = false;

	/* another API change, another hack */
	memset(&loginparams, 0, sizeof(loginparams));
	loginparams.async = 1;

	/* active group, 600 is all groups */
	activegrpno = 600;

	int g;

	for (g = 0; g < acks.size(); g++)
		acks[g].seq = 0;

	for (g = 0; g < ignored.size(); g++)
		ignored[g] = 0;
	
	loadKaduConfig();
        
	fprintf(stderr,"KK Setting geometry to %d %d %d %d\n",
			config.geometry.x(), config.geometry.y(),
			config.geometry.width(), config.geometry.height());
	setGeometry(config.geometry);

	if (config.dock) {
		dw = new DockWidget(this);
		dw->show();
		}

	/* use dock hint? */
	if (config.showhint)
		tip = new DockHint(0);
	else
		tip = NULL;
	
	/* read the userlist */
	userlist.readFromFile();

	readIgnore();

	/* DCC */

	QString buf;
	buf.append("Kadu: ");
	buf.append(QString::number(config.uin));


	/* a newbie? */
	if (config.uin)
		setCaption(i18n("Kadu: %1").arg(config.uin));

	pending.loadFromFile();

	/* use dock icon? */
	if (config.dock) {
		dw->changeIcon();
		}

	QFrame *centralFrame = new QFrame(this);
	setCentralWidget(centralFrame);

	/* initialize group tabbar */
	group_bar = new QTabBar(centralFrame, "groupbar");
	group_bar->addTab(new QTab(i18n("All")));
	refreshGroupTabBar();
	connect(group_bar, SIGNAL(selected(int)), this, SLOT(groupTabSelected(int)));

	/* connect userlist modified signal */
	connect(&userlist, SIGNAL(modified()), this, SLOT(userListModified()));
	connect(&userlist, SIGNAL(statusModified(UserListElement *)), this, SLOT(userListStatusModified(UserListElement *)));

	QSplitter *split = new QSplitter(Qt::Vertical, centralFrame);

	/* initialize and configure userbox */
	userbox = new UserBox(split, "userbox");
	userbox->setPaletteBackgroundColor(config.colors.userboxBg);
	userbox->setPaletteForegroundColor(config.colors.userboxFg);
	userbox->QListBox::setFont(config.fonts.userbox);

	/* add all users to userbox */
	setActiveGroup("");

	connect(userbox, SIGNAL(doubleClicked(QListBoxItem *)), this, SLOT(sendMessage(QListBoxItem *)));
	connect(userbox, SIGNAL(returnPressed(QListBoxItem *)), this, SLOT(sendMessage(QListBoxItem *)));
	connect(userbox, SIGNAL(rightButtonClicked(QListBoxItem *, const QPoint &)),
		this, SLOT(listPopupMenu(QListBoxItem *)));
	connect(userbox, SIGNAL(currentChanged(QListBoxItem *)), this, SLOT(currentChanged(QListBoxItem *)));

	statuslabeltxt = new MyLabel(centralFrame, "statuslabeltxt");
	statuslabeltxt->setText(i18n("Offline"));
	statuslabeltxt->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
	statuslabeltxt->setFont(QFont("Verdana", 9));
	statuslabeltxt->setMinimumWidth(width() - 45);

	/* a bit darker than the rest */
	statuslabeltxt->setPaletteBackgroundColor(QColor(
		qRed(statuslabeltxt->paletteBackgroundColor().rgb()) - 20,
		qGreen(statuslabeltxt->paletteBackgroundColor().rgb()) - 20,
		qBlue(statuslabeltxt->paletteBackgroundColor().rgb()) - 20));

	statuslabel = new MyLabel(centralFrame, "statuslabel");
	statuslabel->setPixmap(QPixmap((const char**)gg_inact_xpm) );

	/* guess what */
	createMenu();
	createStatusPopupMenu();

	dockppm->insertSeparator();

	KIconLoader *loader = KGlobal::iconLoader();

	dockppm->insertItem(loader->loadIcon("exit",KIcon::Small), i18n("&Exit Kadu"), 9);
	if (config.dock)
		connect(dockppm, SIGNAL(activated(int)), dw, SLOT(dockletChange(int)));

	descrtb = new QTextBrowser(split, "descrtb");
	descrtb->setFrameStyle(QFrame::NoFrame);
	descrtb->setMinimumHeight(int(1.5 * QFontMetrics(descrtb->font()).height()));
//	descrtb->resize(descrtb->size().width(), int(1.5 * QFontMetrics(descrtb->font()).height()));
	descrtb->setTextFormat(Qt::RichText);
	descrtb->setAlignment(Qt::AlignVCenter | Qt::WordBreak | Qt::DontClip);
	descrtb->setVScrollBarMode(QScrollView::AlwaysOff);
	descrtb->setPaletteBackgroundColor(config.colors.userboxDescBg);
	descrtb->setPaletteForegroundColor(config.colors.userboxDescText);
	descrtb->setFont(config.fonts.userboxDesc);
	if (!config.showdesc)
		descrtb->hide();

	QValueList<int> splitsizes;
	
	splitsizes.append(config.splitsize.width());
	splitsizes.append(config.splitsize.height());
	split->setSizes(splitsizes);

	QGridLayout * grid = new QGridLayout(centralFrame, 3, 3);
	grid->addMultiCellWidget(group_bar, 0, 0, 0, 2);
	grid->addMultiCellWidget(split, 1, 1, 0, 2);
	grid->addWidget(statuslabeltxt, 2, 0, Qt::AlignLeft);
	grid->addWidget(statuslabel, 2, 2, Qt::AlignCenter);
	grid->setColStretch(0, 3);
	grid->setColStretch(1, 3);
	grid->setColStretch(2, 1);
	grid->setRowStretch(0, 1);
	grid->setRowStretch(1, 40);
	grid->setRowStretch(2, 1);
	grid->activate();

	dccsock = NULL;
	/* dirty workaround for multiple showEvents */
	commencing_startup = true;

	/* pokaz okno jesli RunDocked jest wylaczone lub dock wylaczone */
	if ((!config.rundocked) || (!config.dock))
		show();

	autostatus_timer = new AutoStatusTimer(this);
	if (config.addtodescription)
		autostatus_timer->start(1000,TRUE);

	if (config.uin) {
		uc = new UpdatesClass(config.uin);
		if (config.checkupdates)
			QObject::connect(uc->op, SIGNAL(data(const QByteArray &, QNetworkOperation *)),
				this, SLOT(gotUpdatesInfo(const QByteArray &, QNetworkOperation *)));
		uc->run();
		}
}

void Kadu::changeAppearance() {
	fprintf(stderr, "KK kadu::changeAppearance()\n");

	userbox->setPaletteBackgroundColor(config.colors.userboxBg);
	userbox->setPaletteForegroundColor(config.colors.userboxFg);
	userbox->QListBox::setFont(config.fonts.userbox);

	descrtb->setPaletteBackgroundColor(config.colors.userboxDescBg);
	descrtb->setPaletteForegroundColor(config.colors.userboxDescText);
	descrtb->setFont(config.fonts.userboxDesc);
}

void Kadu::currentChanged(QListBoxItem *item) {
	if (!item || !item->isSelected())
		return;
	fprintf(stderr, "KK Kadu::currentChanged(): %s\n", (const char *)item->text().local8Bit());
	UserListElement &ule = userlist.byAltNick(item->text());
	QString s;
	// uin
	if (ule.uin)
		s+=QString("#")+QString::number(ule.uin);
	// name
	QString name=ule.first_name;
	if(!ule.last_name.isEmpty())
	{
		if(!name.isEmpty())
			name+=" ";
		name+=ule.last_name;
	};
	if(!name.isEmpty())
	{
		if(!s.isEmpty())
			s+=", ";
		s+=name;
	};
	// description
	if(!ule.description.isEmpty())
	{
		if(!s.isEmpty())
			s+=" - ";
		s+=ule.description;
	};	
	descrtb->setText(s);
}

void Kadu::refreshGroupTabBar()
{
	if(!config.grouptabs)
	{
		group_bar->hide();
		return;
	};
	/* usuwamy wszystkie zakladki - od tylu,
	   bo indeksy sie przesuwaja po usunieciu */
	for(int i=group_bar->count()-1; i>=1; i--)
		group_bar->removeTab(group_bar->tabAt(i));
	/* dodajemy nowe zakladki */
	for(int i=0; i<userlist.count(); i++)
	{
		QString groups=userlist[i].group;
		QString group;
		for(int g=0; (group=groups.section(',',g,g))!=""; g++)
		{
			bool createNewTab=true;
			for(int j=0; j<group_bar->count(); j++)
				if(group_bar->tabAt(j)->text()==group)
					createNewTab=false;
			if(createNewTab)
				group_bar->addTab(new QTab(group));
		};
	};
	if(group_bar->count()==1)
		group_bar->hide();
	else
		group_bar->show();
	/* odswiezamy - dziala tylko jesli jest widoczny */
	group_bar->update();
};

void Kadu::setActiveGroup(const QString& group)
{
	userbox->clearUsers();
	for(int i=0; i<userlist.count(); i++)
	{
		bool belongsToGroup;
		if(group=="")
			belongsToGroup=true;
		else
		{
			belongsToGroup=false;
			QString user_groups=userlist[i].group;
			QString user_group;
			for(int g=0; (user_group=user_groups.section(',',g,g))!=""; g++)
				if(user_group==group)
					belongsToGroup=true;
		};
		if (belongsToGroup && (!userlist[i].anonymous || !dw))
			userbox->addUser(userlist[i].altnick);
	};
	UserBox::all_refresh();
};

void Kadu::groupTabSelected(int id)
{
	if(id==0)
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
	fprintf(stderr, "KK Kadu::userListStatusModified(): %d\n", user->uin);

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

void Kadu::removeUser(QStringList &users, bool permanently = false) {
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

	switch (QMessageBox::information(kadu, "Kadu", i18n("Save current userlist to file?"), i18n("Yes"), i18n("No"), QString::null, 0, 1) ) {
		case 0: // Yes?
			userlist.writeToFile();
			break;
		case 1:
			break;
		}
	refreshGroupTabBar();
}

void Kadu::blink() {
	int i;

	if (!doBlink && socket_active) {
		setCurrentStatus(loginparams.status & (~GG_STATUS_FRIENDS_MASK));
		return;
		}
	else
		if (!doBlink && !socket_active) {
	    		statuslabel->setPixmap(QPixmap((const char**)gg_inact_xpm) );
	    		if (dw)
				dw->setType((char **)gg_inact_xpm);
	    		return;
	    		}

	if (blinkOn) {
		statuslabel->setPixmap(QPixmap((const char**)gg_inact_xpm) );
		if (dw)
			dw->setType((char **)gg_inact_xpm);
		blinkOn = false;
		}
	else {
		i = statusGGToStatusNr(loginparams.status & (~GG_STATUS_FRIENDS_MASK));
		statuslabel->setPixmap(QPixmap((const char **)gg_xpm[i]));
		if (dw)
			dw->setType((char **)gg_xpm[i]);
		blinkOn = true;
		}

	blinktimer->start(1000, TRUE);
}

/* dcc initials */
void Kadu::prepareDcc(void) {
	struct in_addr in;
	
	if (!strcmp(config.dccip, "0.0.0.0")) {
		in.s_addr = getMyIP();
		if (!in.s_addr) {
			fprintf(stderr, "KK Cannot determine IP address!\n");
			return;
			}
		delete config.dccip;
		config.dccip = strdup(inet_ntoa(in));
		fprintf(stderr, "KK My IP address: %s\n", inet_ntoa(in));
		}

	dccsock = gg_dcc_socket_create(config.uin, 0);

	if (!dccsock) {
		perror("DCC");
		gg_dcc_free(dccsock);
		QMessageBox::warning(kadu, "", i18n("Couldn't create DCC socket.\nDirect connections disabled."));
		return;
		}

	gg_dcc_ip = inet_addr(config.dccip);
	gg_dcc_port = dccsock->port;
    
	fprintf(stderr, "KK Kadu::prepareDcc() DCC_IP=%s DCC_PORT=%d\n", config.dccip, dccsock->port);
    
	dccsnr = new QSocketNotifier(dccsock->fd, QSocketNotifier::Read, kadu);
	QObject::connect(dccsnr, SIGNAL(activated(int)), kadu, SLOT(dccReceived()));

	dccsnw = new QSocketNotifier(dccsock->fd, QSocketNotifier::Write, kadu);
	QObject::connect(dccsnw, SIGNAL(activated(int)), kadu, SLOT(dccSent()));
}

// code for addUser has been moved from adduser.cpp
// for sharing with search.cpp
void Kadu::addUser(const QString& FirstName, const QString& LastName,
	const QString& NickName, const QString& AltNick,
	const QString& Mobile, const QString& Uin, const int Status,
	const QString& Group, const QString& Description, const bool Anonymous)
{
	uint32_t uin = Uin.toUInt();

	if (!userlist.containsUin(uin) || (!uin && !userlist.containsAltNick(AltNick)))
		userlist.addUser(FirstName, LastName, NickName, AltNick, Mobile, Uin, Status, 
			false, false, true, Group, Description, Anonymous);
	else {
		UserListElement &ule = userlist.byUin(uin);
		if (!uin)
			ule = userlist.byAltNick(AltNick);
		userlist.changeUserInfo(ule.altnick,
			FirstName, LastName, NickName, AltNick, Mobile, ule.blocking, ule.offline_to_user,
			ule.notify, Group);
		}
	userlist.writeToFile();

	userbox->addUser(AltNick);
	UserBox::all_refresh();

	refreshGroupTabBar();

	if (!Anonymous && uin)
		gg_add_notify(sess, uin);
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
		chats[i].ptr->setActiveWindow();
		return i;
		}

	i = 0;
	while (i < chats.count() && !chats[i].uins.equals(senders))
		i++;

	fprintf(stderr, "KK Kadu::openChat(): return %d\n", i);

	return i;
}

/* menu and popup menu parser */
void Kadu::commandParser (int command) {
	int i = 0;
	SearchDialog *sd;
	UinsList uins;
	UserListElement user;
	QStringList users;
	QString keyfile_path;
	QString mykey;
	QFile keyfile;

	switch (command) {
		case KADU_CMD_SEND_MESSAGE:
			Message *msg;
			msg = new Message(userbox->currentText());
			msg->init();
			msg->show();
			break;
		case KADU_CMD_OPEN_CHAT:
			for (i = 0; i < userbox->count(); i++)
				if (userbox->isSelected(i)) {
					user = userlist.byAltNick(userbox->text(i));
					if (user.uin)
						uins.append(user.uin);
					}
			openChat(uins);
			break;
		case KADU_CMD_REMOVE_USER:
			for (i = 0; i < userbox->count(); i++)
				if (userbox->isSelected(i))
					users.append(userbox->text(i));
			removeUser(users);
			break;
		case KADU_CMD_DELETE_HISTORY:
			for (i = 0; i < userbox->count(); i++)
				if (userbox->isSelected(i))
					uins.append(userlist.byAltNick(userbox->text(i)).uin);
			confirmHistoryDeletion(uins);
			break;
		case KADU_CMD_SHOW_HISTORY:
			History *hb;
			for (i = 0; i < userbox->count(); i++)
				if (userbox->isSelected(i))
					uins.append(userlist.byAltNick(userbox->text(i)).uin);
			hb = new History(uins);
			hb->show();
			break;
		case KADU_CMD_USERINFO:
			UserInfo *ui;
			ui = new UserInfo("user info", 0, userbox->currentText());
			ui->show();
			break;
		case KADU_CMD_SEARCH:
			sd = new SearchDialog();
			sd->init();
			sd->show();
			break;
		case KADU_CMD_MUTE:
			mute = !mute;
			if (mute) {
				QPixmap snd_unmute((const char **)snd_mute_xpm);
				QIconSet icon(snd_unmute);
				mmb->changeItem(KADU_CMD_MUTE, icon, i18n("Unmute sounds"));
				}
			else {
				QPixmap snd_mute((const char **)snd_unmute_xpm);
				QIconSet icon(snd_mute);
				mmb->changeItem(KADU_CMD_MUTE, icon, i18n("Mute sounds"));		
				}
			break;
		case KADU_CMD_ADD_USER:
			Adduser *au;
			au = new Adduser(0, "add_user");
			au->show();
			break;
		case KADU_CMD_MANAGE_IGNORED:
			Ignored *ign;
			ign = new Ignored(0, "ignored");
			ign->show();
			break;
		case KADU_CMD_HELP:
			QProcess *help;
			help = new QProcess();
			help->addArgument("konqueror");
			help->addArgument(QString(DATADIR)+"/apps/kadu/doc/index_doc.html");
			help->start();
			delete help;
			break;
		case KADU_CMD_ABOUT:
			About *about;
			about = new About;
			about->show();
			break;
		case KADU_CMD_SEND_USERLIST:
			sendUserlist();
			break;
		case KADU_CMD_SMS:
			Sms *sms;
			sms = new Sms(userbox->currentText(), 0);
			sms->show();
			break;
		case KADU_CMD_MAINMENU_SMS:
			sms = new Sms("", 0);
			sms->show();
			break;
		case KADU_CMD_REMIND_PASSWORD:
			remindPassword *rp;	
			rp = new remindPassword();
			rp->start();
			break;
		case KADU_CMD_CHANGE_PASSWORD:
			changePassword *cp;	
			cp = new changePassword();
			cp->show();
			break;
		case KADU_CMD_REGISTER_USER:
			Register *reg;
			reg = new Register;
			reg->show();
			break;
		case KADU_CMD_UNREGISTER_USER:
			Unregister *ureg;
			ureg = new Unregister;
			ureg->show();
			break;
		case KADU_CMD_QUIT:
			pending.writeToFile();
			close_permitted = true;
			disconnectNetwork();
			close(true);
			break;
		case KADU_CMD_SEARCH_USER:
			sd = new SearchDialog(0, i18n("User info"), userlist.byAltNick(userbox->currentText()).uin);
			sd->init();
			sd->show();
			sd->firstSearch();
			break;
		case KADU_CMD_IMPORT_USERLIST:
			UserlistImport *uli;
			uli = new UserlistImport();
			uli->init();
			uli->show();
			break;
		case KADU_CMD_CONFIG:
			ConfigDialog *cd;
			cd = new ConfigDialog;
			cd->show();
			break;
		case KADU_CMD_EXPORT_USERLIST:
			UserlistExport *ule;
			ule = new UserlistExport();
			ule->init();
			ule->show();
			break;
		case KADU_CMD_HIDE:
			close_permitted = false;
			close(true);
			break;
		case KADU_CMD_SEND_FILE:
			struct gg_dcc *dcc_new;		
			user = userlist.byAltNick(userbox->currentText());
			
			dccSocketClass *dcc;
			if (user.port >= 10) {
				if ((dcc_new = gg_dcc_send_file(user.ip, user.port, config.uin, user.uin)) != NULL) {
					dcc = new dccSocketClass(dcc_new);
					connect(dcc, SIGNAL(dccFinished(dccSocketClass *)), this, SLOT(dccFinished(dccSocketClass *)));		    
					dcc->initializeNotifiers();
					}
				}
			else {
				acks.resize(acks.size() + 1);
				i = acks.size() - 1;
				acks[i].ack = 0;
				acks[i].seq = gg_dcc_request(sess, user.uin);
				acks[i].type = 0;
				acks[i].ptr = NULL;
				}
			break;
		case KADU_CMD_PERSONAL_INFO:
			PersonalInfoDialog *pid;
			pid = new PersonalInfoDialog();
			pid->show();
			break;
#ifdef HAVE_OPENSSL
		case KADU_CMD_SEND_KEY:
			user = userlist.byAltNick(userbox->currentText());

			keyfile_path.append(ggPath("keys/"));
			keyfile_path.append(QString::number(config.uin));
			keyfile_path.append(".pem");

			keyfile.setName(keyfile_path);

			if(keyfile.open(IO_ReadOnly)) {
				QTextStream t(&keyfile);
				mykey = t.read();
				keyfile.close();
				QCString tmp(mykey.local8Bit());
				gg_send_message(sess, GG_CLASS_MSG, user.uin, (unsigned char *)tmp.data());
				QMessageBox::information(this, "Kadu", i18n("Your public key has been sent"), i18n("OK"), QString::null, 0);
			}

			break;
#endif
		}
}

/* changes the active group */
void Kadu::changeGroup(int group) {
	activegrpno = group;
	grpmenu->setItemChecked(true, group-600);
	grpmenu->updateItem(group-600);
	grpmenu->repaint();
	std::cout << group-600 << std::endl;
}

/* the list that pops up if we right-click one someone */
void Kadu::listPopupMenu(QListBoxItem *item) {
	if (item == NULL)
		return;

	QPopupMenu * pm;
	pm = new QPopupMenu(this);

	KIconLoader *loader = KGlobal::iconLoader();

	QPixmap msg;
	msg = loader->loadIcon("mail_generic", KIcon::Small);

	UserListElement user;
	user = userlist.byAltNick(item->text());

	pm->insertItem(msg, i18n("Send message"), KADU_CMD_SEND_MESSAGE);
	pm->insertItem(i18n("Open chat window"), KADU_CMD_OPEN_CHAT);
	pm->insertItem(i18n("Send SMS"), KADU_CMD_SMS);
	if(user.mobile=="")
		pm->setItemEnabled(KADU_CMD_SMS,false);
	pm->insertItem(loader->loadIcon("filesave", KIcon::Small), i18n("Send file"), KADU_CMD_SEND_FILE);
	if (user.status == GG_STATUS_AVAIL || user.status == GG_STATUS_AVAIL_DESCR ||
		user.status == GG_STATUS_BUSY || user.status == GG_STATUS_BUSY_DESCR)
		pm->setItemEnabled(KADU_CMD_SEND_FILE, true);
	else
		pm->setItemEnabled(KADU_CMD_SEND_FILE, false);

#ifdef HAVE_OPENSSL
	pm->insertItem(loader->loadIcon("encrypted", KIcon::Small), i18n("Send my public key"), KADU_CMD_SEND_KEY);
	QString keyfile_path;

	keyfile_path.append(ggPath("keys/"));
	keyfile_path.append(QString::number(config.uin));
	keyfile_path.append(".pem");

	QFileInfo keyfile(keyfile_path);
	if (keyfile.permission(QFileInfo::ReadUser)) {
		pm->setItemEnabled(KADU_CMD_SEND_KEY, true);
	} else {
		pm->setItemEnabled(KADU_CMD_SEND_KEY, false);
	}
#endif

	pm->insertSeparator();
	
	pm->insertItem(loader->loadIcon("remove", KIcon::Small), i18n("Remove from userlist"), KADU_CMD_REMOVE_USER);
	pm->insertItem(loader->loadIcon("eraser", KIcon::Small), i18n("Clear history"), KADU_CMD_DELETE_HISTORY);
	QPixmap history;
	history = loader->loadIcon("history", KIcon::Small);
	pm->insertItem(history, i18n("View history"), KADU_CMD_SHOW_HISTORY);
	pm->insertItem(loader->loadIcon("identity", KIcon::Small), i18n("View/edit user info"), KADU_CMD_USERINFO);
	pm->insertItem(loader->loadIcon("viewmag", KIcon::Small), i18n("Lookup in directory"), KADU_CMD_SEARCH_USER);
	if (!user.uin) {
		pm->setItemEnabled(KADU_CMD_DELETE_HISTORY, false);
		pm->setItemEnabled(KADU_CMD_SHOW_HISTORY, false);
		pm->setItemEnabled(KADU_CMD_SEARCH_USER, false);
		pm->setItemEnabled(KADU_CMD_SEND_MESSAGE, false);
		pm->setItemEnabled(KADU_CMD_OPEN_CHAT, false);
		}
	pm->insertSeparator();
	pm->insertItem(i18n("About..."), KADU_CMD_ABOUT);

	connect(pm, SIGNAL(activated(int)), this, SLOT(commandParser(int)));	
	pm->exec(QCursor::pos());    	
}

/* if something's pending, open it, if not, open new message */
void Kadu::sendMessage(QListBoxItem *item) {
	QString tmp;
	int i,j,k = -1;
	bool stop = false;
	rMessage *rmsg;
	Message *msg;
	UinsList uins;
	PendingMsgs::Element elem;
	uin_t uin;
	QString toadd;

	uin = userlist.byAltNick(item->text()).uin;

	if (!uin) {
		Sms *sms;
		sms = new Sms(userbox->currentText(), 0);
		sms->show();
		return;
		}

	for (i = 0; i < pending.count(); i++) {
		elem = pending[i];
		if ((!uins.count() && elem.uins.contains(uin)) || (uins.count() && elem.uins.equals(uins)))
			if ((elem.msgclass & GG_CLASS_CHAT) == GG_CLASS_CHAT
				|| (elem.msgclass & GG_CLASS_MSG) == GG_CLASS_MSG) {
				if (!uins.count())
					uins = elem.uins;
				for (j = 0; j < elem.uins.count(); j++)
					if (!userlist.containsUin(elem.uins[j])) {
						tmp = QString::number(pending[i].uins[j]);
						if (dw)
							userlist.addUser("", "", tmp, tmp, "", tmp, GG_STATUS_NOT_AVAIL,
								false, false, true, "", "", true);
						else
							addUser("", "", tmp, tmp, "", tmp, GG_STATUS_NOT_AVAIL,
								"", "", true);
						}
				k = openChat(elem.uins);
				chats[k].ptr->formatMessage(false,
					userlist.byUin(elem.uins[0]).altnick, elem.msg,
					timestamp(elem.time), toadd);	    
				deletePendingMessage(i);
				fprintf(stderr, "KK Kadu::sendMessage(): k=%d\n", k);
				i--;
				stop = true;
				}
	    		else {
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
				}
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

	if (uins.count() > 1 || (userlist.byUin(uins[0]).status != GG_STATUS_NOT_AVAIL
		&& userlist.byUin(uins[0]).status != GG_STATUS_NOT_AVAIL_DESCR))
		openChat(uins);
	else {
		msg = new Message(item->text());
		msg->init();
		msg->show();
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
			autohammer = false;
			disconnectNetwork();
			setCurrentStatus(GG_STATUS_NOT_AVAIL);
			break;
		case 7:
			cd = new ChooseDescription(7);
			if (cd->exec() == QDialog::Accepted) {
				setStatus(GG_STATUS_NOT_AVAIL_DESCR);
				statusppm->setItemEnabled(7, false);
				autohammer = false;
				disconnectNetwork();
				}
			break;	    
		case 8:
			statusppm->setItemChecked(8, !statusppm->isItemChecked(8));
			dockppm->setItemChecked(8, !dockppm->isItemChecked(8));	    
			config.privatestatus = statusppm->isItemChecked(8);
			if (!statusppm->isItemChecked(6) && !statusppm->isItemChecked(7))
				setStatus(sess->status & (~GG_STATUS_FRIENDS_MASK));
			break;
		}
}

void Kadu::setCurrentStatus(int status) {
	int statusnr;
	int i;
	i = 0;
	statusnr = statusGGToStatusNr(status);
	while (i<8) {
		statusppm->setItemChecked(i, false);
		dockppm->setItemChecked(i, false);
		i++;
		}
	statusppm->setItemChecked(statusnr, true);
	dockppm->setItemChecked(statusnr, true);	

	statuslabeltxt->setText(i18n(statustext[statusnr]));
	statusppm->setItemEnabled(7, statusnr != 6);
	dockppm->setItemEnabled(7, statusnr != 6);
	statuslabel->setPixmap(QPixmap((const char**)gg_xpm[statusnr]));
	setIcon(QPixmap((const char**)gg_xpm[statusnr]));
	if (!pending.pendingMsgs() && dw)
		dw->setType((char **)gg_xpm[statusnr]);
}

void Kadu::slotShowStatusMenu() {
	QPoint point = statuslabeltxt->mapToGlobal(QPoint(0, 0));
	point.setY(point.y()-statusppm->height());
	statusppm->popup(point);
}

void Kadu::setStatus(int status) {

	fprintf(stderr, "KK Kadu::setStatus(): setting status: %d\n",
		status | (GG_STATUS_FRIENDS_MASK * config.privatestatus));

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
			descr = (unsigned char *)strdup((const char *)own_description.local8Bit());
			iso_to_cp(descr);
			if (status == GG_STATUS_NOT_AVAIL_DESCR)
				gg_change_status_descr(sess, status, (const char *)descr);
			else
				gg_change_status_descr(sess,
					status | (GG_STATUS_FRIENDS_MASK * config.privatestatus), (const char *)descr);
			delete descr;
			}
		else
			gg_change_status(sess, status | (GG_STATUS_FRIENDS_MASK * config.privatestatus));
		if (sess->check & GG_CHECK_WRITE)
			kadusnw->setEnabled(true);
	
		setCurrentStatus(status);

		fprintf(stderr, "KK Kadu::setStatus(): actual status: %d\n", sess->status);

		return;
		}

/*    if (!autohammer) {
	if (progresswindow) {
//	    if (progresswindow->isVisible())
//		progresswindow->close();
	    delete progresswindow;
	    }
	progresswindow = new Operation(i18n("Connecting to network"), i18n("Cancel"), 7 ); // <-- this is the progress bar, in case the name misleads you ;)
	}*/

	if (config.allowdcc)
		prepareDcc();

	/* use proxy? */
	if (config.useproxy) {
		char * gg_proxy_username;
		char * gg_proxy_password;
		gg_proxy_host = (char *)config.proxyaddr.latin1();
		gg_proxy_port = config.proxyport;
		if (config.proxyuser.length()) {
			gg_proxy_username = (char *)config.proxyuser.latin1();
			gg_proxy_password = (char *)config.proxypassword.latin1();
			}
		else
			gg_proxy_username = gg_proxy_password = NULL;
		gg_proxy_enabled = 1;
		}
	else
		gg_proxy_enabled = 0;

	loginparams.status = status | (GG_STATUS_FRIENDS_MASK * config.privatestatus);
	loginparams.password = (char *)config.password.latin1();
	loginparams.uin = config.uin;
	loginparams.client_version = GG_DEFAULT_CLIENT_VERSION;
	loginparams.has_audio = 1;
	if (config.allowdcc && inet_addr(config.extip) && config.extport > 1023) {
		loginparams.external_addr = inet_addr(config.extip);
		loginparams.external_port = config.extport;
		}
	else {
		loginparams.external_addr = 0;
		loginparams.external_port = 0;
		}	
	if (config.servers.count() && inet_addr(config.servers[server_nr].latin1()) != INADDR_NONE) {
		loginparams.server_addr = inet_addr(config.servers[server_nr].latin1());
		loginparams.server_port = 8074;
		server_nr++;
		if (server_nr >= config.servers.count())
			server_nr = 0;
		}
	else {
		switch (server_nr) {
			case 0: loginparams.server_addr = 0;loginparams.server_port = 0; break;
			case 1: loginparams.server_addr = inet_addr("217.17.41.85");loginparams.server_port = 8074; break;
			case 2: loginparams.server_addr = inet_addr("217.17.41.84");loginparams.server_port = 8074; break;
			case 3: loginparams.server_addr = inet_addr("217.17.41.83");loginparams.server_port = 8074; break;
			}
		server_nr++;
		if (server_nr > 3)
			server_nr=0;
		}
	sess = gg_login(&loginparams);

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
		QMessageBox::warning(kadu, i18n("Connection problem"), i18n("Couldn't connect.\nCheck your internet connection."));
		}
}

void Kadu::checkConnection(void) {
	// Since it doesn't work anymore...
	readevent->start(10000, TRUE);
	return;	
}

void Kadu::dccFinished(dccSocketClass *dcc) {
	fprintf(stderr, "KK dccFinished\n");
	delete dcc;
}

bool Kadu::event(QEvent *e) {
	QCustomEvent *ce;
	dccSocketClass *dcc;
	dccSocketClass **data;

	if (e->type() == QEvent::User) {
		fprintf(stderr, "KK Kadu::event()\n");
		ce = (QCustomEvent *)e;
		data = (dccSocketClass **)ce->data();
		dcc = *data;
		switch (dcc->state) {
			case DCC_SOCKET_TRANSFER_FINISHED:
				QMessageBox::information(0, i18n("Information"),
					i18n("File has been transferred sucessfully."), i18n("&OK"));
				break;
			case DCC_SOCKET_TRANSFER_DISCARDED:
				QMessageBox::information(0, i18n("Information"), 
				i18n("File transfer has been discarded."), i18n("&OK"));
				break;
			case DCC_SOCKET_TRANSFER_ERROR:
			case DCC_SOCKET_CONNECTION_BROKEN:
				QMessageBox::information(0, i18n("Error"), i18n("File transfer error!"),
				i18n("&OK"));
				break;
			case DCC_SOCKET_COULDNT_OPEN_FILE:
				QMessageBox::information(0, i18n("Error"), i18n("Couldn't open file!"),
				i18n("&OK"));
				break;
			}
		delete data;
		delete dcc;
		ce->setData(NULL);
		}
	return QWidget::event(e);
}

void Kadu::dccReceived(void) {
	fprintf(stderr, "KK Kadu::dccReceived()\n");
	watchDcc();
}

void Kadu::dccSent(void) {
	fprintf(stderr, "KK Kadu::dccSent()\n");
	dccsnw->setEnabled(false);
	if (dccsock->check & GG_CHECK_WRITE)
		watchDcc();
}

void Kadu::watchDcc(void) {
	fprintf(stderr, "KK Kadu::watchDcc(): data on socket\n");			
	if (!(dcc_e = gg_dcc_watch_fd(dccsock))) {
		fprintf(stderr, "KK Kadu::watchDcc(): Connection broken unexpectedly!\n");
		config.allowdcc = false;
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
			fprintf(stderr, "KK GG_EVENT_DCC_ERROR\n");
			break;
		case GG_EVENT_DCC_NEW:
			dccSocketClass *dcc;    
			dcc = new dccSocketClass(dcc_e->event.dcc_new);
			connect(dcc, SIGNAL(dccFinished(dccSocketClass *)), this, SLOT(dccFinished(dccSocketClass *)));
			dcc->initializeNotifiers();
			fprintf(stderr, "KK GG_EVENT_DCC_NEW: spawning object\n");
			break;
		default:
			break;
		}

	if (dccsock->check == GG_CHECK_WRITE)
		dccsnw->setEnabled(true);

	gg_free_event(dcc_e);
}

void Kadu::dataReceived(void) {
	fprintf(stderr, "KK Kadu::dataReceived()\n");
	if (sess->check && GG_CHECK_READ)
		eventHandler(GG_CHECK_READ);
}

void Kadu::dataSent(void) {
	fprintf(stderr, "KK Kadu::dataSent()\n");
	kadusnw->setEnabled(false);
	if (sess->check & GG_CHECK_WRITE)
		eventHandler(GG_CHECK_WRITE);
}

/* the main network function called every n msec from QTimer */
void Kadu::eventHandler(int state) {
	int i;
	static int calls = 0;

	fprintf(stderr, "KK Kadu::eventHandler()\n");
	calls++;
	if (calls > 1)
		fprintf(stderr, "************* KK Kadu::eventHandler(): Recursive eventHandler calls detected!\n");
	if (!(e = gg_watch_fd(sess))) {
		fprintf(stderr,"KK Kadu::eventHandler(): Connection broken unexpectedly!\n");
		char error[512];
		disconnectNetwork();
		snprintf(error, sizeof(error), "KK Kadu::eventHandler(): Unscheduled connection termination\n");
		fprintf(stderr, error);
		setCurrentStatus(GG_STATUS_NOT_AVAIL);
		//QMessageBox::warning(kadu, "Connect error", error );
		gg_free_event(e);
		if (autohammer)
			setStatus(loginparams.status & (~GG_STATUS_FRIENDS_MASK));
		calls--;
		return;	
		}
	if (sess->state == GG_STATE_CONNECTING_HUB || sess->state == GG_STATE_CONNECTING_GG) {
		fprintf(stderr, "KK Kadu::eventHandler(): changing QSocketNotifiers.\n");

		kadusnw->setEnabled(false);
		delete kadusnw;

		kadusnr->setEnabled(false);
		delete kadusnr;

		kadusnw = new QSocketNotifier(sess->fd, QSocketNotifier::Write, this); 
		QObject::connect(kadusnw, SIGNAL(activated(int)), kadu, SLOT(dataSent()));

		kadusnr = new QSocketNotifier(sess->fd, QSocketNotifier::Read, this); 
		QObject::connect(kadusnr, SIGNAL(activated(int)), kadu, SLOT(dataReceived()));    
		}

	switch (sess->state) {
		case GG_STATE_RESOLVING:
			fprintf(stderr, "KK Kadu::eventHandler(): Resolving address\n");
			break;
		case GG_STATE_CONNECTING_HUB:
			fprintf(stderr, "KK Kadu::eventHandler(): Connecting to hub\n");
			break;
		case GG_STATE_READING_DATA:
			fprintf(stderr, "KK Kadu::eventHandler(): Fetching data from hub\n");
			break;
		case GG_STATE_CONNECTING_GG:
			fprintf(stderr, "KK Kadu::eventHandler(): Connecting to server\n");
			break;
		case GG_STATE_READING_KEY:
			fprintf(stderr, "KK Kadu::eventHandler(): Waiting for hash key\n");
			break;
		case GG_STATE_READING_REPLY:
			fprintf(stderr, "KK Kadu::eventHandler(): Sending key\n");
			break;
		case GG_STATE_CONNECTED:
			break;
		default:
			break;
		}

	if (sess->check == GG_CHECK_READ) {
		timeout_connected = true;
		last_read_event = time(NULL);
		}

	if (e->type == GG_EVENT_MSG) {
		if (e->event.msg.msgclass == GG_CLASS_CTCP) {
			struct gg_dcc *dcc_new;

			UserListElement user;
			user = userlist.byUin(e->event.msg.sender);
			dccSocketClass *dcc;
			dcc_new = gg_dcc_get_file(user.ip, user.port, config.uin, e->event.msg.sender);
			dcc = new dccSocketClass(dcc_new);
			connect(dcc, SIGNAL(dccFinished(dccSocketClass *)), this, SLOT(dccFinished(dccSocketClass *)));		    
			dcc->initializeNotifiers();
			}
		else {
			UinsList uins;
			fprintf(stderr, "KK eventHandler(): %d\n", e->event.msg.recipients_count);
			if (e->event.msg.msgclass == GG_CLASS_CHAT) {
				uins.append(e->event.msg.sender);	
				for (i = 0; i < e->event.msg.recipients_count; i++)
					uins.append(e->event.msg.recipients[i]);
				}
			else
				uins.append(e->event.msg.sender);
			eventRecvMsg(e->event.msg.msgclass, uins, e->event.msg.message, e->event.msg.time, 0, NULL);
			}
		}

	if (e->type == GG_EVENT_PUBDIR50_SEARCH_REPLY
		|| e->type == GG_EVENT_PUBDIR50_READ || e->type == GG_EVENT_PUBDIR50_WRITE) {
		gg_pubdir50_t res = e->event.pubdir50;
		uint32_t seq = gg_pubdir50_seq(res);
		i = 0;
		while (i < SearchList.count() && SearchList[i].seq != seq)
			i++;
		if (i < SearchList.count())
			switch (SearchList[i].type) {
				case DIALOG_SEARCH:
					((SearchDialog *)SearchList[i].ptr)->showResults(res);
					break;
				case DIALOG_PERSONAL:
					((PersonalInfoDialog *)SearchList[i].ptr)->fillFields(res);
					break;
				}
		}

	if (e->type == GG_EVENT_NOTIFY_DESCR || e->type == GG_EVENT_NOTIFY) {
		eventGotUserlist(e);
		UserBox::all_refresh();
		}

	if (e->type == GG_EVENT_STATUS) {
		eventStatusChange(e);
		UserBox::all_refresh();
		}

	if (e->type == GG_EVENT_ACK) {
		fprintf(stderr, "KK Kadu::eventHandler(): message reached %d (seq %d)\n", e->event.ack.recipient, e->event.ack.seq);
		ackHandler(e->event.ack.seq);
		}

	if (e->type == GG_EVENT_CONN_SUCCESS) {
		doBlink = false;
		sendUserlist();

		setCurrentStatus(loginparams.status & (~GG_STATUS_FRIENDS_MASK));

		userlist_sent = true;

		if (ifStatusWithDescription(loginparams.status))
			setStatus(loginparams.status & (~GG_STATUS_FRIENDS_MASK));
			
/* uruchamiamy autoawaya(jezeli wlaczony) po wyslaniu userlisty i ustawieniu statusu */
		if (config.autoaway)
			AutoAwayTimer::on();
/* jezeli sie rozlaczymy albo stracimy polaczenie, proces laczenia sie z serwerami zaczyna sie od poczatku */
		server_nr=0;
		pingtimer = new QTimer;
		QObject::connect(pingtimer, SIGNAL(timeout()), kadu, SLOT(pingNetwork()));
		pingtimer->start(60000, TRUE);
		
		readevent = new QTimer;
		QObject::connect(readevent, SIGNAL(timeout()), kadu, SLOT(checkConnection()));    
		readevent->start(10000, TRUE);
		}

	if (e->type == GG_EVENT_CONN_FAILED) {
		char error[512];
		snprintf(error, sizeof(error), "KK Kadu::eventHandler(): Unable to connect, the following error has occured:\n%s\nKK Kadu::eventHandler(): Keep trying to connect?\n", strerror(errno));
		disconnectNetwork();	
		setCurrentStatus(GG_STATUS_NOT_AVAIL);
		fprintf(stderr, "KK Kadu::eventHandler(): Connection failed\n");
		fprintf(stderr, error);

		if (autohammer)
			setStatus(loginparams.status & (~GG_STATUS_FRIENDS_MASK));
		}

	if (socket_active) {
		if (sess->state == GG_STATE_IDLE && userlist_sent) {
			char error[512];
			socket_active = false;
			UserBox::all_changeAllToInactive();
			snprintf(error, sizeof(error), "KK Kadu::eventHandler(): Unscheduled connection termination\n");
			fprintf(stderr, error);
			disconnectNetwork();			
			setCurrentStatus(GG_STATUS_NOT_AVAIL);
			if (autohammer)
				setStatus(config.defaultstatus & (~GG_STATUS_FRIENDS_MASK));
			}
		else
			if (sess->check & GG_CHECK_WRITE)
				kadusnw->setEnabled(true);
		}

	gg_free_event(e);

	calls--;
}

void Kadu::pingNetwork(void) {
	fprintf(stderr, "KK Kadu::pingNetwork()\n");
	gg_ping(sess);
	pingtimer->start(60000, TRUE);
}

void Kadu::disconnectNetwork() {
	int i;

	doBlink = false;
	fprintf(stderr, "KK Kadu::disconnectNetwork(): calling offline routines\n");

	if (config.autoaway)
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
	if (readevent) {
		readevent->stop();
		delete readevent;
		readevent = NULL;
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
	if (progresswindow) {
//	if (progresswindow->isVisible())
//	    progresswindow->close();
		delete progresswindow;
		progresswindow = NULL;
		}

	disconnect_planned = true;
	if (sess) {
		gg_logoff(sess);
		gg_free_session(sess);
		sess = NULL;
		}
	userlist_sent = false;

	i = 0;
	while (i < userlist.count()) {
		userlist[i].status = GG_STATUS_NOT_AVAIL;
		i++;
		}

	i = 0;
	while (i < chats.count()) {
		chats[i].ptr->setTitle();
		i++;
		}

	UserBox::all_refresh();

	socket_active = false;
	statuslabel->setPixmap(QPixmap((const char**)gg_inact_xpm));
	if (dw)
		dw->setType((char **)gg_inact_xpm);
	setIcon(QPixmap((const char**)gg_inact_xpm));

}

void Kadu::cleanUp(void) {
	writeIgnored(NULL);
}

Kadu::~Kadu(void) {
}

void Kadu::createMenu() {

	KIconLoader *loader = KGlobal::iconLoader();

	mmb = new KMenuBar(this, "mmb");

	QPopupMenu *ppm = new QPopupMenu(this, "ppm");
	ppm->insertItem(i18n("Manage &ignored"), KADU_CMD_MANAGE_IGNORED);
	ppm->insertItem(loader->loadIcon("configure", KIcon::Small), i18n("&Configuration"), KADU_CMD_CONFIG);
	ppm->insertItem(loader->loadIcon("reload", KIcon::Small), i18n("Resend &userlist"), KADU_CMD_SEND_USERLIST);
	if (mute) {
		QPixmap snd_unmute((const char **)snd_mute_xpm);
		QIconSet icon(snd_unmute);
		ppm->insertItem(icon, i18n("Unmute sounds"), KADU_CMD_MUTE);
		}
	else {
		QPixmap snd_mute((const char **)snd_unmute_xpm);
		QIconSet icon(snd_mute);
		ppm->insertItem(icon, i18n("Mute sounds"), KADU_CMD_MUTE);		
		}
	ppm->insertSeparator();

	grpmenu = new QPopupMenu(this);
	grpmenu->insertItem(i18n("All"), 600);
	grpmenu->insertSeparator();

	ppm->insertItem(i18n("Remind &password"), KADU_CMD_REMIND_PASSWORD);
	ppm->insertItem(i18n("&Change password"), KADU_CMD_CHANGE_PASSWORD);
	QPixmap new__user((const char **)new_user);
	ppm->insertItem(new__user,i18n("Register &new user"), KADU_CMD_REGISTER_USER);
	ppm->insertItem(i18n("Unregister user"), KADU_CMD_UNREGISTER_USER);
	ppm->insertItem(i18n("Personal information"), KADU_CMD_PERSONAL_INFO);
	ppm->insertSeparator();
	QPixmap find;
	find = loader->loadIcon("viewmag", KIcon::Small);
	ppm->insertItem(find, i18n("&Search for users"), KADU_CMD_SEARCH);
	ppm->insertItem(i18n("I&mport userlist"), KADU_CMD_IMPORT_USERLIST);
	ppm->insertItem(i18n("E&xport userlist"), KADU_CMD_EXPORT_USERLIST);
	ppm->insertItem(QPixmap((const char **)gg_act_xpm),i18n("&Add user"), KADU_CMD_ADD_USER);
	ppm->insertItem(i18n("Send SMS"), KADU_CMD_MAINMENU_SMS);
	ppm->insertSeparator();	
	ppm->insertItem(i18n("H&elp"), KADU_CMD_HELP);	
	ppm->insertItem(i18n("A&bout..."), KADU_CMD_ABOUT);
	ppm->insertSeparator();
	ppm->insertItem(i18n("&Hide Kadu"), KADU_CMD_HIDE);
	ppm->insertItem(loader->loadIcon("exit", KIcon::Small), i18n("&Exit Kadu"), KADU_CMD_QUIT);

	mmb->insertItem(i18n("&Kadu"), ppm);
//	mmb->polish();

	connect(ppm, SIGNAL(activated(int)), this, SLOT(commandParser(int)));
}

void Kadu::createStatusPopupMenu() {

	QPixmap pixmap;
	QIconSet icon;

	statusppm = new QPopupMenu(this, "statusppm");
	dockppm = new KPopupMenu(this, "dockppm");

	for (int i=0; i<8; i++) {
		pixmap = QPixmap((const char **)gg_xpm[i]);
		icon = QIconSet(pixmap);
		statusppm->insertItem(icon, i18n((const char *)statustext[i]), i);
		dockppm->insertItem(icon, i18n((const char *)statustext[i]), i);
		}

	statusppm->insertSeparator();
	dockppm->insertSeparator();
	statusppm->insertItem(i18n("Private"),8);
	statusppm->setItemChecked(8, config.privatestatus);
	dockppm->insertItem(i18n("Private"),8);
	dockppm->setItemChecked(8, config.privatestatus);

	statusppm->setCheckable(true);
	dockppm->setCheckable(true);
	statusppm->setItemChecked(6, true);

	connect(statusppm, SIGNAL(activated(int)), this, SLOT(slotHandleState(int)));
}

void Kadu::closeEvent(QCloseEvent *e) {
	if (!close_permitted && dw) {
		e->ignore();
		hide();
		}
	else {
		config.splitsize.setWidth(userbox->size().height());
		config.splitsize.setHeight(descrtb->size().height());
		config.geometry = geometry();
		saveKaduConfig();
		fprintf(stderr,"KK closeEvent(): Graceful shutdown...\n");
		e->accept();
		}
}

void Kadu::setClosePermitted(bool permitted)
{
	close_permitted=permitted;
};

void MyLabel::mousePressEvent (QMouseEvent * e) {
	if (e->button() == Qt::LeftButton)
		kadu->slotShowStatusMenu();
}

void Kadu::showdesc(void) {
	descrtb->show();
}

void Kadu::hidedesc(void) {
	descrtb->hide();
}

#include "kadu.moc"
