/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KADU_H
#define KADU_H

/** values below most probably need no alteration **/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kmenubar.h>
#include <qprogressdialog.h>
#include <qmultilineedit.h>
#include <kapp.h>
#include <qsocketnotifier.h>
#include <qarray.h>
#include <qmainwindow.h>
#include <qtabbar.h>
#include <kpopupmenu.h>

#include "userlist.h"
#include "userbox.h"
#include "pending_msgs.h"
#include "misc.h"
#include "status.h"

struct colors {
	QString userboxBgColor;
	QString userboxFgColor;
	QString chatMyBgColor;
	QString chatUsrBgColor;
	QString chatMyFontColor;
	QString chatUsrFontColor;
};


struct config {
	uin_t uin;
	QString password;
	QString nick;
	char *soundmsg;
	char *soundchat;
	char *soundprog;
	double soundvol;
	bool soundvolctrl;
	bool playartsdsp;
	bool playsound;
	bool logmessages;
	bool savegeometry;
	bool playsoundchat;
	bool playsoundchatinvisible;
	int defaultstatus;
	QString defaultdescription;
	int sysmsgidx;
	bool allowdcc;
	char *dccip;
	char *extip;
	int extport;
	QStringList servers;
	bool dock;
	bool privatestatus;
	bool rundocked;
	bool grouptabs;
	QRect geometry;

	bool smsbuildin;
	char * smsapp;
	bool smscustomconf;
	char * smsconf;

	bool emoticons;
	QString emoticons_theme;
	bool autosend;
	bool scrolldown;
	int chatprunelen;
	bool chatprune;
	bool msgacks;

	bool autoaway;
	int autoawaytime;

	QStringList notifies;
	char *soundnotify;
	bool notifyglobal;
	bool notifyall;
	bool notifydialog;
	bool notifysound;

	bool useproxy;
	QString proxyaddr;
	unsigned short proxyport;
	QString proxyuser;
	QString proxypassword;

	bool raise;

	struct colors colors;
	QString userboxFont;
	int userboxFontSize;
	QString chatFont;
	int chatFontSize;
};

struct groups {
  int number;
  char * name;
};

class dccSocketClass;

class Kadu : public QMainWindow
{
	Q_OBJECT
	 
	public:
		QTimer * autoaway;
		Kadu(QWidget* parent=0, const char *name=0);
		~Kadu();
		bool autoawayed;
		bool autohammer;
		bool userInActiveGroup(uin_t uin);
		// code for addUser has been moved from adduser.cpp
		// for sharing with search.cpp
		void addUser(const QString& FirstName,const QString& LastName,
			const QString& NickName,const QString& AltNick,
			const QString& Mobile,const QString& Uin,const int Status,
			const QString& Group,const QString& Description, const bool Foreign = false);
		void removeUser(QString &, bool);
		void refreshGroupTabBar();
		void setClosePermitted(bool permitted);
		
		UserBox *userbox;

	protected:
		void enterEvent(QEvent *);
		void closeEvent(QCloseEvent *);	
		bool event(QEvent *e);
		void watchDcc(void);
		struct gg_event *dcc_e;
		int dcc_ret;

		int activegrpno;

		bool blinkOn;
		bool doBlink;

		AutoStatusTimer* autostatus_timer;

	public slots:
		void blink();
		void dccFinished(dccSocketClass *dcc);
		void slotHandleState(int command);
		void slotShowStatusMenu();
		void setCurrentStatus(int status);
		void sendMessage(QListBoxItem *);
		void listPopupMenu(QListBoxItem *);
		void commandParser(int);
		void dataReceived();
		void dataSent();
		void dccReceived();
		void dccSent();
		void eventHandler(int state);
		void prepareDcc(void);
		void autoAway(void);
		void cleanUp(void);
		void pingNetwork(void);
		void checkConnection(void);
		void setStatus(int);
		void disconnectNetwork(void);
		void changeGroup(int);
		int openChat(UinsList);
	
	private:
		QMenuBar *mmb;
		QTabBar *group_bar;
		int beforeAutoAway;
		int commencing_startup;
		void createMenu();
		void createStatusPopupMenu();
		void setActiveGroup(const QString& group);
		bool close_permitted;
		//friend class DockWidget;
	
	private slots:
		void groupTabSelected(int id);
		void userListModified();
		void userListStatusModified(UserListElement *);
};

struct acks {
    int ack;
    int seq;
    int type;
    QDialog *ptr;
};

class Operation : public QProgressDialog {
	Q_OBJECT
	public:
	Operation(const QString & labelText, const QString & cancelButtonText, int totalSteps, QWidget *parent = 0 );
	int laststate;
	QTimer * t;
	int steps;

	private slots:
	void perform();
	void cancel();
};

class MyLabel : public QLabel {
	Q_OBJECT
	public:
		MyLabel(QWidget *parent, const char *name): QLabel(parent, name) {};

	protected:
		void mousePressEvent (QMouseEvent * e);
};

class Chat;

struct chats {
    UinsList uins;
    Chat *ptr;
    bool operator==(const chats& r) const
    {
    	return (uins==r.uins)&&(ptr==r.ptr);
    };
};

extern char **gg_xpm[];
extern KApplication *a;
extern Kadu *kadu;
extern struct gg_session *sess;
extern struct config config;
// Ominiecie bledu w gcc 3.2
static QValueList<chats> chats_gcc32_bug;
////////////////////////////
extern QValueList<chats> chats;
extern UserList userlist;
extern QArray<uin_t> ignored;
extern PendingMsgs pending;
// Ominiecie bledu w gcc 3.2
static QArray<groups> grouplist_gcc32_bug;
////////////////////////////
extern QArray<groups> grouplist;
extern bool mute;
extern bool userlist_sent;
extern int server_nr;
// Ominiecie bledu w gcc 3.2
static QArray<acks> acks_gcc32_bug;
////////////////////////////
extern QArray<acks> acks;
extern QString own_description;

extern QPopupMenu *statusppm;
extern QSocketNotifier *kadusnr;
extern QSocketNotifier *kadusnw;
extern QSocketNotifier *dccsnr;
extern QSocketNotifier *dccsnw;

void deletePendingMessage(int nr);
void sendUserlist(void);

void createKonfig(char *, uin_t);
void addIgnored(uin_t);
void delIgnored(uin_t);
bool isIgnored(uin_t);
int writeIgnored(char *);
void *watch_socket(void *);
void kadu_debug(int, char*);
void playSound(char *);

void readConfig(void);
QString pwHash(const QString);
void confirmHistoryDeletion(const char *);
void remindPassword();

extern KPopupMenu *dockppm;

#endif
