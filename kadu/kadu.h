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
#include <qfile.h>

#include "misc.h"
#include "userbox.h"
#include "userlist.h"
#include "tabbar.h"
#include "dcc.h"
#include "status.h"

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
		KaduTextBrowser* descrtb;
		QMenuBar* MenuBar;
		QPopupMenu* MainMenu;
		KaduTabBar* GroupBar;
		UserBox* Userbox;

		int commencing_startup;
		bool updateChecked;
		bool Docked;

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
		virtual void resizeEvent(QResizeEvent *);
//		virtual void moveEvent(QMoveEvent *);

		struct gg_event *dcc_e;
		int dcc_ret;
		bool blinkOn;
	 
	public:
		Kadu(QWidget* parent=0, const char *name=0);
		~Kadu();
		static void InitModules();
		void changeAppearance();
		bool userInActiveGroup(UinType uin);
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
		/**
			Modu³ dokowania powinien to ustawic, aby kadu
			wiedzialo, ze jest zadokowane.
		**/
		void setDocked(bool docked);
		/**
			Zwraca true je¶li kadu jest zadokowane.
		**/
		bool docked();

		// te zmienne s± tylko chwilowo publiczne.
		// trzeba to uporz±dkowaæ
		bool autohammer;
		bool doBlink;
		
		//potrzebne dla modu³u dokuj±cego ¿eby g³ówne okno nie miga³o przy starcie...
		bool showMainWindowOnStart;

	private slots:
		void connected();
		void connecting();
		void connectionError(GaduConnectionError);
		void dccSetupFailed();
		void disconnected();
		void systemMessageReceived(QString &);
	
	public slots:
		void blink();
		void dccFinished(dccSocketClass *dcc);
		void slotHandleState(int command);
		void setCurrentStatus(int status);
		void sendMessage(QListBoxItem *);
		void dataReceived();
		void dataSent();
		void dccReceived();
		void dccSent();
		void pingNetwork(void);
		void setStatus(int);
		void disconnectNetwork(void);
		void gotUpdatesInfo(const QByteArray &data, QNetworkOperation *op);
		void currentChanged(QListBoxItem *item);
		void showdesc(bool show = true);
		void statusMenuAboutToHide(void);
		void mouseButtonClicked(int, QListBoxItem *);
		void infopanelUpdate(UinType);
		virtual bool close(bool quit = false);
		void quitApplication();

		// protezy...
		void proteza_statusChanged(int);
		void proteza_disconnectNetwork();

		void about();
		void addUserAction();
		void blockUser();
		void configure();
		void changePassword();
		void deleteHistory();
		void deleteUsers();
		void help();
		void hideKadu();
		void ignoreUser();
		void importExportUserlist();
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
		void showUserInfo();
		void unregisterUser();
		void viewHistory();
		void popupMenu();
		void show();
/*		void showMinimized();
		void hide();*/

	signals:
		void disconnectingNetwork();
		void disconnectedNetwork();
		void keyPressed(QKeyEvent* e);
		/**
			Podczas ³±czenia (mruganie ikonki) czas na zmianê
			ikony na t± reprezentuj±c± docelowy status.
		**/
		void connectingBlinkShowStatus(int status);
		/**
			Podczas ³±czenia (mruganie ikonki) czas na zmianê
			ikony na t± reprezentuj±c± status offline.
		**/		
		void connectingBlinkShowOffline();
		/**
			Zmieni³ siê nasz aktualny status GG.
		**/
		void currentStatusChanged(int status);
		/**
			U¿ytkownik chce zmieniæ status. Je¶li jest to status
			bezopisowy, to zmienna sigDesc bêdzie równa QString::null.
			Je¶li zostanie ustawiona zmienna stop, to status nie
			bêdzie zmieniony.
		**/
		void changingStatus(int status, QString sigDesc, bool &stop);
		/**
			wywo³ana zosta³a funkcja show() na g³ównym oknie
		**/
		void shown();
		/**
			wci¶niêty przycisk minimalizacji g³ównego okna
			póki co nie dzia³a
		**/
//		void minimized();
};

class KaduSlots : public QObject
{
	Q_OBJECT

	public slots:
		void onCreateConfigDialog();
		void onDestroyConfigDialog();
		void chooseColor(const char *name, const QColor& color);
		void chooseFont(const char *name, const QFont& font);
		void updatePreview();
};

extern Kadu* kadu;

extern QPopupMenu* statusppm;
extern QPopupMenu* dockppm;

extern int lockFileHandle;
extern QFile *lockFile;
#endif
