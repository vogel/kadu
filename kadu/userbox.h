#ifndef USERBOX_H
#define USERBOX_H

#include <qlistbox.h>
#include <qtooltip.h>
#include <qstringlist.h>
#include <qpainter.h>
#include <qpopupmenu.h>

#include "misc.h"

class KaduListBoxPixmap : public QListBoxItem {
        public:
                KaduListBoxPixmap(const QPixmap &pix, const QString &text);
                KaduListBoxPixmap(const QPixmap &pix, const QString &text, const QString &descr, bool bold);

		QString description() const { return descr; }
		bool isBold() const { return bold; }
		int height(const QListBox *lb) const;
		int width(const QListBox *lb) const;

        protected:
                void paint(QPainter *painter);
		void setDescription(const QString &d) { descr = d; }
		void setBold(bool b) { bold = b; }

	private:
		QPixmap pm;
		QString descr;
		bool bold;
};

class UserBoxMenu : public QPopupMenu
{
    Q_OBJECT
    public:
	    UserBoxMenu(QWidget *parent=0, const char* name=0);
	    int getItem(const QString &caption);
	    int addItem(const QString &text, const QObject* receiver, const char* member, const QKeySequence accel= 0, int id= -1);
	    /**
	    	@param iconname nazwa ikony z zestawu lub sciezka do pliku
	    **/
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
		/**
			Funkcja czy¶ci ca³± listê u¿ytkowników w UserBox
		**/
		void clearUsers() { Users.clear(); };
		void refresh();
		/**
			Funkcja dodaje u¿ytkownika o podanym altnick do listy w UserBox
		**/
		void addUser(const QString &altnick);
		/**
			Funkcja usuwa u¿ytkownika o podanym altnick z listy w UserBox
		**/
		void removeUser(const QString &altnick);
		/**
			Funkcja zmienia nazwe altnick z podanej na inna w liscie UserBox
		**/
		void renameUser(const QString &oldaltnick, const QString &newaltnick);
		/**
			Funkcja sprawdza czy dany altnick, znajduje siê na liscie w UserBox
		**/
		bool containsAltNick(const QString &altnick);
		void changeAllToInactive();
		// Functions below works on all created userboxes
		static void all_refresh();
		static void all_removeUser(QString &altnick);		
		static void all_changeAllToInactive();
		static void all_renameUser(const QString &oldaltnick, const QString &newaltnick);		
		static void initModule();
		/**
		    Funkcja zwraca liste zaznaczonych uzytkownikow 
		    @return UinsList z zaznaczonymi 
		    uzytkownikami posiadajacymi numer GG
		**/

		UinsList getSelectedUins();
		/**
		    Funkcja zwraca liste zaznaczonych uzytkownikow
		    @return UserList z zaznaczonymi uzytkownikami
		**/

		UserList getSelectedUsers();
		/**
		    Funkcja zwraca liste zaznaczonych uzytkownikow
		    @return QStringList z AltNick'ami
		**/
		QStringList getSelectedAltNicks();
		/**
		    Funkcja znajdujaca aktywny UserBox.
		    @return wskaznik do aktywnego UserBox'a, 
		    jesli taki nie istnieje zwracana jest wartosc NULL
		**/
		static UserBox* getActiveUserBox();
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
