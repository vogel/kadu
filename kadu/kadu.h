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

#include <qapplication.h>
#include <qprogressdialog.h>
#include <qmultilineedit.h>
#include <qsocketnotifier.h>
#include <qarray.h>
#include <qmainwindow.h>
#include <qpopupmenu.h>
#include <qnetworkprotocol.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qevent.h>
#include <qframe.h>
#include <qlayout.h>

#include "userlist.h"
#include "userbox.h"
#include "pending_msgs.h"
#include "status.h"
/*
struct groups {
  int number;
  char * name;
};
*/
class dccSocketClass;

class KaduTabBar;

class Kadu : public QMainWindow
{
	Q_OBJECT
	 
	public:
		Kadu(QWidget* parent=0, const char *name=0);
		void changeAppearance();
		~Kadu();
		bool autohammer;
		bool userInActiveGroup(uin_t uin);
		void addUser(const QString& FirstName,const QString& LastName,
			const QString& NickName,const QString& AltNick,
			const QString& Mobile,const QString& Uin,const int Status,
			const QString& Group,const QString& Description, const bool Anonymous = false);
		void removeUser(QStringList &, bool);
		void refreshGroupTabBar();
		void setClosePermitted(bool permitted);
    
		UserBox *userbox;

		AutoStatusTimer* autostatus_timer;    

	protected:
		void closeEvent(QCloseEvent *);	
		bool event(QEvent *e);
		void watchDcc(void);
		void keyPressEvent(QKeyEvent *e);
		void resizeEvent(QResizeEvent *e);

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
		void cleanUp(void);
		void pingNetwork(void);
		void checkConnection(void);
		void setStatus(int);
		void disconnectNetwork(void);
		void changeGroup(int);
		int openChat(UinsList);
		void gotUpdatesInfo(const QByteArray &data, QNetworkOperation *op);
		void currentChanged(QListBoxItem *item);
		void showdesc(void);
		void hidedesc(void);
		void statusMenuAboutToHide(void);
		void mouseButtonClicked(int, QListBoxItem *);

	private:
		QFrame *centralFrame;
		QGridLayout *grid;
		QMenuBar *mmb;
		KaduTabBar *group_bar;
		QTextBrowser *descrtb;
		int commencing_startup;
		void createMenu();
		void createStatusPopupMenu();
		void setActiveGroup(const QString& group);
		bool close_permitted;
	
	private slots:
		void groupTabSelected(int id);
		void userListModified();
		void userListStatusModified(UserListElement *);
};

struct acks {
    int ack;
    int seq;
    int type;
    QWidget *ptr;
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

extern QString gg_icons[];
extern QApplication *a;

extern Kadu *kadu;

extern struct gg_session *sess;
// Ominiecie bledu w gcc 3.2
static QValueList<chats> chats_gcc32_bug;
////////////////////////////
extern QValueList<chats> chats;
extern UserList userlist;
extern QValueList<uin_t> ignored;
extern PendingMsgs pending;
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

void createConfig();
void addIgnored(uin_t);
void delIgnored(uin_t);
bool isIgnored(uin_t);
int writeIgnored(QString filename = "");
void *watch_socket(void *);
void kadu_debug(int, char*);
void playSound(const QString &sound, const QString player = QString::null);
void readConfig(void);
QString pwHash(const QString);
void confirmHistoryDeletion(const char *);
extern QPopupMenu *dockppm;

#endif
