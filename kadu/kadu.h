#ifndef KADU_H
#define KADU_H

#include <qapplication.h>
#include <qframe.h>
#include <qlayout.h>
#include <qmainwindow.h>
#include <qpopupmenu.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qtoolbutton.h>
#include <qtoolbar.h>
#include <qvaluelist.h>

#include "misc.h"
#include "userbox.h"
#include "userlist.h"
#include "tabbar.h"
#include "status.h"

class QFile;
class QMenuBar;

/**
	Toolbar Kadu
**/
class ToolBar : public QToolBar
{
	private:
		struct ToolButton
		{
			QString iconname;
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
		static void registerButton(const QString &iconname, const QString& caption,
			QObject* receiver, const char* slot, const int position=-1, const char* name="");
		static void unregisterButton(const char* name);
		static void registerSeparator(int position=-1);
		static QToolButton* getButton(const char* name);
		static void refreshIcons(const QString &caption=QString::null, const QString &newIconName=QString::null, const QString &newCaption=QString::null);
};

/**
	G³ówne okno Kadu
**/
class Kadu : public QMainWindow
{
	Q_OBJECT

	private:
		friend class KaduSlots;
		KaduTextBrowser* InfoPanel;
		QMenuBar* MenuBar;
		QPopupMenu* MainMenu;
		KaduTabBar* GroupBar;
		UserBox* Userbox;

		QPopupMenu* statusMenu;
		QPushButton* statusButton;
		
		UserStatus status;

		bool ShowMainWindowOnStart;
		bool DoBlink;
		bool BlinkOn;
		bool Docked;
		int lastId;

		void createMenu();
		void createToolBar();
		void createStatusPopupMenu();
		void setActiveGroup(const QString& group);

		void showStatusOnMenu(int);

	public slots:
		void show();
		void mouseButtonClicked(int, QListBoxItem *);
		void infopanelUpdate(UinType);
		void currentChanged(QListBoxItem *item);
		void sendMessage(const QString &to);
		void configure();

	private slots:
		void userListModified();
		void openChat();
		void chatMsgReceived(UinsList senders, const QString &msg, time_t time);
		void userListChanged();

		void wentOnline(const QString &);
		void wentBusy(const QString &);
		void wentInvisible(const QString &);
		void wentOffline(const QString &);
		void groupTabSelected(int id);
		void connected();
		void connecting();
		void disconnected();
		void imageReceivedAndSaved(UinType sender, uint32_t size, uint32_t crc32, const QString &path);
		void systemMessageReceived(const QString &msg);
		void userStatusChanged(const UserListElement &, const UserStatus &oldstatus, bool onConnection);
		void deleteOldConfigFiles();

	protected:
		void keyPressEvent(QKeyEvent *e);
		virtual void resizeEvent(QResizeEvent *);
//		virtual void moveEvent(QMoveEvent *);

	public:
		Kadu(QWidget* parent=0, const char *name=0);
		~Kadu();
		bool userInActiveGroup(UinType uin);
		void removeUser(const QStringList &, bool);

		/**
			Zwraca wskaznik do belki menu glownego okna.
		**/
		QMenuBar* menuBar() const;

		/**
			Zwraca wskaznik do glownego menu programu.
		**/
		QPopupMenu* mainMenu() const;

		/**
			Zwraca wskaznik do zakladek z nazwami grup.
		**/
		KaduTabBar* groupBar() const;

		/**
			Zwraca wskaznik do userbox-a w glownym oknie.
		**/
		UserBox* userbox() const;

		/**
			Zwraca true je¶li kadu jest zadokowane.
		**/
		bool docked() const;

		/**
			zwraca nazwê aktualnie wybranej grupy
		**/
		QString currentGroup() const;

		/**
		**/
		void startupProcedure();

		/**
		**/
		int personalInfoMenuId;//potrzebne dla modu³u account_management

		/**
			w zwi±zku z tym, ¿e opis sk³adni dla parsera jest u¿ywany w kilku miejscach
			dodane zosta³o to pole, ¿eby nie trzeba by³o zmieniaæ tekstu w kilku miejscach
		**/
		static const QString SyntaxText;
	public slots:
		void slotHandleState(int command);
		void changeAppearance();
		void blink();
		void refreshGroupTabBar();
		void showdesc(bool show = true);
		void statusMenuAboutToHide(void);
		virtual bool close(bool quit = false);
		void quitApplication();

		/**
			Potrzebne dla modu³u dokuj±cego ¿eby
			g³ówne okno nie miga³o przy starcie...
		**/
		void setShowMainWindowOnStart(bool show);

		/**
			Modu³ dokowania powinien to ustawic, aby kadu
			wiedzialo, ze jest zadokowane.
		**/
		void setDocked(bool docked);

		void about();
		void addUserAction();
		void blockUser();
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
		void searchInDirectory();
		void showUserInfo();
		void viewHistory();
		void popupMenu();

		// odczytuje z obrazka tekst i zapisuje go w drugim parametrze
		void readTokenValue(QPixmap, QString &);

	signals:
		void disconnectingNetwork();
		void disconnectedNetwork();
		void keyPressed(QKeyEvent* e);
		void statusPixmapChanged(QPixmap &);

		/**
			U¿ytkownik chce zmieniæ status. Je¶li jest to status
			bezopisowy, to zmienna sigDesc bêdzie równa QString::null.
			Je¶li zostanie ustawiona zmienna stop, to status nie
			bêdzie zmieniony.
		**/
		void changingStatus(UserStatus &, bool &stop);

		/**
			wywo³ana zosta³a funkcja show() na g³ównym oknie
		**/
		void shown();

		/**
			wyst±pi³ b³±d po³±czenia
			TODO: wywaliæ po 0.4, odpowiedni sygna³ jest w GaduProtocol
		**/
		void connectionError(const QString &reason);

		/**
			u¿ywany przez modu³ hints do zdobycia pozycji traya od modu³u docking
			TODO: trzeba wymy¶liæ jaki¶ elegancki sposób na komunikacjê pomiêdzy modu³ami, które nie zale¿± od siebie
		**/
		void searchingForTrayPosition(QPoint &point);
};

class KaduSlots : public QObject
{
	Q_OBJECT

	public:
		KaduSlots(QObject *parent=0, const char *name=0);
	public slots:
		void onCreateConfigDialog();
		void onDestroyConfigDialog();
		void chooseColor(const char *name, const QColor& color);
		void chooseFont(const char *name, const QFont& font);
		void updatePreview();
};

extern Kadu* kadu;
extern QPopupMenu* dockMenu;
extern int lockFileHandle;
extern QFile *lockFile;

#endif
