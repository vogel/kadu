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
#include <qtoolbar.h>
#include <qmenubar.h>
#include <qvaluelist.h>

#include "libgadu.h"
#include "misc.h"
#include "status.h"
#include "userbox.h"
#include "userlist.h"
#include "tabbar.h"
#include "dcc.h"

/**
	Toolbar Kadu
**/
class ToolBar : public QToolBar
{
	private:
		struct ToolButton
		{
			QIconSet iconfile;
			QString caption, name;
			QObject* receiver;
			QString slot;
			QToolButton* button;
			int position;
		};
		static QValueList<ToolButton> RegisteredToolButtons;
		void createControls();
	public:
		static ToolBar* instance;
		ToolBar(QMainWindow* parent);
		~ToolBar();
		static void registerButton(const QIconSet& iconfile, const QString& caption, 
		    QObject* receiver, const char* slot, const int position=-1, const char* name="");
		static void unregisterButton(const char* name);
		static void registerSeparator(int position=-1);
		static QToolButton* getButton(const char* name);
};

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
		QMenuBar* MenuBar;
		QPopupMenu* MainMenu;
		KaduTabBar* GroupBar;
		UserBox* Userbox;
		
		int commencing_startup;

		void createMenu();
		void createToolBar();
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
		bool blinkOn;
	 
	public:
		Kadu(QWidget* parent=0, const char *name=0);
		~Kadu();
		static void InitModules();
		void changeAppearance();
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
		void changePassword();
		void deleteHistory();
		void deleteUsers();
		void exportUserlist();
		void help();
		void hideKadu();
		void ignoreUser();
		void importUserlist();
		void lookupInDirectory();
		void manageIgnored();
		void notifyUser();
		void offlineToUser();
		void personalInfo();
		void quit();
		void registerUser();
		void remindPassword();
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

class StatusLabel : public QLabel {
	Q_OBJECT
	public:
		StatusLabel(QWidget *parent, const char *name): QLabel(parent, name) {};

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

extern Kadu* kadu;

extern QPopupMenu* statusppm;
extern QPopupMenu* dockppm;

#endif
