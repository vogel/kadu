/***************************************************************************
                          kadu.cpp  -  description
                             -------------------
    begin                : pia lip 06 00:16:52 CEST 2002
    copyright            : (C) 2002 by tomee, chilek, adrian
    email                : tomee@cpi.pl, chilek@chilan.com, arians@aska.com.pl
 ***************************************************************************/

/*                                                                         *
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
#include <kpopupmenu.h>
#include <qtextcodec.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <qtimer.h>
#include <qaccel.h>
#include <qpainter.h>
#include <qmenubar.h>

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
#include <net/route.h>

#include <pthread.h>
#include <ctype.h>
#include <errno.h>

#include <pwd.h>
#include <sys/stat.h>

#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "../libgadu/lib/libgadu.h"

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
//

extern void ackHandler(int);
extern void UinToUserlistEntry (uin_t, int);

bool userlist_sent = FALSE;
bool socket_active = FALSE;
bool disconnect_planned = FALSE;
int userlist_count = 0;
int last_ping;
int last_read_event = -1;
bool timeout_connected = true;
bool i_wanna_be_invisible = true;
bool i_am_busy = false;
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
DockWidget * dw;
QPopupMenu * grpmenu;

UserList userlist;
PendingMsgs pending;
QValueList<struct chats> chats;
struct gg_session sess;
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

char **gg_xpm[8] = {gg_act_xpm, gg_actdescr_xpm, gg_busy_xpm, gg_busydescr_xpm,
	gg_invi_xpm, gg_invidescr_xpm, gg_inact_xpm, gg_inactdescr_xpm};
int gg_statuses[8] = {GG_STATUS_AVAIL, GG_STATUS_AVAIL_DESCR, GG_STATUS_BUSY, GG_STATUS_BUSY_DESCR,
	GG_STATUS_INVISIBLE, GG_STATUS_INVISIBLE_DESCR, GG_STATUS_NOT_AVAIL, GG_STATUS_NOT_AVAIL_DESCR};
const char *statustext[8] = {"Online", "Online (d.)",
	"Busy", "Busy (d.)",
	"Invisible", "Invisible (d.)",
	"Offline", "Offline (d.)"};
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
	KADU_CMD_ABOUT,
	KADU_CMD_SEND_USERLIST,
	KADU_CMD_SMS,
	KADU_CMD_REMIND_PASSWORD,
	KADU_CMD_REGISTER_USER,
	KADU_CMD_QUIT,
	KADU_CMD_SEARCH_USER,
	KADU_CMD_EXPORT_USERLIST,
	KADU_CMD_CONFIG,
	KADU_CMD_IMPORT_USERLIST,
	KADU_CMD_HIDE,
	KADU_CMD_SEND_FILE,
	KADU_CMD_PERSONAL_INFO
};

/* our own description container */
QString own_description;

void cp_to_iso(unsigned char *buf) {
    while (*buf) {
	if (*buf == (unsigned char)'¥') *buf = '¡';
        if (*buf == (unsigned char)'¹') *buf = '±';
        if (*buf == 140) *buf = '¦';
        if (*buf == 156) *buf = '¶';
        if (*buf == 143) *buf = '¬';
        if (*buf == 159) *buf = '¼';

        buf++;
        }
}

void iso_to_cp(unsigned char *buf) {
    while (*buf) {
	if (*buf == (unsigned char)'¡') *buf = '¥';
        if (*buf == (unsigned char)'±') *buf = '¹';
        if (*buf == (unsigned char)'¦') *buf = 140;
        if (*buf == (unsigned char)'¶') *buf = 156;
        if (*buf == (unsigned char)'¬') *buf = 143;
        if (*buf == (unsigned char)'¼') *buf = 159;

        buf++;
        }
}

unsigned long getMyIP(void) {
    unsigned long dest, gw;
    int flags, fd;
    FILE *file;
    char buf[256],name[32];
    bool stopped = false;
    char *iface;
    struct ifreq ifr;
    
    file = fopen("/proc/net/route", "r");
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

int statusGGToStatusNr(int status) {
    int i = 0;
    if (status == GG_STATUS_INVISIBLE2)
	return 4;
    while (i < 8 && gg_statuses[i] != status)
	i++;
    if (i < 8)
	return i;
    return -1;
}

void deletePendingMessage(int nr) {
	pending.deleteMsg(nr);

	fprintf(stderr, "KK deletePendingMessage()\n");

	if (pending.pendingMsgs())
		dw->setType((char **)gg_msg_xpm);
	else	
		dw->setType((char **)gg_xpm[statusGGToStatusNr(getActualStatus() & (~GG_STATUS_FRIENDS_MASK))]);
}

/* zwraca nasz aktualny status 
 jesli stan sesji jest inny niz polaczone to znaczy
 ze jestesmy niedostepni */
int getActualStatus() {
    if (sess.state == GG_STATE_CONNECTED)
	return sess.status;
    
    return GG_STATUS_NOT_AVAIL;
}

/* sprawdza czy nasz status jest opisowy
 odporne na podanie status'u z maska dla przyjaciol */
bool ifStatusWithDescription(int status) {
    status = status & (~GG_STATUS_FRIENDS_MASK);

    return (status == GG_STATUS_AVAIL_DESCR || status == GG_STATUS_NOT_AVAIL_DESCR ||
	status == GG_STATUS_BUSY_DESCR || status == GG_STATUS_INVISIBLE_DESCR);
}

/*bool ifPendingMessages(int uin) {
    bool pendings = false;
    int i;
    
    switch (uin) {
	case -1:
	    pendings = pending.count();
	    break;
	default:
	    for (i = 0; i < pending.size(); i++)
    		if (pending[i].uins[0] == uin)
		    pendings = true;
	}
    return pendings;
}*/

/* reading only ignore file right now */
void readConfig(void)
{
    FILE *f;
    char buf[256];

    char *path = getenv("HOME");
    char *path3 = "/.gg/ignore";
    char buffer[255], buffer2[256];
    snprintf(buffer2,256,"%s%s",path,path3);	

    ignore: if (!(f = fopen(buffer2, "r"))) {
	fprintf(stderr, "readConfig(): Failed to open ignore file. Ignore list empty. Need to read manual?\n");
	return;
	}

    while (fgets(buf, sizeof(buf) -1, f)) {
	if (buf[strlen(buf) - 1] == '\n')
	    buf[strlen(buf) - 1] = 0;

	if (buf[0] == '#')
	    continue;

	addIgnored(atoi(buf));
	}
}

char *pwHash(const char *tekst) {
    char *nowytekst;
    nowytekst = strdup(tekst);
    int ile, znak;
    for (ile = 0; ile < strlen(tekst); ile++) {
	znak = nowytekst[ile]^ile^1;
	nowytekst[ile] = znak;
	}
    return nowytekst;
}

void confirmHistoryDeletion(UinsList &uins) {
	QString fname;
	QString s;
	int i;

	switch(QMessageBox::information( kadu, "Kadu",i18n("Clear history?"), i18n("Yes"), i18n("No"), QString::null, 1, 1)) {
		case 0: // Yes?
			fname = getenv("HOME");
			fname.append("/.gg/history/");
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

void remindPassword() {
    struct gg_http *h = gg_remind_passwd(config.uin, 0);
    struct gg_pubdir *dupa;

    dupa = (gg_pubdir *) h->data;

    if (h && dupa->success)
	fprintf(stderr,"KK remindPassword(): Done\n");
    else
	fprintf(stderr,"KK remindPassword(): Problem!\n");

    gg_free_remind_passwd(h);
}

unsigned int GetStatusFromUserlist(uin_t uin) {
	UserListElement user;
	user = userlist.byUin(uin);
	return user.status;
}

/* sends the userlist. ripped off EKG, actually, but works */
void sendUserlist() {
	uin_t *uins;
	int i;

	uins = (uin_t *) malloc(userlist.count() * sizeof(uin_t));
	
	for (i = 0; i < userlist.count(); i++)
		uins[i] = userlist[i].uin;

	/* we were popping up sometimes, so let's keep the server informed */
	if (i_wanna_be_invisible)
		gg_change_status(&sess, GG_STATUS_INVISIBLE);

	gg_notify(&sess, uins, userlist.count());
	fprintf(stderr, "KK send_userlist(): Userlist sent\n");

	free(uins);
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

	/* we're not auto away yet */
	autoawayed = false;

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
        
	if (config.savegeometry) {
		fprintf(stderr,"KK Setting geometry to %d %d %d %d\n",
			config.geometry.x(), config.geometry.y(),
			config.geometry.width(), config.geometry.height());
		setGeometry(config.geometry);
		}
	else {
		resize(130,250);
		move(maximumSize().width() - x(), maximumSize().height() - y());
		}
    
	/* use dock icon? */
	if (config.dock) {
		dw = new DockWidget(this);
		dw->show();
	}
	
	/* read the userlist */
	userlist.readFromFile();
	/* read config, ignores actually, don't wanna rename it */
	readConfig();

	/* DCC */

	/*char buf[255];
	char buf2[255];
	strncpy(buf,"Kadu:",255);
	snprintf(buf2,255,"%s %d",buf,config.uin);*/
	QString buf;
	buf.append("Kadu: ");
	buf.append(QString::number(config.uin));

	/* use proxy? */
	if (config.useproxy) {
		gg_proxy_host = config.proxyaddr;
		gg_proxy_port = config.proxyport;
		gg_proxy_enabled = 1;
		}

	/* a newbie? */
	if (!config.uin) {
		QString path_;
		path_ = preparePath("");
		mkdir(path_.local8Bit(), 0700);
		path_.append("/history/");
		mkdir(path_.local8Bit(), 0700);
		switch (QMessageBox::information( kadu, "Kadu",
	    		i18n("You don't have a config file.\nWhat would you like to do?"),"New UIN", "Configure", "Cancel", 0, 1) ) {
			case 1: // Configure
				ConfigDialog *cd;
				cd = new ConfigDialog;
				cd->show();
				break;
	    		case 0: // Register
				Register *reg;
				reg = new Register;
				reg->show();
				break;
	    		case 2: // Nothing
				break;
			}
		setCaption("Kadu: new user");
		}
	else
		setCaption(i18n("Kadu: %1").arg(config.uin));

	/* initialize and configure userbox */
	userbox = new UserBox(this);
	userbox->setPaletteBackgroundColor(QColor(config.colors.userboxBgR,config.colors.userboxBgG,config.colors.userboxBgB));
	userbox->setPaletteForegroundColor(QColor(config.colors.userboxFgR,config.colors.userboxFgG,config.colors.userboxFgB));
	userbox->QListBox::setFont(QFont(config.colors.userboxFont, config.colors.userboxFontSize));


	for (int i = 0; i < userlist.count(); i++)
		userbox->addUin(userlist[i].uin);
	UserBox::all_refresh();
	
	/* start auto away. yes, even if it's disabled. this way enabling it will work at run-time */
	autoaway = new QTimer(this);
	autoaway->start(config.autoawaytime * 1000, TRUE);
	connect(autoaway, SIGNAL( timeout() ), this, SLOT( autoAway() ));

	connect(userbox, SIGNAL(doubleClicked(QListBoxItem *)), this, SLOT( sendMessage(QListBoxItem *) ) );
	connect(userbox, SIGNAL(rightButtonClicked(QListBoxItem *, const QPoint &)),
		this, SLOT(listPopupMenu(QListBoxItem *)));

	statuslabeltxt = new MyLabel (this);
	statuslabeltxt->setText(i18n("Offline"));
	statuslabeltxt->setFrameStyle(QFrame::WinPanel|QFrame::Sunken);
	statuslabeltxt->setFont(QFont("Verdana", 9));
	statuslabeltxt->setMinimumWidth(this->width()-45);

	/* a bit darker than the rest */
	statuslabeltxt->setPaletteBackgroundColor(QColor(
		qRed(statuslabeltxt->paletteBackgroundColor().rgb()) - 20,
		qGreen(statuslabeltxt->paletteBackgroundColor().rgb()) - 20,
		qBlue(statuslabeltxt->paletteBackgroundColor().rgb()) - 20));

	statuslabel = new MyLabel (this);
	statuslabel->setPixmap(QPixmap((const char**)gg_inact_xpm) );

	/* guess what */
	createMenu();
	createStatusPopupMenu();

	dockppm->insertSeparator();

	KIconLoader *loader = KGlobal::iconLoader();

	dockppm->insertItem(loader->loadIcon("exit",KIcon::Small), i18n("Exit"), 9);
	connect(dockppm, SIGNAL(activated(int)), dw, SLOT(dockletChange(int)));

	QGridLayout * grid = new QGridLayout (this,3,3);
	grid->addMultiCellWidget(mmb,0,0,0,2);
	grid->addMultiCellWidget(userbox, 1, 1, 0, 2);
	grid->addWidget(statuslabeltxt,2,0, Qt::AlignLeft);
	grid->addWidget(statuslabel, 2, 2, Qt::AlignCenter);
	grid->setColStretch(0, 3);
	grid->setColStretch(1, 1);
	grid->setColStretch(2, 1);

	dccsock = NULL;
	/* dirty workaround for multiple showEvents */
	commencing_startup = true;

	/* pokaz okno jesli RunDocked jest wylaczone lub dock wylaczone */
	if((!config.rundocked)||(!config.dock))
		show();
}

void Kadu::removeUser(QString &username, bool permanently = false) {
	int i = 0;

	UserBox::all_removeUser(username);	
	UserBox::all_refresh();
	
	UserListElement user = userlist.byAltNick(username);
	gg_remove_notify(&sess, user.uin);
    	userlist.removeUser(user.uin);

	switch (QMessageBox::information(kadu, "Kadu", i18n("Save current userlist to file?"), i18n("Yes"), i18n("No"), QString::null, 0, 1) ) {
		case 0: // Yes?
			userlist.writeToFile();
			break;
		case 1:
			break;
		}
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
	    		dw->setType((char **)gg_inact_xpm);
	    		return;
	    		}

	if (blinkOn) {
		statuslabel->setPixmap(QPixmap((const char**)gg_inact_xpm) );
		dw->setType((char **)gg_inact_xpm);
		blinkOn = false;
		}
	else {
		i = statusGGToStatusNr(loginparams.status & (~GG_STATUS_FRIENDS_MASK));
		statuslabel->setPixmap(QPixmap((const char **)gg_xpm[i]));
		dw->setType((char **)gg_xpm[i]);
		blinkOn = true;
		}

	blinktimer->start(1000, TRUE);
}

/* dcc initials */
void Kadu::prepareDcc(void) {

	dccsock = gg_dcc_socket_create(config.uin, 0);
	
	if (!dccsock) {
		perror("DCC");
		gg_dcc_free(dccsock);
		QMessageBox::warning(kadu, "", i18n("Couldn't create DCC socket.\nDirect connections disabled.") );		
		return;
		}

	struct in_addr in;
	in.s_addr = getMyIP();
	config.dccip = inet_ntoa(in);
	fprintf(stderr, "KK My IP address: %s\n", inet_ntoa(in));

	gg_dcc_ip = inet_addr(config.dccip);
	gg_dcc_port = dccsock->port;
    
	fprintf(stderr, "KK Kadu::prepareDcc() DCC_IP=%s DCC_PORT=%d\n", config.dccip, dccsock->port);
    
	dccsnr = new QSocketNotifier(dccsock->fd, QSocketNotifier::Read, kadu);
	QObject::connect(dccsnr, SIGNAL(activated(int)), kadu, SLOT(dccReceived()));

	dccsnw = new QSocketNotifier(dccsock->fd, QSocketNotifier::Write, kadu);
	QObject::connect(dccsnw, SIGNAL(activated(int)), kadu, SLOT(dccSent()));
}

/*bool Kadu::userInActiveGroup(uin_t uin) {
  int i, j;

  if (grouplist.size() == 0)
    return true;

  for (i = 0; i < grouplist.size(); i++)
    if (grouplist[i].number == activegrpno)
       break;

  for (j = 0; j < userlist.size(); j++)
    if (userlist[j].uin == uin)
       break;

  if (userlist[j].group == NULL && activegrpno == 600)
    return true;
  else if (userlist[j].group == NULL && activegrpno != 600)
    return false;
  else if (strcmp(userlist[j].group, grouplist[i].name) == 0)
     return true;
  else if (activegrpno == 600)
     return true;
  else
    return false;

}*/

// code for addUser has been moved from adduser.cpp
// for sharing with search.cpp
void Kadu::addUser(const QString& FirstName, const QString& LastName,
	const QString& NickName, const QString& AltNick,
	const QString& Mobile, const QString& Uin, const int Status,
	const QString& Group, const QString& Description, const bool Foreign)
{
	userlist.addUser(FirstName, LastName, NickName, AltNick, Mobile, Uin, Status, Group, Description, Foreign);
	userlist.writeToFile();

	userbox->addUin(Uin.toInt());
	UserBox::all_refresh();

	gg_add_notify(&sess, Uin.toInt());
};


/* cancel autoaway, we're alive */
void Kadu::enterEvent (QEvent * e) {
	autoaway->stop();
	autoaway->start(config.autoawaytime * 1000, TRUE);
	if (autoawayed && socket_active && userlist_sent) {
		fprintf(stderr, "KK Kadu::enterEvent(): auto away cancelled\n");
		autoawayed = false;
		switch (beforeAutoAway) {
			case GG_STATUS_AVAIL:
			case GG_STATUS_AVAIL_DESCR:
			case GG_STATUS_INVISIBLE:
			case GG_STATUS_INVISIBLE_DESCR:
				i_am_busy = false;
				break;
			}
		setStatus(beforeAutoAway);
		}
	QWidget::enterEvent(e);
}

/* invoked every config.autoawaytime of idleness seconds */
void Kadu::autoAway(void) {
	if (!config.autoaway || i_wanna_be_invisible || i_am_busy || !socket_active || !userlist_sent)
		return;
	beforeAutoAway = getActualStatus();
	fprintf(stderr, "KK Kadu::autoAway(): going auto away\n");
	setStatus(GG_STATUS_BUSY);
	i_am_busy = true;
	autoawayed = true;
	autoaway->start(config.autoawaytime * 1000, TRUE);
}

int Kadu::openChat(UinsList senders) {
	int i,j;
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
	char buf1[255];
	uin_t uin;
	SearchDialog *sd;
	UinsList uins;
	UserListElement user;
	QString tmp;
		
	switch (command) {
		case KADU_CMD_SEND_MESSAGE:
			Message *msg;
			msg = new Message(userbox->currentText());
			msg->init();
			msg->show();
			break;
		case KADU_CMD_OPEN_CHAT:
			for (i = 0; i < userbox->count(); i++)
				if (userbox->isSelected(i))
					uins.append(userlist.byAltNick(userbox->text(i)).uin);
			openChat(uins);
			break;
		case KADU_CMD_REMOVE_USER:
			tmp = userbox->currentText();
			removeUser(tmp);
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
			ui = new UserInfo("user info", 0, userlist.byAltNick(userbox->currentText()).uin);
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
			sms = new Sms(0, userbox->currentText(), 0);
			sms->show();
			break;
		case KADU_CMD_REMIND_PASSWORD:
			remindPassword();
			break;
		case KADU_CMD_REGISTER_USER:
			Register *reg;
			reg = new Register;
			reg->show();
			break;
		case KADU_CMD_QUIT:
			close_permitted = true;
			disconnectNetwork();
			close(true);
			break;
		case KADU_CMD_SEARCH_USER:
			sd = new SearchDialog(0, i18n("User info"), userlist.byAltNick(userbox->currentText()).uin);
			sd->init();
			sd->show();
			sd->doSearch();
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
				acks[i].seq = gg_dcc_request(&sess, user.uin);
				acks[i].type = 0;
				acks[i].ptr = NULL;
				}
			break;
		case KADU_CMD_PERSONAL_INFO:
			PersonalInfoDialog *pid;
			pid = new PersonalInfoDialog();
			pid->show();
			break;
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
	pm->insertItem(loader->loadIcon("filesave", KIcon::Small), i18n("Send file"), KADU_CMD_SEND_FILE);
	if (user.status == GG_STATUS_AVAIL || user.status == GG_STATUS_AVAIL_DESCR ||
		user.status == GG_STATUS_BUSY || user.status == GG_STATUS_BUSY_DESCR)
		pm->setItemEnabled(KADU_CMD_SEND_FILE, true);
	else
		pm->setItemEnabled(KADU_CMD_SEND_FILE, false);
    
	pm->insertSeparator();
	
	pm->insertItem(loader->loadIcon("remove", KIcon::Small), i18n("Remove from userlist"), KADU_CMD_REMOVE_USER);
	pm->insertItem(loader->loadIcon("eraser", KIcon::Small), i18n("Clear history"), KADU_CMD_DELETE_HISTORY);
	QPixmap history;
	history = loader->loadIcon("history", KIcon::Small);
	pm->insertItem(history, i18n("View history"), KADU_CMD_SHOW_HISTORY);
	pm->insertItem(loader->loadIcon("identity", KIcon::Small), i18n("View/edit user info"), KADU_CMD_USERINFO);
	pm->insertItem(loader->loadIcon("viewmag", KIcon::Small), i18n("Lookup in directory"), KADU_CMD_SEARCH_USER);
	pm->insertSeparator();
	pm->insertItem(i18n("About..."), KADU_CMD_ABOUT);

	connect(pm, SIGNAL(activated(int)), this, SLOT(commandParser(int)));	
	pm->exec(QCursor::pos());    	
}

/* if something's pending, open it, if not, open new message */
void Kadu::sendMessage(QListBoxItem *item) {
	QString tmp;
	int i,j;
	bool stop = false;
	rMessage *rmsg;
	Message *msg;
	UinsList uins;
	PendingMsgs::Element elem;
	
	for (i = 0; i < pending.count(); i++) {
		elem = pending[i];
		if (!uins.count() || elem.uins.equals(uins))
			if (elem.msgclass == GG_CLASS_CHAT) {
				if (!uins.count())
					uins = elem.uins;
				for (j = 0; j < elem.uins.count(); j++)
					if (!userlist.containsUin(elem.uins[j])) {
						tmp = QString::number(pending[i].uins[j]);
						addUser("", "", tmp, tmp, "", tmp, GG_STATUS_NOT_AVAIL, "", "", true);
						}
				j = openChat(elem.uins);
				chats[j].ptr->checkPresence(elem.uins, elem.msg, elem.time);	    
				deletePendingMessage(i);
				fprintf(stderr, "KK Kadu::sendMessage(): j=%d\n", j);
				i--;
				stop = true;
				}
	    		else {
				if (!stop)
		    			{
  		    			rmsg = new rMessage(item->text(),
						elem.msgclass, elem.uins, elem.msg);
					deletePendingMessage(i);
					UserBox::all_refresh();
					rmsg->init();
					rmsg->show();
					}
				return;
				}
		}

	if (stop) {
		UserBox::all_refresh();
		return;
		}

	uins.clear();
	for (i = 0; i < userbox->count(); i++)
		if (userbox->isSelected(i))
			uins.append(userlist.byAltNick(userbox->text(i)).uin);
	if (!uins.count())
		uins.append(userlist.byAltNick(item->text()).uin);

	if (uins.count() > 1 || (GetStatusFromUserlist(uins[0]) != GG_STATUS_NOT_AVAIL
		&& GetStatusFromUserlist(uins[0]) != GG_STATUS_NOT_AVAIL_DESCR))
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
			cd->exec();
			autohammer = true;
			setStatus(GG_STATUS_AVAIL_DESCR);
			break;
		case 2:
			autohammer = true;
			setStatus(GG_STATUS_BUSY);
			break;
		case 3:
			cd = new ChooseDescription(3);
			cd->exec();
			autohammer = true;
			setStatus(GG_STATUS_BUSY_DESCR);
			break;
		case 4:
			autohammer = true;
			setStatus(GG_STATUS_INVISIBLE);
			break;
		case 5:
			cd = new ChooseDescription(5);
			cd->exec();
			autohammer = true;
			setStatus(GG_STATUS_INVISIBLE_DESCR);
			break;
		case 6:
			autohammer = false;
			disconnectNetwork();
			setCurrentStatus(GG_STATUS_NOT_AVAIL);
			break;
		case 7:
			cd = new ChooseDescription(7);
			cd->exec();
			setStatus(GG_STATUS_NOT_AVAIL_DESCR);
			statusppm->setItemEnabled(7, false);
			autohammer = false;
			disconnectNetwork();
			break;	    
		case 8:
			statusppm->setItemChecked(8, !statusppm->isItemChecked(8));
			dockppm->setItemChecked(8, !dockppm->isItemChecked(8));	    
			config.privatestatus = statusppm->isItemChecked(8);
			if (!statusppm->isItemChecked(6) && !statusppm->isItemChecked(7))
				setStatus(sess.status & (~GG_STATUS_FRIENDS_MASK));
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
	if (pending.pendingMsgs())
		dw->setType((char **)gg_msg_xpm);
	else
		dw->setType((char **)gg_xpm[statusnr]);
}

void Kadu::slotShowStatusMenu() {
	QPoint point = statuslabeltxt->mapToGlobal(QPoint(0, 0));
	point.setY(point.y()-statusppm->height());
	statusppm->popup(point);
}

void Kadu::setStatus(int status) {

	fprintf(stderr, "KK Kadu::setStatus()\n");
    
	bool with_description;
    
	with_description = ifStatusWithDescription(status);
	status &= ~GG_STATUS_FRIENDS_MASK;
    	
	i_am_busy = i_wanna_be_invisible = false;
	if (status == GG_STATUS_BUSY || status == GG_STATUS_BUSY_DESCR)
		i_am_busy = true;
	else
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
				gg_change_status_descr(&sess, status, (const char *)descr);
			else
				gg_change_status_descr(&sess,
					status | (GG_STATUS_FRIENDS_MASK * config.privatestatus), (const char *)descr);
			delete descr;
			}
		else
			gg_change_status(&sess, status | (GG_STATUS_FRIENDS_MASK * config.privatestatus));
		if (sess.check & GG_CHECK_WRITE)
			kadusnw->setEnabled(true);
	
		setCurrentStatus(status);
	    		
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

	socket_active = TRUE;
	last_ping = time(NULL);
	loginparams.status = status | (GG_STATUS_FRIENDS_MASK * config.privatestatus);
	loginparams.password = config.password;
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
	if (inet_addr(config.server) != INADDR_NONE) {
		loginparams.server_addr = inet_addr(config.server);
		loginparams.server_port = 8074;
		}
	else {
		loginparams.server_addr = 0;
		loginparams.server_port = 0;
		}
	sess = *gg_login(&loginparams);

	kadusnw = new QSocketNotifier(sess.fd, QSocketNotifier::Write, this); 
	QObject::connect(kadusnw, SIGNAL(activated(int)), kadu, SLOT(dataSent()));

	kadusnr = new QSocketNotifier(sess.fd, QSocketNotifier::Read, this); 
	QObject::connect(kadusnr, SIGNAL(activated(int)), kadu, SLOT(dataReceived()));    
}

void Kadu::checkConnection(void) {
	// Since it doesn't work anymore...
	readevent->start(10000, TRUE);

	return;	

/*	if ((last_read_event < time(NULL) - 90) && timeout_connected && socket_active) {
		timeout_connected = false;
		int i = 0;
		QPixmap qp_inact((const char **)gg_inact_xpm);
		while (userlist[i].uin != 0) {
			mylist->changeItem(qp_inact, userlist[i].nickname, i);
			i++;
			}
		char error[1023];
		QMessageBox * msgb;

		snprintf(error, sizeof(error), "nConnect(): Timeout after 90 seconds.\nDisconnecting.\n");
		fprintf(stderr, error);
		disconnectNetwork();			
		socket_active = false;
		mycombo->setCurrentItem(3);
		last_read_event = time(NULL);
		msgb = new QMessageBox(this);
		msgb->warning(kadu, "Connect error", error );
	} */
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
	if (sess.check && GG_CHECK_READ)
		eventHandler(GG_CHECK_READ);
}

void Kadu::dataSent(void) {
	fprintf(stderr, "KK Kadu::dataSent()\n");
	kadusnw->setEnabled(false);
	if (sess.check & GG_CHECK_WRITE)
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
	if (!(e = gg_watch_fd(&sess))) {
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

	switch (sess.state) {
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

	if (sess.check == GG_CHECK_READ) {
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

	if (e->type == GG_EVENT_NOTIFY_DESCR) {
		eventGotUserlistWithDescription(e);
		UserBox::all_refresh();
		}

	if (e->type == GG_EVENT_NOTIFY) {
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

		pingtimer = new QTimer;
		QObject::connect(pingtimer, SIGNAL(timeout()), kadu, SLOT(pingNetwork()));
		pingtimer->start(60000, TRUE);

		readevent = new QTimer;
		QObject::connect(readevent, SIGNAL(timeout()), kadu, SLOT(checkConnection()));    
		readevent->start(10000, TRUE);
		}

	if (e->type == GG_EVENT_CONN_FAILED) {
		char error[512];
		snprintf(error, sizeof(error), "KK Kadu::eventHandler(): Unable to connect, the following error has occured:\n%s\nKK Kadu::nConnect(): Keep trying to connect?\n", strerror(errno));
		disconnectNetwork();	
		setCurrentStatus(GG_STATUS_NOT_AVAIL);
		fprintf(stderr, "KK Kadu::eventHandler(): Connection failed\n");
		fprintf(stderr, error);

		if (autohammer)
			setStatus(loginparams.status & (~GG_STATUS_FRIENDS_MASK));
		}

	if (socket_active) {
		if (sess.state == GG_STATE_IDLE && userlist_sent) {
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
			if (sess.check & GG_CHECK_WRITE)
				kadusnw->setEnabled(true);
		}

	gg_free_event(e);

	calls--;
}

void Kadu::pingNetwork(void) {
	fprintf(stderr, "KK Kadu::pingNetwork()\n");
	gg_ping(&sess);
	// Fuck you, since you don't reply anyway
	if (socket_active && i_wanna_be_invisible)
		gg_change_status(&sess, GG_STATUS_INVISIBLE | (GG_STATUS_FRIENDS_MASK * config.privatestatus));
	pingtimer->start(60000, TRUE);
}

void Kadu::disconnectNetwork() {
	int i;

	doBlink = false;
	fprintf(stderr, "KK Kadu::disconnectNetwork(): calling offline routines\n");

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

	i_am_busy = false;
	disconnect_planned = true;
	gg_logoff(&sess);
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
	dw->setType((char **)gg_inact_xpm);
	setIcon(QPixmap((const char**)gg_inact_xpm));

}

DockWidget::DockWidget(QWidget *parent, const char *name ) : KSystemTray( parent, name )
{
	if (!config.dock)
		return;

	setPixmap( QPixmap((const char**)gg_inact_xpm) );
	QToolTip::add(this, i18n("Left click - hide/show window\nMiddle click - next message"));
}

void DockWidget::setType(char **gg_xpm) {
	if (!config.dock)
		return;
	
	setPixmap(QPixmap((const char**)gg_xpm));
}

void DockWidget::dockletChange(int id)
{
	if (id < 9)
		kadu->slotHandleState(id);
	else {
		kadu->close_permitted = true;
		kadu->disconnectNetwork();
		a->quit();
		}
}

void DockWidget::mousePressEvent(QMouseEvent * e) {
	bool message = false;
	int i,j;
	QString tmp;
	PendingMsgs::Element elem;
	
	if (!config.dock)
		return;

	if (e->button() == MidButton) {
		bool stop = false;
	
		UinsList uins;
		for (i = 0; i < pending.count(); i++) {
			elem = pending[i];
			if (!uins.count() || elem.uins.equals(uins))
				if (elem.msgclass == GG_CLASS_CHAT) {
					if (!uins.count())
						uins = elem.uins;
					for (j = 0; j < elem.uins.count(); j++)
						if (!userlist.containsUin(elem.uins[j])) {
							tmp = QString::number(elem.uins[j]);
							kadu->addUser("", "", tmp, tmp, "", tmp, GG_STATUS_NOT_AVAIL, "", "", true);
							}
					j = kadu->openChat(elem.uins);
					chats[j].ptr->checkPresence(elem.uins,
						elem.msg, elem.time);	    
					deltePendingMessage(i);
					stop = true;
					}		
				else {
					if (!stop) {
						rMessage *rmsg;
						rmsg = new rMessage(userlist.byUin(elem.uins[0]).altnick,
							elem.msgclass, elem.uins, elem.msg);
						deletePendingMessage(i);
						UserBox::all_refresh();
						rmsg->init();
						rmsg->show();
						}
					return;
					}
			}
		if (stop) {
	    		UserBox::all_refresh();
			return;
			}
		}

	if (e->button() == LeftButton) 
		{
		switch (kadu->isVisible()) {
			case 0:
				kadu->show();
				kadu->setFocus();
				break;
			case 1:
				kadu->hide();
				break;
			}
		return;
		}

	if (e->button() == RightButton) {
		dockppm->exec(QCursor::pos());
		return;
		}
}

void Kadu::cleanUp(void) {
	// may be of use
	if (sess.password !=NULL)
		free(sess.password);
	writeIgnored(NULL);
}

Kadu::~Kadu(void) {
}

void Kadu::createMenu() {

	KIconLoader *loader = KGlobal::iconLoader();

	QPopupMenu *ppm = new QPopupMenu(this);
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

/*
for (int k = 0; k < grouplist.size(); k++) {
    grpmenu->insertItem(__c2q(grouplist[k].name), grouplist[k].number);
    }

ppm->insertItem(i18n("Groups"), grpmenu);

QObject::connect(grpmenu, SIGNAL( activated(int) ), this, SLOT( changeGroup(int) ));

ppm->insertSeparator();
*/

	ppm->insertItem(i18n("Remind &password"), KADU_CMD_REMIND_PASSWORD);
	QPixmap new__user((const char **)new_user);
	ppm->insertItem(new__user,i18n("Register &new user"), KADU_CMD_REGISTER_USER);
	ppm->insertItem(i18n("Personal information"), KADU_CMD_PERSONAL_INFO);
	ppm->insertSeparator();
	QPixmap find;
	find = loader->loadIcon("viewmag", KIcon::Small);
	ppm->insertItem(find, i18n("&Search for users"), KADU_CMD_SEARCH);
	ppm->insertItem(i18n("I&mport userlist"), KADU_CMD_IMPORT_USERLIST);
	ppm->insertItem(i18n("E&xport userlist"), KADU_CMD_EXPORT_USERLIST);
	ppm->insertItem(QPixmap((const char **)gg_act_xpm),i18n("&Add user"), KADU_CMD_ADD_USER);
	ppm->insertSeparator();
	ppm->insertItem(i18n("A&bout..."), KADU_CMD_ABOUT);
	ppm->insertSeparator();
	ppm->insertItem(i18n("&Hide Kadu"), KADU_CMD_HIDE);
	ppm->insertItem(loader->loadIcon("exit", KIcon::Small), i18n("&Exit Kadu"), KADU_CMD_QUIT);

	mmb = new KMenuBar(this);
	mmb->insertItem(i18n("&Kadu"), ppm);
	mmb->polish();

	connect(ppm, SIGNAL(activated(int)), this, SLOT(commandParser(int)));
}

void Kadu::createStatusPopupMenu() {

	KIconLoader *loader = KGlobal::iconLoader();
	QPixmap pixmap;
	QIconSet icon;

	statusppm = new QPopupMenu(this);
	dockppm = new KPopupMenu(this);

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

	statusppm->popup(QPoint(0,0));
	statusppm->close();
	connect(statusppm, SIGNAL(activated(int)), this, SLOT(slotHandleState(int)));
}

void Kadu::closeEvent(QCloseEvent *e) {
	if (!close_permitted && config.dock) {
		e->ignore();
		hide();
		}
	else {
		saveKaduConfig();
		fprintf(stderr,"KK closeEvent(): Graceful shutdown...\n");
		e->accept();
		}
}

void MyLabel::mousePressEvent (QMouseEvent * e) {
	if (e->button() == Qt::LeftButton)
		kadu->slotShowStatusMenu();
}

#include "kadu.moc"
