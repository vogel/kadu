#ifndef KADU_H
#define KADU_H

#include <qapplication.h>
#include <qprogressdialog.h>
#include <qmultilineedit.h>
#include <qsocketnotifier.h>
#include <qmainwindow.h>
#include <qpopupmenu.h>
#include <qnetworkprotocol.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qevent.h>
#include <qframe.h>
#include <qlayout.h>
#include <qtoolbutton.h>

#include "libgadu.h"
#include "misc.h"
#include "status.h"
#include "userbox.h"
#include "userlist.h"
#include "tabbar.h"
#include "dcc.h"

/**
	G³ówne okno Kadu
**/
class Kadu : public QMainWindow
{
	Q_OBJECT

	private:
		QFrame* centralFrame;
		QGridLayout* grid;
		QTextBrowser* descrtb;
		QToolButton* mutebtn;
		QMenuBar* MenuBar;
		QPopupMenu* MainMenu;		
		KaduTabBar* GroupBar;
		UserBox* Userbox;

		int commencing_startup;
		
		void createMenu();
		void createStatusPopupMenu();
		void setActiveGroup(const QString& group);

	private slots:
		void groupTabSelected(int id);
		void userListModified();
		void userListStatusModified(UserListElement *);
		void openChat();
		void userListUserAdded(const UserListElement& user);

	protected:
		bool event(QEvent *e);
		void watchDcc(void);
		void keyPressEvent(QKeyEvent *e);
		void resizeEvent(QResizeEvent *e);

		struct gg_event *dcc_e;
		int dcc_ret;
		int activegrpno;
		bool blinkOn;
	 
	public:
		Kadu(QWidget* parent=0, const char *name=0);
		void changeAppearance();
		~Kadu();
		bool userInActiveGroup(uin_t uin);
		void removeUser(QStringList &, bool);
		void refreshGroupTabBar();
		/**
			Zwraca wskaznik do belki menu glownego okna.
		**/
		QMenuBar* menuBar();
		/**
			Zwraca wskaznik do glownego menu programu.
		**/
		QPopupMenu* mainMenu();
		/**
			Zwraca wskaznik do zakladek z nazwami grup.
		**/		
    		KaduTabBar* groupBar();
		/**
			Zwraca wskaznik do userbox-a w glownym oknie.
		**/				
		UserBox* userbox();
    
		// te zmienne s± tylko chwilowo publiczne.
		// trzeba to uporz±dkowaæ
		bool autohammer;
		bool doBlink;
		AutoStatusTimer* autostatus_timer;

	public slots:
		void blink();
		void dccFinished(dccSocketClass *dcc);
		void slotHandleState(int command);
		void slotShowStatusMenu();
		void setCurrentStatus(int status);
		void sendMessage(QListBoxItem *);
		void dataReceived();
		void dataSent();
		void dccReceived();
		void dccSent();
		void prepareDcc(void);
		void pingNetwork(void);
		void setStatus(int);
		void disconnectNetwork(void);
		void changeGroup(int);
		void gotUpdatesInfo(const QByteArray &data, QNetworkOperation *op);
		void currentChanged(QListBoxItem *item);
		void showdesc(bool show = true);
		void statusMenuAboutToHide(void);
		void mouseButtonClicked(int, QListBoxItem *);
		void infopanelUpdate(uin_t);
		virtual bool close(bool quit = false);
		void quitApplication();

		void about();
		void addUserAction();
		void blockUser();
		void configure();
		void changePassword1();
		void deleteHistory();
		void deleteUsers();
		void exportUserlist();
		void help();
		void hideKadu();
		void ignoreUser();
		void importUserlist();
		void lookupInDirectory();
		void manageIgnored();
		void muteUnmuteSounds();
		void notifyUser();
		void offlineToUser();
		void personalInfo();
		void quit();
		void registerUser();
		void remindPassword1();
		void searchInDirectory();
		void sendFile();
		void makeVoiceChat();
		void sendKey();
		void sendSms();
		void sendSmsToUser();
		void showUserInfo();
		void unregisterUser();
		void viewHistory();
		void popupMenu();		
};

class MyLabel : public QLabel {
	Q_OBJECT
	public:
		MyLabel(QWidget *parent, const char *name): QLabel(parent, name) {};

	protected:
		void mousePressEvent (QMouseEvent * e);
};

class KaduSlots : public QObject
{

    Q_OBJECT
    
	public slots:
	    void onCreateConfigDialog();
	    void onDestroyConfigDialog();
};

extern QApplication *a;
extern Kadu *kadu;

extern QSocketNotifier* kadusnr;
extern QSocketNotifier* kadusnw;
extern QSocketNotifier* dccsnr;
extern QSocketNotifier* dccsnw;

extern QPopupMenu* statusppm;
extern QPopupMenu* dockppm;

extern struct gg_session* sess;
extern bool userlist_sent;
extern bool socket_active;
extern int last_read_event;
extern int server_nr;
extern bool timeout_connected;
extern struct gg_login_params loginparams;
extern QTimer *pingtimer;
extern QHostAddress config_dccip;
extern QHostAddress config_extip;
extern QValueList<QHostAddress> config_servers;

#endif
