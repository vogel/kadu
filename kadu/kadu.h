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

#define KADU_VERSION 0.3.1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kmenubar.h>
#include <ksystemtray.h>
#include <qprogressdialog.h>
#include <qmultilineedit.h>
#include <kapp.h>
#include <qsocketnotifier.h>
#include "userbox.h"

#define GG_STATUS_INVISIBLE2 0x0009 /* g³upy... */

#define __c2q(__char_pointer__) QString::fromLocal8Bit(__char_pointer__)

struct userlist {
    char *first_name, *last_name, *nickname, *comment, *mobile, *group;
    QString *description;
    uin_t uin;
    unsigned int status;
    bool anonymous;
    int ip;
    short port;
};

struct pending {
    uin_t uin;
    QString * msg;
    int msgclass;
    time_t time;
};

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

class Kadu : public QWidget
{
	Q_OBJECT
	 
	public:
		QTimer * autoaway;
		Kadu(QWidget* parent=0, const char *name=0);
		~Kadu();
		bool autoawayed;
		bool autohammer;
		void syncUserlist(void);
		bool userInActiveGroup(uin_t uin);
		// code for addUser has been moved from adduser.cpp
		// for sharing with search.cpp
		void addUser(const QString& FirstName,const QString& LastName,
			const QString& NickName,const QString& AltNick,
			const QString& Mobile,const QString& Uin,const int Status,
			const QString& Group,const QString& Description);

    protected:
	void enterEvent (QEvent *);
	void closeEvent (QCloseEvent *);	
	void showEvent (QShowEvent * e);
	bool event(QEvent *e);
	void watchDcc(void);
	struct timeval dcc_tv;
	struct gg_event *dcc_e;
	fd_set dcc_rd, dcc_wd;
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
	void sortUsers(void);
	void dataReceived();
	void dataSent();
	void dccReceived();
	void dccSent();
	void nConnect(int state);
	void prepareDcc(void);
	void autoAway(void);
	void cleanUp(void);
	void pingNetwork(void);
	void checkConnection(void);
	void setStatus(int);
	void disconnectNetwork(void);
	void changeGroup(int);
	int openChat(uin_t);
	
    private:
	KMenuBar *mmb;
	int beforeAutoAway;
	int commencing_startup;
	void createMenu();
	void createStatusPopupMenu();
	bool close_permitted;

    friend class DockWidget;
};

struct acks {
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
    MyLabel ( QWidget * parent ): QLabel(parent) { ; };

  protected:
    void mousePressEvent (QMouseEvent * e);

};

class Chat;

struct chats {
    unsigned int uin;
    Chat *ptr;
};

extern const char *statustext[8];
extern char **gg_xpm[8];
extern int gg_statuses[8];
extern KApplication *a;
extern Kadu *kadu;
extern MyListBox *mylist;
extern struct gg_session sess;
extern struct config config;
extern QArray<chats> chats;
extern QArray<userlist> userlist;
extern QArray<uin_t> ignored;
extern QArray<pending> pending;
extern QArray<groups> grouplist;
extern int userlist_count;
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
void removeUser(QString &, bool);
int writeUserlist(char *);
char *preparePath(char *);
uin_t UserToUin(const QString *);
QString UserToMobile(const QString *);
int getActualStatus();
bool ifStatusWithDescription(int status);
void deletePendingMessage(int nr);

void createKonfig(char *, uin_t);
void addIgnored(uin_t);
void delIgnored(uin_t);
bool isIgnored(uin_t);
int writeIgnored(char *);
int readUserlist(void);
void *watch_socket(void *);
void kadu_debug(int, char*);
void playSound(char *);

extern "C" { void send_userlist(void); }
QString UinToUser(uin_t);
bool ifPendingMessages(int uin = -1);
void readConfig(void);
char *pwHash(const char *);
void confirmHistoryDeletion(const char *);
void remindPassword();
void UinToUserlistEntry (uin_t uin, int new_status);
int GetStatusFromUserlist (unsigned int uin);

#endif
