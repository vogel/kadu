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
MyListBox * mylist;
QPopupMenu *statusppm;
KPopupMenu *dockppm;
QLabel * statuslabel;
QLabel * statuslabeltxt;
DockWidget * dw;
QPopupMenu * grpmenu;

QArray<struct userlist> userlist(0);
QArray<struct pending> pending(0);
QArray<struct chats> chats(0);
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

void removeUser(QString &username, bool permanently = false) {
    int i = 0;

    while (i < mylist->count() && mylist->item(i)->text() != username)
	i++;

    if (i == mylist->count()) {
	fprintf(stderr, "KK removeUser(): FATAL: QListBox and userlist[] out of sync! Returning!\n");
	return;
	}

    gg_remove_notify(&sess, userlist[i].uin);
    free(userlist[i].first_name);
    userlist[i].first_name = NULL;
    free(userlist[i].last_name);
    userlist[i].last_name = NULL;
    free(userlist[i].nickname);
    userlist[i].nickname = NULL;
    free(userlist[i].comment);
    userlist[i].comment = NULL;
    free(userlist[i].mobile);
    userlist[i].mobile = NULL;
    free(userlist[i].group);	
    userlist[i].group = NULL;
    userlist[i].anonymous = false;
    if (userlist[i].description) {
	delete userlist[i].description;
  	userlist[i].description = NULL;
	}

    mylist->removeItem(i);

    int k;
    for (k = i; k < userlist.size() - 1; k++) {
	userlist[k] = userlist[k+1];
	}

    userlist.resize(userlist.size() - 1);
    kadu->syncUserlist();

    fprintf(stderr, "KK removeUser(): new userlist status\n");
    for (int i = 0; i < mylist->count(); i++) {
        fprintf(stderr, "KK Index %d / Nickname %s / Display %s / Uin %d / Status %d\n", i, userlist[i].nickname,
        mylist->item(i)->text().latin1(), userlist[i].uin, userlist[i].status);
        }
    fprintf(stderr, "KK removeUser(): if you spot any inconsistency, report!\n");

    switch (QMessageBox::information(kadu, "Kadu", i18n("Save current userlist to file?"), i18n("Yes"), i18n("No"), QString::null, 0, 1) ) {
	case 0: // Yes?
	    writeUserlist(NULL);
	    break;
	case 1:
	    return;
	}
}

int writeUserlist(char *filename = NULL) {
    char *tmp;
    int i;
    
    if (!(tmp = preparePath("")))
	return -1;
    mkdir(tmp, 0700);

    if (!filename) {
	if (!(filename = preparePath("userlist")))
	    return -1;
	}

    QFile f(filename);

    if (!f.open(IO_WriteOnly)) {
	std::cout << "KK write_userlist(): Error opening file :(" << std::endl;
	return -2;
	}

/*  someone tell me how. */
//	fchmod(fileno(f), 0600);

    QString jakisstr;
    for (i = 0; i < userlist.size(); i++) {
	jakisstr.truncate(0);

        jakisstr.append(__c2q(userlist[i].first_name));
        jakisstr.append(QString(";"));
	jakisstr.append(__c2q(userlist[i].last_name));
    	jakisstr.append(QString(";"));
	jakisstr.append(__c2q(userlist[i].comment));
	jakisstr.append(QString(";"));
	jakisstr.append(__c2q(userlist[i].nickname));
	jakisstr.append(QString(";"));
	jakisstr.append(__c2q(userlist[i].mobile));
	jakisstr.append(QString(";"));
	jakisstr.append(__c2q(userlist[i].group));
	jakisstr.append(QString(";"));
	jakisstr.append(QString::number(userlist[i].uin));
	jakisstr.append(QString("\r\n"));

	std::cout << jakisstr;

	f.writeBlock(jakisstr.local8Bit(), jakisstr.length());
	}	    
    f.close();

    return 0;
}

char *preparePath(char *filename)
{
    static char path[1023];
    char *home = getenv("HOME");
    struct passwd *pw;

    if (!home) {
	if (!(pw = getpwuid(getuid())))
	    return NULL;
	home = pw->pw_dir;
	}
    snprintf(path, sizeof(path), "%s/.gg/%s", home, filename);

    return path;
}

void deletePendingMessage(int nr) {
    int j;
    
    delete pending[nr].uins;
    if (nr != pending.size() - 1) {
	for (j = nr; j < pending.size() - 1; j++) {
	    pending[j].uins->duplicate(*pending[j+1].uins);
	    pending[j].msg = pending[j+1].msg;
	    pending[j].msgclass = pending[j+1].msgclass;
	    pending[j].time = pending[j+1].time;
	    }
	}
    else {
	delete pending[nr].msg;
	pending[nr].time = 0;
	pending[nr].msgclass = 0;
	}
    pending.resize(pending.size()-1);

//    kadu->sortUsers();
//    kadu->syncUserlist();

    if (ifPendingMessages())
        dw->setType((char **)gg_msg_xpm);
    else	
        dw->setType((char **)gg_xpm[statusGGToStatusNr(getActualStatus() & (~GG_STATUS_FRIENDS_MASK))]);
}

uin_t UserToUin(const QString *str) {
    int i;
    i = 0;
    while (i < userlist.size() && str->compare(__c2q(userlist[i].nickname)))
	i++;

    if (i == userlist.size())
	return 0;
    else
	return userlist[i].uin;
}
  
QString UserToMobile(const QString *str) {
    int i;
    i = 0;
    while (i < userlist.size() && str->compare(__c2q(userlist[i].nickname)))
	i++;
	
    if (i == userlist.size())
	return "";
    else
	return userlist[i].mobile;
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

/* sends the userlist. ripped off EKG, actually, but works */
void send_userlist() {
    uin_t *uins;
    int i;

    uins = (uin_t *) malloc(userlist.size() * sizeof(uin_t));
	
    for (i = 0; i < userlist.size(); i++)
	uins[i] = userlist[i].uin;

    /* we were popping up sometimes, so let's keep the server informed */
    if (i_wanna_be_invisible)
	gg_change_status(&sess, GG_STATUS_INVISIBLE);

    gg_notify(&sess, uins, userlist_count);
    fprintf(stderr, "KK send_userlist(): Userlist sent\n");

    free(uins);
}

/* takes the UIN number, returns a random string ;) */
QString UinToUser(uin_t uin) {
    if (uin == config.uin)
	return "System";

    int i;
    for (i = 0; i < userlist.size(); i++)
	if (userlist[i].uin == uin)
	    return __c2q(userlist[i].nickname);
 
    /* the guy was not on our list. let's add him */

    userlist.resize(userlist.size() + 1);
    i = userlist.size() - 1;

    userlist[i].uin = uin;
    char _nick[512];
    snprintf(_nick, sizeof(_nick), "(%d)", uin);
    userlist[i].nickname = strdup(_nick);
    userlist[i].first_name = strdup(_nick);
    userlist[i].last_name = strdup(_nick);
    userlist[i].comment = strdup(_nick);
    userlist[i].mobile = NULL;
    userlist[i].group = NULL;
    userlist[i].description = NULL;
    userlist[i].status = GG_STATUS_NOT_AVAIL;
    userlist[i].anonymous = true;
    kadu->syncUserlist();
    gg_notify(&sess, &uin, 1);
    
    return __c2q(userlist[i].nickname);
}

bool ifPendingMessages(int uin) {
    bool pendings = false;
    int i;
    
    switch (uin) {
	case -1:
	    pendings = pending.size();
	    break;
	default:
	    for (i = 0; i < pending.size(); i++)
    		if ((*pending[i].uins)[0] == uin)
		    pendings = true;
	}
    return pendings;
}

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

void confirmHistoryDeletion (const char *user) {
    char buf1[1023];
    QString s;
    	
    switch(QMessageBox::information( kadu, "Kadu",i18n("Clear history?"), i18n("Yes"), i18n("No"), QString::null, 1, 1)) {
	case 1: // Yes?
	    s = __c2q(user);
	    snprintf(buf1, sizeof(buf1), "%s/.gg/history/%d",getenv("HOME"),UserToUin(&s));
	    fprintf(stderr, "KK History(): unlinking %s\n", buf1);
	    unlink(buf1);
	    break;
	case 0: // Nope?
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

void UinToUserlistEntry (uin_t uin, int new_status) {
    int i,j;
    for (i = 0; i < userlist.size(), userlist[i].nickname != NULL; i++)
        if (userlist[i].uin == uin) {
            userlist[i].status = new_status;
            j = 0;
	    while (j < chats.size() && (*chats[j].uins)[0] != uin)
		j++;
	    if (j < chats.size() && chats[j].ptr)
		chats[j].ptr->setTitle();
	    return;
            }
}

int GetStatusFromUserlist (unsigned int uin) {
    for (int i = 0; i < userlist.size(); i++)
        if (userlist[i].uin == uin)
            return userlist[i].status;
    return -1;
}
  
/* a monstrous constructor so Kadu would take longer to start up */
Kadu::Kadu(QWidget *parent, const char *name) : QWidget (parent, name)
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
    /* empty buffers, g < 1 if we're clean & lucky */
    for (g = 0; g < userlist.size(); g++)
	userlist[g].description = NULL;

//    for (g = 0; g < pending.size(); g++)
//	pending[g].uin = 0;

    for (g = 0; g < acks.size(); g++)
	acks[g].seq = 0;

//    for (g = 0; g < chats.size(); g++)
//	chats[g].uin = 0;

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
readUserlist();
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
	switch( QMessageBox::information( kadu, "Kadu",
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
mylist = new MyListBox(this);
mylist->setPaletteBackgroundColor(QColor(config.colors.userboxBgR,config.colors.userboxBgG,config.colors.userboxBgB));
mylist->setPaletteForegroundColor(QColor(config.colors.userboxFgR,config.colors.userboxFgG,config.colors.userboxFgB));
mylist->QListBox::setFont(QFont(config.colors.userboxFont, config.colors.userboxFontSize));

    QPixmap qp_inact((const char **)gg_inact_xpm);
    for (int i = 0; i < userlist.count(); i++)
	mylist->insertItem(qp_inact, __c2q(userlist[i].nickname));
	
/* start auto away. yes, even if it's disabled. this way enabling it will work at run-time */
autoaway = new QTimer(this);
autoaway->start(config.autoawaytime * 1000, TRUE);
connect(autoaway, SIGNAL( timeout() ), this, SLOT( autoAway() ));

connect(mylist, SIGNAL( doubleClicked(QListBoxItem *)), this, SLOT( sendMessage(QListBoxItem *) ) );
connect(mylist, SIGNAL( rightButtonClicked(QListBoxItem *, const QPoint &)),
	this, SLOT( listPopupMenu(QListBoxItem *)));

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
grid->addMultiCellWidget(mylist, 1, 1, 0, 2);
grid->addWidget(statuslabeltxt,2,0, Qt::AlignLeft);
grid->addWidget(statuslabel, 2, 2, Qt::AlignCenter);
grid->setColStretch(0, 3);
grid->setColStretch(1, 1);
grid->setColStretch(2, 1);

dccsock = NULL;
/* dirty workaround for multiple showEvents */
commencing_startup = true;

//show();

/* set an offline icon */
//setIcon((const char **)gg_inact_xpm);
//repaint();

}

/* reimplemented, to auto login */
void Kadu::showEvent (QShowEvent * e) {
    QWidget::showEvent(e);

/*    if (commencing_startup)
	if (config.defaultstatus != GG_STATUS_NOT_AVAIL && config.defaultstatus != GG_STATUS_NOT_AVAIL_DESCR) {
	    autohammer = true;
	    own_description = config.defaultdescription;
	    setStatus(config.defaultstatus);
	    commencing_startup = false;
	    }*/
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

bool Kadu::userInActiveGroup(uin_t uin) {
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

}

/* Userlist code is very dirty, so to avoid double messages we sync the
   userlist each time. Sorry, poor CPU folks! */

void Kadu::syncUserlist() {
//KIconLoader *loader = KGlobal::iconLoader();
    bool hasPendingMesg = false;
    int i,j;

    mylist->clear();
    for (i = 0; i < userlist.size(); i++) {
  /*
  if (!userInActiveGroup(userlist[i].uin))
    continue;
  */
	hasPendingMesg = false;
	if (ifPendingMessages(userlist[i].uin)) {
	    mylist->insertItem(QPixmap((const char **)gg_msg_xpm), __c2q(userlist[i].nickname));
	    hasPendingMesg = true;			
	    continue;
	    }

	if (hasPendingMesg)
	    continue;
	else {
	    switch (userlist[i].status) {
		case GG_STATUS_AVAIL:
		    mylist->insertItem(QPixmap((const char **)gg_act_xpm), __c2q(userlist[i].nickname));			
		    break;
		case GG_STATUS_AVAIL_DESCR:
		    mylist->insertItem(QPixmap((const char **)gg_actdescr_xpm), __c2q(userlist[i].nickname));			
		    break;
		case GG_STATUS_NOT_AVAIL:
		    mylist->insertItem(QPixmap((const char **)gg_inact_xpm), __c2q(userlist[i].nickname));			
		    break;
		case GG_STATUS_BUSY:
		    mylist->insertItem(QPixmap((const char **)gg_busy_xpm), __c2q(userlist[i].nickname));			
		    break;
		case GG_STATUS_BUSY_DESCR:
		    mylist->insertItem(QPixmap((const char **)gg_busydescr_xpm), __c2q(userlist[i].nickname));			
		    break;
		case GG_STATUS_NOT_AVAIL_DESCR:
		    mylist->insertItem(QPixmap((const char **)gg_inactdescr_xpm), __c2q(userlist[i].nickname));			
		    break;
		case GG_STATUS_INVISIBLE_DESCR:
		    mylist->insertItem(QPixmap((const char **)gg_invidescr_xpm), __c2q(userlist[i].nickname));			
    		    break;
		case GG_STATUS_INVISIBLE2:
		    mylist->insertItem(QPixmap((const char **)gg_invi_xpm), __c2q(userlist[i].nickname));			
		    break;
		default:
		    mylist->insertItem(QPixmap((const char **)gg_inact_xpm), __c2q(userlist[i].nickname));			
		}
	    }
	}	

    if (ifPendingMessages())
	dw->setType((char **)gg_msg_xpm);
}

// code for addUser has been moved from adduser.cpp
// for sharing with search.cpp
void Kadu::addUser(const QString& FirstName,const QString& LastName,
	const QString& NickName,const QString& AltNick,
	const QString& Mobile,const QString& Uin,const int Status,
	const QString& Group,const QString& Description)
{
	int i=userlist.size();
	userlist.resize(i + 1);

	userlist[i].first_name = strdup(FirstName.local8Bit());
	userlist[i].last_name = strdup(LastName.local8Bit());
	userlist[i].nickname = strdup(NickName.local8Bit());
	userlist[i].comment = strdup(AltNick.local8Bit());
	userlist[i].mobile = strdup(Mobile.local8Bit());
	userlist[i].uin = atoi(Uin.local8Bit());
	userlist[i].status = Status;
	userlist[i].group = strdup(Group.local8Bit());
	// I don't know why userlist.description has QString* type,
	// instead of QString or char* like other fields
	userlist[i].description = new QString(Description);
	writeUserlist(NULL);

	syncUserlist();
	sortUsers();

	gg_add_notify(&sess, userlist[i].uin);
};


/* don't worry if you can't follow this. I can't either. */
/* should brew beer, sorts users instead */
void Kadu::sortUsers() {
    unsigned int i, j, k = 0;
    bool item_selected = 0;
    
    for (i = 0; i < mylist->count(); i++) {
	if (mylist->isSelected(i)) {
	    item_selected = i;
	    break;
	    }
	} // int i

    for (i = 0; i < mylist->count(); i++) {
	j = 0;
	while (j < userlist.size() && QString::compare(__c2q(userlist[j].nickname), mylist->item(i)->text()))
	    j++;
    
	if (userlist[j].status == GG_STATUS_NOT_AVAIL_DESCR) {
	    QPixmap * gg_st;
	    if (ifPendingMessages(userlist[j].uin))
		gg_st = new QPixmap((const char**)gg_msg_xpm);
	    else
		gg_st = new QPixmap((const char**)gg_inactdescr_xpm);		
	    QString tmpstr;
	    tmpstr = mylist->item(i)->text();
	    mylist->changeItem(*gg_st, tmpstr, i);
	    delete gg_st;				
	    }
	else
	    if (userlist[j].status == GG_STATUS_AVAIL || userlist[j].status == GG_STATUS_AVAIL_DESCR) {
		QPixmap * gg_st;

		if (ifPendingMessages(userlist[j].uin))
		    gg_st = new QPixmap((const char**)gg_msg_xpm);
		else
		    if (userlist[j].status == GG_STATUS_AVAIL_DESCR)
			gg_st = new QPixmap((const char**)gg_actdescr_xpm);
    		    else
			gg_st = new QPixmap((const char**)gg_act_xpm);

		QPixmap * gg_st2;
		QString tmpstr, tmpstr2;
		tmpstr = mylist->item(i)->text();
		tmpstr2 = mylist->item(k)->text();
		fprintf(stderr, "KK Kadu::sortUsers(): k: %d, i: %d, j: %d\n", k, i, j);
		gg_st2 = new QPixmap(*mylist->item(k)->pixmap());
		mylist->changeItem(*gg_st, tmpstr, k);
		mylist->changeItem(*gg_st2, tmpstr2, i);
		delete gg_st;
		delete gg_st2;
		struct userlist tmpustr;
		tmpustr = userlist[k];
		userlist[k] = userlist[i];
		userlist[i] = tmpustr;
		k++;
		}
	    else
		if (userlist[j].status == GG_STATUS_BUSY || userlist[j].status == GG_STATUS_BUSY_DESCR) {
		    QPixmap * gg_st;
		    if (ifPendingMessages(userlist[j].uin))
			gg_st = new QPixmap((const char**)gg_msg_xpm);
		    else
			if (userlist[j].status == GG_STATUS_BUSY_DESCR)
			    gg_st = new QPixmap((const char**)gg_busydescr_xpm);
			else
			    gg_st = new QPixmap((const char**)gg_busy_xpm);

		    QPixmap * gg_st2;
		    QString tmpstr, tmpstr2;
		    tmpstr = mylist->item(i)->text();
		    tmpstr2 = mylist->item(k)->text();
		    fprintf(stderr, "KK Kadu::sortUsers(): k: %d, i: %d, j: %d\n", k, i, j);
		    gg_st2 = new QPixmap(*mylist->item(k)->pixmap());
		    mylist->changeItem(*gg_st, tmpstr, k);
		    mylist->changeItem(*gg_st2, tmpstr2, i);
		    delete gg_st;
		    delete gg_st2;
		    struct userlist tmpustr;
		    tmpustr = userlist[k];
		    userlist[k] = userlist[i];
		    userlist[i] = tmpustr;
		    k++;
		    }
		else
		    if (userlist[j].status == GG_STATUS_INVISIBLE2) {
			QPixmap * gg_st;
			if (ifPendingMessages(userlist[j].uin))
			    gg_st = new QPixmap((const char**)gg_msg_xpm);
			else
			    gg_st = new QPixmap((const char**)gg_invi_xpm);
			QPixmap * gg_st2;
			QString tmpstr, tmpstr2;
			tmpstr = mylist->item(i)->text();
			tmpstr2 = mylist->item(k)->text();
			fprintf(stderr, "KK Kadu::sortUsers(): k: %d, i: %d, j: %d\n", k, i, j);
			gg_st2 = new QPixmap(*mylist->item(k)->pixmap());
			mylist->changeItem(*gg_st, tmpstr, k);
			mylist->changeItem(*gg_st2, tmpstr2, i);
			delete gg_st;
			delete gg_st2;
			struct userlist tmpustr;
			tmpustr = userlist[k];
			userlist[k] = userlist[i];
			userlist[i] = tmpustr;
			k++;
			}

	mylist->setSelected(i, false);
	} //int i

    mylist->setSelected(item_selected, true);
}

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
//	setCurrentStatus(beforeAutoAway);
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

int Kadu::openChat(QArray<uin_t> senders) {
    int i,j;
    QArray<uin_t> uins;
    
    i = 0;
    while (i < chats.size() && (*chats[i].uins) != senders)
	i++;
    
    if (i == chats.size()) {
//        j = 0;
//	while (j < userlist.size() && userlist[j].uin != uin)
//	    j++;
	Chat *chat;
	uins.duplicate(senders);
//	uins[0] = userlist[j].uin;
	chat = new Chat(uins, 0);
	chat->show();
	}
    else {
        chats[i].ptr->setActiveWindow();
	return i;
	}
    
    i = 0;
    while (i < chats.size() && (*chats[i].uins) != senders)
	i++;

    return i;
}

/* menu and popup menu parser */
void Kadu::commandParser (int command) {
    int i = 0;
    char buf1[255];
    uin_t uin;
    SearchDialog *sd;
    QArray<uin_t> uins;
    QString tmp;
    		
    switch (command) {
	case 1:
	    Message *msg;
	    msg = new Message(mylist->currentText());
	    msg->show();
	    break;
	case 2:
	    tmp = mylist->currentText();
	    uin = UserToUin(&tmp);
	    uins.resize(1);
	    uins[0] = uin;
	    openChat(uins);
	    break;
	case 3:
	    tmp = mylist->currentText();
	    removeUser(tmp, false);
	    break;
	case 4:
	    //clear history
	    confirmHistoryDeletion(mylist->currentText().local8Bit());
	    break;
	case 5:
	    tmp = mylist->currentText();
	    History *hb;
	    hb =new History(UserToUin(&tmp));
	    hb->show();
	    break;
	case 6:
	    tmp = mylist->currentText();
	    UserInfo *ui;
	    ui = new UserInfo("user info", 0, UserToUin(&tmp));
	    ui->show();
	    break;
	case 7:
	    sd = new SearchDialog;
	    sd->show();
//		sd->doSearch();
	    break;
	case 8:
//		QPopupMenu *ppm;
//		ppm = mmb->findItem(0);
	    mute = !mute;
	    if (mute) {
	        QPixmap snd_unmute((const char **)snd_mute_xpm);
	        QIconSet icon(snd_unmute);
	        mmb->changeItem(8, icon, i18n("Unmute sounds"));
	        }
	    else {
	        QPixmap snd_mute((const char **)snd_unmute_xpm);
	        QIconSet icon(snd_mute);
	        mmb->changeItem(8, icon, i18n("Mute sounds"));		
	        }
	    break;
	case 9:
	    Adduser *au;
	    au = new Adduser(0, "add_user");
	    au->show();
	    break;
	case 10:
	    Ignored *ign;
	    ign = new Ignored(0, "ignored");
	    ign->show();
	    break;
	case 11:
	    About *about;
	    about = new About;
	    about->show();
	    break;
	case 12:
	    send_userlist();
	    break;
	case 13:
	    Sms *sms;
	    sms = new Sms(0, mylist->currentText(), 0);
	    sms->show();
	    break;
	case 14:
	    remindPassword();
	    break;
	case 15:
	    Register *reg;
	    reg = new Register;
	    reg->show();
	    break;
	case 16:
	    close_permitted = true;
	    disconnectNetwork();
	    close(true);
	    break;
	case 17:
	    tmp = mylist->currentText();
	    sd = new SearchDialog(0,i18n("User info"), UserToUin(&tmp));
	    sd->show();
	    sd->doSearch();
	    break;
	case 18:
	    UserlistImport *uli;
	    uli = new UserlistImport;
	    uli->show();
	    break;
	case 19:
	    ConfigDialog *cd;
	    cd = new ConfigDialog;
	    cd->show();
	    break;
	case 20:
	    UserlistExport *ule;
	    ule = new UserlistExport;
	    ule->show();
	    break;
	case 21:
	    close_permitted = false;
	    close(true);
	    break;
	case 22:
	    struct gg_dcc *dcc_new;
		
	    tmp = mylist->currentText();
	    uin = UserToUin(&tmp);	
	
	    i = 0;
	    while (i<userlist.size() && userlist[i].uin != uin)
	        i++;
	
	    if (i < userlist.size()) {
	        dccSocketClass *dcc;
	        if (userlist[i].port >= 10) {
	    	    if ((dcc_new = gg_dcc_send_file(userlist[i].ip, userlist[i].port, config.uin, uin)) != NULL) {
		        dcc = new dccSocketClass(dcc_new);
		        connect(dcc, SIGNAL(dccFinished(dccSocketClass *)), this, SLOT(dccFinished(dccSocketClass *)));		    
		        dcc->initializeNotifiers();
		        }
		    }
		else {
		    acks.resize(acks.size() + 1);
		    i = acks.size() - 1;
		    acks[i].ack = 0;
		    acks[i].seq = gg_dcc_request(&sess, uin);
		    acks[i].type = 0;
		    acks[i].ptr = NULL;
		    }
		}
		    
	    break;
	case 23:
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
  kadu->syncUserlist();
  kadu->sortUsers();
}

/* the list that pops up if we right-click one someone */
void Kadu::listPopupMenu (QListBoxItem * item) {
    if (item == NULL)
	return;

    QPopupMenu * pm;
    pm = new QPopupMenu(this);

    KIconLoader *loader = KGlobal::iconLoader();

    QPixmap msg;
    msg = loader->loadIcon("mail_generic", KIcon::Small);

    int i = 0;
    while (item->text().compare(__c2q(userlist[i].nickname)))
	i++;

    pm->insertItem(msg, i18n("Send message"), 1);
    pm->insertItem(i18n("Open chat window"), 2);
    pm->insertItem(i18n("Send SMS"), 13);
    pm->insertItem(loader->loadIcon("filesave", KIcon::Small), i18n("Send file"), 22);
    if (userlist[i].status == GG_STATUS_AVAIL || userlist[i].status == GG_STATUS_AVAIL_DESCR ||
	userlist[i].status == GG_STATUS_BUSY || userlist[i].status == GG_STATUS_BUSY_DESCR)
	pm->setItemEnabled(22, true);
    else
	pm->setItemEnabled(22, false);
    
    pm->insertSeparator();
	
    pm->insertItem(loader->loadIcon("remove", KIcon::Small), i18n("Remove from userlist"), 3);
    pm->insertItem(loader->loadIcon("eraser", KIcon::Small), i18n("Clear history"), 4);
    QPixmap history;
    history = loader->loadIcon("history", KIcon::Small);
    pm->insertItem(history, i18n("View history"), 5);
    pm->insertItem(loader->loadIcon("identity", KIcon::Small), i18n("View/edit user info"), 6);
    pm->insertItem(loader->loadIcon("viewmag", KIcon::Small), i18n("Lookup in directory"), 17);
    pm->insertSeparator();
    pm->insertItem(i18n("About..."), 11);


    connect(pm, SIGNAL( activated(int) ), this, SLOT( commandParser(int) ));	
    pm->exec(QCursor::pos());    	
}

/* if something's pending, open it, if not, open new message */
void Kadu::sendMessage (QListBoxItem * item) {
    QString tmp;
    int i,j;
    bool stop = false;
    rMessage *rmsg;
    Message *msg;
    
    tmp = item->text();
    uin_t uin = (unsigned int)UserToUin((const QString *)&tmp);
    for (i = 0; i < pending.size(); i++)
	if ((*pending[i].uins)[0] == uin)
	    if (pending[i].msgclass == GG_CLASS_CHAT) {
		j = openChat(*pending[i].uins);
		chats[j].ptr->checkPresence(*pending[i].uins, pending[i].msg, pending[i].time);	    
		deletePendingMessage(i);
		i--;
		stop = true;
		}
	    else {
		if (!stop)
		    {
  		    rmsg = new rMessage(item->text(), i);
	  	    rmsg->show();
		    }
		return;
		}
    if (stop) {
	kadu->sortUsers();
	kadu->syncUserlist();
	return;
	}

    if (GetStatusFromUserlist(uin) != GG_STATUS_NOT_AVAIL && GetStatusFromUserlist(uin) != GG_STATUS_NOT_AVAIL_DESCR)
	{
	QArray<uin_t> uins(1);
	uins[0] = uin;
	openChat(uins);
	}	
    else {
	msg = new Message(item->text());
	msg->show();
	}
    
    return;
}

/* when we want to change the status */
void Kadu::slotHandleState (int command) {
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
    if (ifPendingMessages())
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
	if (with_description)
	    {
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
		QMessageBox::information(0, i18n("Information"), i18n("File has been transferred sucessfully."),
		    i18n("&OK"));
		break;
	    case DCC_SOCKET_TRANSFER_DISCARDED:
		QMessageBox::information(0, i18n("Information"), i18n("File transfer has been discarded."),
		    i18n("&OK"));
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
	nConnect(GG_CHECK_READ);
}

void Kadu::dataSent(void) {
    fprintf(stderr, "KK Kadu::dataSent()\n");
    kadusnw->setEnabled(false);
    if (sess.check & GG_CHECK_WRITE)
	nConnect(GG_CHECK_WRITE);
}

/* the main network function called every n msec from QTimer */
void Kadu::nConnect(int state) {
    int i;
    static int calls = 0;
    
    fprintf(stderr, "KK Kadu::nConnect()\n");
    calls++;
    if (calls > 1)
	fprintf(stderr, "************* KK Kadu::nConnect(): Recursive nConnect calls detected!\n");
    if (!(e = gg_watch_fd(&sess))) {
	fprintf(stderr,"KK Kadu::nConnect(): Connection broken unexpectedly!\n");
	char error[512];
	disconnectNetwork();
	snprintf(error, sizeof(error), "KK Kadu::nConnect(): Unscheduled connection termination\n");
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
    	    fprintf(stderr, "KK Kadu::nConnect(): Resolving address\n");
	    break;
	case GG_STATE_CONNECTING_HUB:
	    fprintf(stderr, "KK Kadu::nConnect(): Connecting to hub\n");
	    break;
	case GG_STATE_READING_DATA:
	    fprintf(stderr, "KK Kadu::nConnect(): Fetching data from hub\n");
	    break;
	case GG_STATE_CONNECTING_GG:
	    fprintf(stderr, "KK Kadu::nConnect(): Connecting to server\n");
	    break;
	case GG_STATE_READING_KEY:
	    fprintf(stderr, "KK Kadu::nConnect(): Waiting for hash key\n");
	    break;
	case GG_STATE_READING_REPLY:
	    fprintf(stderr, "KK Kadu::nConnect(): Sending key\n");
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
		
	    i = 0;
	    while (i < userlist.size() && userlist[i].uin != e->event.msg.sender)
		i++;
	    if (i < userlist.size()) {
		dccSocketClass *dcc;
		dcc_new = gg_dcc_get_file(userlist[i].ip, userlist[i].port, config.uin, e->event.msg.sender);
		dcc = new dccSocketClass(dcc_new);
		connect(dcc, SIGNAL(dccFinished(dccSocketClass *)), this, SLOT(dccFinished(dccSocketClass *)));		    
		dcc->initializeNotifiers();
		}
	    }
	else {
	    QArray<uin_t> uins;
	    fprintf(stderr, "KK nConnect(): %d\n", e->event.msg.recipients_count);
	    if (e->event.msg.msgclass == GG_CLASS_CHAT) {
		uins.resize(e->event.msg.recipients_count + 1);
		uins[0] = e->event.msg.sender;
		for (i = 1; i < e->event.msg.recipients_count + 1; i++)
		    uins[i] = e->event.msg.recipients[i - 1];
		}
	    else {
		uins.resize(1);
		uins[0] = e->event.msg.sender;
		}
	    eventRecvMsg(e->event.msg.msgclass, uins, e->event.msg.message, e->event.msg.time, 0, NULL);
	    }
	}

    if (e->type == GG_EVENT_NOTIFY_DESCR) {
	eventGotUserlistWithDescription(e);
	sortUsers();
	}

    if (e->type == GG_EVENT_NOTIFY) {
	eventGotUserlist(e);
	sortUsers();
	}
	    
    if (e->type == GG_EVENT_STATUS) {
	eventStatusChange(e);
	syncUserlist();
	sortUsers();
	}

    if (e->type == GG_EVENT_ACK) {
	fprintf(stderr, "KK Kadu::nConnect(): message reached %d (seq %d)\n", e->event.ack.recipient, e->event.ack.seq);
	ackHandler(e->event.ack.seq);
	}

    if (e->type == GG_EVENT_CONN_SUCCESS) {
    	doBlink = false;
	send_userlist();
			
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
	snprintf(error, sizeof(error), "KK Kadu::nConnect(): Unable to connect, the following error has occured:\n%s\nKK Kadu::nConnect(): Keep trying to connect?\n", strerror(errno));
	disconnectNetwork();	
	setCurrentStatus(GG_STATUS_NOT_AVAIL);
	fprintf(stderr, "KK Kadu::nConnect(): Connection failed\n");
	fprintf(stderr, error);
	    
	if (autohammer)
	    setStatus(loginparams.status & (~GG_STATUS_FRIENDS_MASK));
/*	    else {
    		switch (QMessageBox::information(kadu, i18n("Connect error"), error, i18n("Yes"), i18n("No"), QString::null, 0, 1)) {
		    case 0: // Yes?
//		    autohammer = true;
			setStatus(loginparams.status & (~GG_STATUS_FRIENDS_MASK));
			break;
		    case 1: // No?
			break;
		    }
		}*/
	}

    if (socket_active)
	if (sess.state == GG_STATE_IDLE && userlist_sent) {
	    char error[512];
	    socket_active = false;
	    int i = 0;
	    QPixmap qp_inact((const char **)gg_inact_xpm);
	    while (userlist[i].uin != 0) {
	        mylist->changeItem(qp_inact, userlist[i].nickname, i);
		i++;
		}
	    snprintf(error, sizeof(error), "KK Kadu::nConnect(): Unscheduled connection termination\n");
	    fprintf(stderr, error);
	    disconnectNetwork();			
	    setCurrentStatus(GG_STATUS_NOT_AVAIL);
	    if (autohammer)
		setStatus(config.defaultstatus & (~GG_STATUS_FRIENDS_MASK));
	    //QMessageBox::warning(kadu, i18n("Connect error"), error);
	    }
	else
	    if (sess.check & GG_CHECK_WRITE)
	        kadusnw->setEnabled(true);	
	    
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

    i_am_busy = FALSE;
    disconnect_planned = TRUE;
    gg_logoff(&sess);
    userlist_sent = FALSE;

    i = 0;
    while (i < userlist.size()) {
	UinToUserlistEntry(userlist[i].uin, GG_STATUS_NOT_AVAIL);
	i++;
	}

    i = 0;
    while (i < chats.size()) {
	chats[i].ptr->setTitle();
	i++;
	}
	
    syncUserlist();
    sortUsers();

    socket_active = FALSE;
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
        
    if (!config.dock)
	return;
    
    if (e->button() == MidButton) {
	bool stop = false;
	
	uin_t uin = 0;
	for (i = 0; i < pending.size(); i++) {
	    if (!uin || (*pending[i].uins)[0] == uin)
		if (pending[i].msgclass == GG_CLASS_CHAT) {
		    uin = (*pending[i].uins)[0];
		    j = kadu->openChat(*pending[i].uins);
		    chats[j].ptr->checkPresence(*pending[i].uins, pending[i].msg, pending[i].time);	    
		    deletePendingMessage(i);
	    	    i--;
		    stop = true;
		    }		
		else {
		    if (!stop) {
			rMessage *rmsg;
			rmsg = new rMessage(UinToUser((*pending[i].uins)[0]).local8Bit(), i);
			rmsg->show();
			}
		    return;
		    }
	    }
	if (stop) {
	    kadu->sortUsers();
	    kadu->syncUserlist();
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
    //	may be of use
    if (sess.password !=NULL)
	free(sess.password);
    writeIgnored(NULL);
}

Kadu::~Kadu(void) {
}

void Kadu::createMenu() {

    KIconLoader *loader = KGlobal::iconLoader();

    QPopupMenu * ppm = new QPopupMenu(this);
    ppm->insertItem(i18n("Manage &ignored"), 10);
    ppm->insertItem(loader->loadIcon("configure", KIcon::Small), i18n("&Configuration"), 19);
    ppm->insertItem(loader->loadIcon("reload", KIcon::Small), i18n("Resend &userlist"), 12);
    if (mute) {
	QPixmap snd_unmute((const char **)snd_mute_xpm);
	QIconSet icon(snd_unmute);
	ppm->insertItem(icon, i18n("Unmute sounds"), 8);
	}
    else {
	QPixmap snd_mute((const char **)snd_unmute_xpm);
	QIconSet icon(snd_mute);
	ppm->insertItem(icon, i18n("Mute sounds"), 8);		
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

    ppm->insertItem(i18n("Remind &password"), 14);
    QPixmap new__user((const char **)new_user);
    ppm->insertItem(new__user,i18n("Register &new user"), 15);
    ppm->insertItem(i18n("Personal information"), 23);
    ppm->insertSeparator();
    QPixmap find;
    find = loader->loadIcon("viewmag", KIcon::Small);
    ppm->insertItem(find, i18n("&Search for users"), 7);
    ppm->insertItem(i18n("I&mport userlist"), 18);
    ppm->insertItem(i18n("E&xport userlist"), 20);
    ppm->insertItem(QPixmap((const char **)gg_act_xpm),i18n("&Add user"), 9);
    ppm->insertSeparator();
    ppm->insertItem(i18n("A&bout..."), 11);
    ppm->insertSeparator();
    ppm->insertItem(i18n("&Hide Kadu"), 21);
    ppm->insertItem(loader->loadIcon("exit", KIcon::Small), i18n("&Exit Kadu"), 16);

    mmb = new KMenuBar(this);
    mmb->insertItem(i18n("&Kadu"), ppm);
    mmb->polish();

    connect(ppm, SIGNAL( activated(int) ), this, SLOT( commandParser(int) ));

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
