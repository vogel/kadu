#ifndef USERBOX_H
#define USERBOX_H

#include <qlistbox.h>
#include <qtooltip.h>
#include <qstringlist.h>
#include <qpainter.h>
#include <qpopupmenu.h>

#include "misc.h"

class KaduListBoxPixmap : public QListBoxPixmap {
        public:
                KaduListBoxPixmap(const QPixmap &pix, const QString &text);

        protected:
                void paint(QPainter *painter);
};

class UserBoxMenu : public QPopupMenu
{
    Q_OBJECT
    public:
	    UserBoxMenu(QWidget *parent=0, const char* name=0);
	    int getItem(const QString &caption);
	    int addItem(const QString &text, const QObject* receiver, const char* member, const QKeySequence accel= 0, int id= -1);
	    int addItem(const QString &iconname, const QString &text, const QObject* receiver, const char* member, const QKeySequence accel= 0, int id= -1);
    private slots:
	    void restoreLook();

    public slots:
	    void show(QListBoxItem *item);
    signals:
	    void popup();
};


/**
	Klasa reprezentuj±ca listê kontaktów wraz z ikonkami stanów
**/
class UserBox : public QListBox , QToolTip
{    
	Q_OBJECT

	private:
		static QValueList<UserBox*> UserBoxes;
		// Pixmaps
		static QPixmap* OnlineMobilePixmap;
		static QPixmap* OnlineDescMobilePixmap;
		static QPixmap* BusyMobilePixmap;
		static QPixmap* BusyDescMobilePixmap;
		static QPixmap* OfflineMobilePixmap;
		static QPixmap* OfflineDescMobilePixmap;		
		//
		QStringList Users;
		void sortUsersByAltNick(QStringList &);
			
	protected:
		virtual void maybeTip(const QPoint&);
		virtual void mousePressEvent(QMouseEvent *e);
		virtual void mouseMoveEvent(QMouseEvent* e);
		virtual void keyPressEvent(QKeyEvent *e);

	public:
		UserBox(QWidget* parent=0,const char* name=0,WFlags f=0);
		~UserBox();
		static UserBoxMenu *userboxmenu;
		virtual void clear() { QListBox::clear(); };
		void clearUsers() { Users.clear(); };
		void refresh();
		void addUser(const QString &altnick);
		void removeUser(const QString &altnick);
		void renameUser(const QString &oldaltnick, const QString &newaltnick);
		void changeAllToInactive();
		// Functions below works on all created userboxes
		static void all_refresh();
		static void all_removeUser(QString &altnick);		
		static void all_changeAllToInactive();
		static void all_renameUser(const QString &oldaltnick, const QString &newaltnick);		
		static void initModule();
		UinsList getSelectedUins();
		UserList getSelectedUsers();
		QStringList getSelectedAltNicks();

	public slots:	
		void showHideInactive();

};


class UserBoxSlots : public QObject
{
	Q_OBJECT
	private:
		QValueList<QColor> vl_userboxcolor;
		QValueList<QFont> vl_userboxfont;

	public slots:
		void onCreateConfigDialog();
		void onDestroyConfigDialog();
		void chooseColorGet(const QColor& color);
		void chooseColorGet(const QString& text);
		void chooseUserBoxSelect(int nr);
		void chooseUserBoxFont(int nr);
		void chooseUserBoxFontSize(int nr);
};

#endif
