/***************************************************************************
                          kadu.h  -  description
                             -------------------
    begin                : wto sie 21 18:35:52 CEST 2001
    copyright            : (C) 2001 by tomee
    email                : tomee@cpi.pl
 ***************************************************************************/

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

#define KADU_VERSION 0.3.2

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kmenubar.h>
#include <ksystemtray.h>
#include <qprogressdialog.h>
#include <qmultilineedit.h>
#include <kapp.h>
#include <qsocketnotifier.h>
#include <qarray.h>
#include <qmainwindow.h>
#include <qtabbar.h>

#include "userlist.h"
#include "userbox.h"
#include "pending_msgs.h"
#include "misc.h"

#define GG_STATUS_INVISIBLE2 0x0009 /* g³upy... */

struct colors {
    int userboxBgR;
    int userboxBgG;
    int userboxBgB;
    int userboxFgR;
    int userboxFgG;
    int userboxFgB;
    char* userboxFont;
    int userboxFontSize;
};


struct config {
	uin_t uin;
	char *password;
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
	char *server;
	bool dock;
	bool privatestatus;
	bool rundocked;
	bool grouptabs;
	QRect geometry;

	char * smsapp;
	bool smscustomconf;
	char * smsconf;

	bool emoticons;
	char *emoticonspath;
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
	bool notifydialog;
	bool notifysound;

	bool useproxy;
	char * proxyaddr;
	unsigned short proxyport;

	bool raise;

	struct colors colors;
};

struct groups {
  int number;
  char * name;
};

class dccSocketClass;
class DockWidget;

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
		friend class DockWidget;
	
	private slots:
		void groupTabSelected(int id);
		void userListModified();
};

struct acks {
    int ack;
    int seq;
    int type;
    QDialog *ptr;
};

class DockWidget : public KSystemTray  {
   Q_OBJECT

 public:
   DockWidget(QWidget *parent=0, const char *name=0);
   void DockWidget::setType(char **gg_xpm);
   
 public slots:
   // Status change slots
   void dockletChange(int id);
   
    
	protected:
		void mousePressEvent (QMouseEvent*);

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

/*class About : public QDialog {
 Q_OBJECT
	public:
		About(QDialog* parent=0, const char *name=0);

};*/

class ChooseDescription : public QDialog {
	Q_OBJECT
	public:
	ChooseDescription::ChooseDescription ( int nr, QWidget * parent=0, const char * name=0);

	private:
		QMultiLineEdit * desc;

	private slots:
		void okidokiPressed();

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
};

extern const char *statustext[8];
extern char **gg_xpm[8];
extern int gg_statuses[8];
extern KApplication *a;
extern Kadu *kadu;
extern struct gg_session sess;
extern struct config config;
extern QValueList<struct chats> chats;
extern UserList userlist;
extern QArray<uin_t> ignored;
extern PendingMsgs pending;
extern QArray<groups> grouplist;
extern DockWidget *dw;
extern bool mute;
extern bool userlist_sent;
extern QArray<acks> acks;
extern QString own_description;

extern QPopupMenu *statusppm;
extern QSocketNotifier *kadusnr;
extern QSocketNotifier *kadusnw;
extern QSocketNotifier *dccsnr;
extern QSocketNotifier *dccsnw;

void cp_to_iso(unsigned char *);
void iso_to_cp(unsigned char *);
int statusGGToStatusNr(int);
int getActualStatus();
bool ifStatusWithDescription(int status);
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
char *pwHash(const char *);
void confirmHistoryDeletion(const char *);
void remindPassword();
unsigned int GetStatusFromUserlist(uin_t uin);

#endif
